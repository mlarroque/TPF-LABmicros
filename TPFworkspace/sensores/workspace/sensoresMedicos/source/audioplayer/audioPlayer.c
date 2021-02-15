/*
 * audioPlayer.c
 *
 *  Created on: 26 ene. 2021
 *      Author: Matias
 */

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
/*extern const edma_config_t DMA_config;
extern const sai_config_t I2S0_tx_config;
extern sai_transfer_format_t I2S0_tx_format;
extern edma_handle_t I2S0_TX_Handle;
extern sai_edma_handle_t I2S0_SAI_Tx_eDMA_Handle;*/


#include "audioPlayer.h"
#include "mp3dec.h"
#include "flashHal.h"
#include "UDA.h"
#include "fsl_debug_console.h"

#define MAIN_CALLBACK_SAI 1

#define WORD_LEN 2   //2bytes
#define N_CHUNKS 64
#define CHUNK_WORD_LEN ((STREAM_LEN/WORD_LEN)/N_CHUNKS)
#define CHUNK_BYTE_LEN (STREAM/N_CHUNKS)
//#define PP_BUFFER_LEN (CHUNK_WORD_LEN * WORD_LEN * 2)
#define PP_BUFFER_LEN 1152 * 2

#define USE_SIZE_HELIX 0

short audio_pp_buffer[PP_BUFFER_LEN];  //PING PONG BUFFER

int ppBufferWrite = 0;
int ppBufferRead = 0;

int audioStatus;

int measure_decoding = 1;
int debug_counter = 0;

int bytesLeft;

int mp3dataLen = STREAM_LEN;

unsigned char * p2mp3record = 0;

HMP3Decoder p2mp3decoder;
MP3FrameInfo mp3FrameInfo;

codec_handle_t codecHandle = {0};
extern codec_config_t boardCodecConfig;

sai_transfer_t xfer;

void counterCallback(void);
int decode_chunk_mp3(short * audio_pp_pointer);
void continue_playing(void);
void callbackSAI(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

void init_audio_player(sai_edma_callback_t userCallback, void *userData){
	if (userCallback != 0){
		sai_edma_init(userCallback, userData);
	}
	else{
		sai_edma_init(callbackSAI, userData);
	}

	p2mp3decoder = MP3InitDecoder();
	audioStatus = AUDIO_IDLE;
}

void init_audio_RTOS(void){
	uint32_t delayCycle = 500000U;

	/* Use default setting to init codec */
	CODEC_Init(&codecHandle, &boardCodecConfig);  //fsl_codec_common
	CODEC_SetFormat(&codecHandle, getMasterClockHz(), getSampleRateHz(), getBitWidth());
	while (delayCycle)
	{
	  __ASM("nop");
	  delayCycle--;
	}
}

void free_audio_player(void){
	MP3FreeDecoder(p2mp3decoder);
	stop_playing();
	sai_deinit();
}

audioResult_t save_record(audioData_t * audioData){
	audioResult_t result = AUDIO_ERROR;

	if( (audioData->p2audioData != 0) &&
			(audioData->audioDataLen <= STREAM_LEN) &&
			(flashINIT()  != -1) &&
			(flashAlloc( (int *) audioData->p2audioData, (int) audioData->audioDataLen, audioData->audioTag) != -1) ){

		result = AUDIO_SUCCES;

	}

	return result;
}

audioResult_t read_record(audioData_t * audioData){
	audioResult_t result = AUDIO_ERROR;

	if( (flashINIT()  != -1)){

		audioData->p2audioData = readFlash(&(audioData->audioDataLen), audioData->audioTag);
		if (audioData->p2audioData != 0){

			result = AUDIO_SUCCES;
		}
	}

	return result;
}

void start_playing(audioTag_t tag, audioFormat_t audioInputFormat, audioFormat_t audioOutputFormat){
	printf("START PLAYING\n");

	sai_edma_reset(callbackSAI, NULL);

	p2mp3record = 0;

	if( (audioInputFormat == AUDIO_MP3) && (audioOutputFormat == AUDIO_I2S_STEREO_DECODED) && (audioStatus ==AUDIO_IDLE)){

		p2mp3record = (unsigned char *) readFlash(&mp3dataLen, tag); //obtain mp3 pointer

		bytesLeft = STREAM_LEN;  //It´s important to initialize this global variable.
		ppBufferRead = 0;

		if (decode_chunk_mp3(audio_pp_buffer) != -1){
			xfer.data = (uint8_t *) audio_pp_buffer;
			xfer.dataSize = mp3FrameInfo.outputSamps * 2;
			sendSAIdata(&xfer);
			audioStatus = AUDIO_PROCESSING;  //Turn on flag
			//enable DMArequest (FIFO I2S triggers DMA)
			//assure that first DMA transfer can start at this point!
			ppBufferWrite = (int)(PP_BUFFER_LEN/2);
			//continue_playing();

		}
		else{
			stop_playing();
		}

	}


}


audioStatus_t get_player_status(void){
	return audioStatus;
}


void continue_playing(void){
	if ((audioStatus == AUDIO_PROCESSING) && (p2mp3record != 0) && (decode_chunk_mp3(audio_pp_buffer + ppBufferWrite) == 0)){
		//trigger next DMA?? no debería hacer falta...

		ppBufferRead = ppBufferWrite;
		if (ppBufferWrite == 0){  //se acomoda la proxima posición a escribir en el ping pong buffer...
			ppBufferWrite = (int)(PP_BUFFER_LEN/2);
		}
		else{
			ppBufferWrite = 0;
		}
	}
	else{
		audioStatus = AUDIO_IDLE;
		stop_playing();
	}
}


void stop_playing(void){
	audioStatus = AUDIO_IDLE;
	p2mp3record = 0;
	ppBufferWrite = 0;
	stopSAIsend();
	//desabilitar DMA REQUEST

}


int decode_chunk_mp3(short * audio_pp_pointer){
	int ret = -1;
	//obtain next sync word
	int offset = MP3FindSyncWord(p2mp3record, bytesLeft);
	bytesLeft -= offset; //refresh bytes left to decode
	p2mp3record += offset;
	//printf("decoding: bytesLeft: %d\n", bytesLeft);
	while((MP3GetNextFrameInfo(p2mp3decoder, &mp3FrameInfo, p2mp3record) == ERR_MP3_INVALID_FRAMEHEADER) && (offset != -1)){
		//PRINTF("	ENTRO EN WHILE \n");
		offset = MP3FindSyncWord(p2mp3record + 1, bytesLeft);
		bytesLeft -= (offset + 1); //refresh bytes left to decode
		p2mp3record += (offset + 1);
	}

	//if there is something to decode and the decoding is ok...
	//the decoding starts with the sync word and the result is save in a ping pong buffer..
	//who use this function have to suit the pointer to buffer having into acount ping pong buffer logic..
	int result_decoding = MP3Decode(p2mp3decoder, &p2mp3record, &bytesLeft, audio_pp_pointer, USE_SIZE_HELIX);
	if ( (offset != -1) && ( result_decoding == ERR_MP3_NONE) ){

		MP3GetLastFrameInfo(p2mp3decoder, &mp3FrameInfo);
		ret = 0;
	}

	return ret;
}

void callbackSAI(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData){
	if(audioStatus != AUDIO_IDLE){

		xfer.data = (uint8_t *)  audio_pp_buffer + ppBufferRead;
		xfer.dataSize = mp3FrameInfo.outputSamps * 2;
		sendSAIdata(&xfer);
		continue_playing();
	}
	else{
		stop_playing();
	}
}

void counterCallback(void){
	debug_counter++;
}

