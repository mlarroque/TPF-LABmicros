/*
 * audioPlayer.c
 *
 *  Created on: 26 ene. 2021
 *      Author: Matias
 */

#include "peripherals.h"
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
//#include "fsl_edma.h"
#include "fsl_sai_edma.h"
#include "timer.h"


#define WORD_LEN 4   //4bytes
#define N_CHUNKS 64
#define CHUNK_WORD_LEN ((STREAM_LEN/WORD_LEN)/N_CHUNKS)
#define CHUNK_BYTE_LEN (STREAM/N_CHUNKS)
#define PP_BUFFER_LEN (CHUNK_WORD_LEN * WORD_LEN * 2)

#define USE_SIZE_HELIX 0

short audio_pp_buffer[PP_BUFFER_LEN];  //PING PONG BUFFER

int ppBufferWrite = 0;
int ppBufferRead = 0;

int audioStatus;


int bytesLeft;

int mp3dataLen = STREAM_LEN;

unsigned char * p2mp3record = 0;

HMP3Decoder p2mp3decoder;
MP3FrameInfo mp3FrameInfo;


sai_transfer_t xfer;


/****************************************/
/*@brief continue_playing:
 * This function continue the process of playing a record. If some audio is pending, this function
 * will change the status to sending audio, but not necessary will send all the audio pending.
 *
 *@param none.
 *
 * @retval  none

int decode_chunk_mp3(short * audio_pp_pointer);

/*@brief continue_playing:
 * This function continue the process of playing a record. If some audio is pending, this function
 * will change the status to sending audio, but not necessary will send all the audio pending.
 *
 *@param none.
 *
 * @retval  none.
 */

int decode_chunk_mp3(short * audio_pp_pointer);
void continue_playing(void);

void init_audio_player(void){

	p2mp3decoder = MP3InitDecoder();
	audioStatus = AUDIO_IDLE;
	InitializeTimers();
}

void free_audio_player(void){
	MP3FreeDecoder(p2mp3decoder);
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

	p2mp3record = 0;

	if( (audioInputFormat == AUDIO_MP3) && (audioOutputFormat == AUDIO_I2S_STEREO_DECODED) && (audioStatus ==AUDIO_IDLE)){

		p2mp3record = (unsigned char *) readFlash(&mp3dataLen, tag); //obtain mp3 pointer

		bytesLeft = STREAM_LEN;  //It´s important to initialize this global variable.


		if (decode_chunk_mp3(audio_pp_buffer) != -1){
			xfer.data = audio_pp_buffer;
			xfer.dataSize = mp3FrameInfo.outputSamps / 2;
			SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, &xfer);
			audioStatus = AUDIO_PROCESSING;  //Turn on flag
			//enable DMArequest (FIFO I2S triggers DMA)
			//assure that first DMA transfer can start at this point!
			ppBufferWrite = (int)(PP_BUFFER_LEN/2);

			SetTimer(AUDIO_PLAYER_TIMER,20, continue_playing);

		}
		else{
			stop_playing();
		}

	}


}


playerStatus_t get_player_status(void){
	return audioStatus;
}


void continue_playing(void){
	DisableTimer(AUDIO_PLAYER_TIMER);
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
	}
}


void stop_playing(void){
	audioStatus = AUDIO_IDLE;
	p2mp3record = 0;
	ppBufferWrite = 0;
	DisableTimer(AUDIO_PLAYER_TIMER);
	//desabilitar DMA REQUEST

}


int decode_chunk_mp3(short * audio_pp_pointer){
	int ret = -1;
	//obtain next sinc word
	int offset = MP3FindSyncWord(p2mp3record, bytesLeft);
	bytesLeft -= offset; //refresh bytes left to decode
	p2mp3record += mp3dataLen - bytesLeft;
	//if there is something to decode and the decoding is ok...
	//the decoding starts with the sync word and the result is save in a ping pong buffer..
	//who use this function have to suit the pointer to buffer having into acount ping pong buffer logic..
	if ( (offset != -1) && (MP3Decode(p2mp3decoder, &p2mp3record, &bytesLeft, audio_pp_pointer, USE_SIZE_HELIX) == 0) ){
		MP3GetLastFrameInfo(p2mp3decoder, &mp3FrameInfo);

		ret = 0;
	}
	return ret;
}


void finish_TX_DMA_SAI_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData){
	if(audioStatus != AUDIO_IDLE){
		xfer.data = audio_pp_buffer + ppBufferRead;
		xfer.dataSize = mp3FrameInfo.outputSamps / 2;
		SAI_TransferSendEDMA(base, handle, &xfer);
		SetTimer(AUDIO_PLAYER_TIMER,20, continue_playing);
	}
	else{
		stop_playing();
	}
}
