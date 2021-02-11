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

#include "debug_definitions.h"
#include "audioPlayer.h"
#include "codec_config_hal.h"
#include "mp3dec.h"
#include "flashHal.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_sai_edma.h"
#include "fsl_sai.h"
#include "timer.h"
#include "board.h"
//#include "GrabacionEmergencia_wavarray.h"
#include "fsl_debug_console.h"
#define MAIN_CALLBACK_SAI 0

#define DEMO_SAI I2S0
#define DEMO_I2C I2C0
#define DEMO_SAI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_SAI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_I2C_CLKSRC kCLOCK_BusClk
#define DEMO_I2C_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define EXAMPLE_DMA (DMA0)
#define EXAMPLE_CHANNEL (0U)
#define EXAMPLE_SAI_TX_SOURCE kDmaRequestMux0I2S0Tx
#define OVER_SAMPLE_RATE (384U)

#define WORD_LEN 4   //4bytes
#define N_CHUNKS 64
#define CHUNK_WORD_LEN ((STREAM_LEN/WORD_LEN)/N_CHUNKS)
#define CHUNK_BYTE_LEN (STREAM/N_CHUNKS)
#define PP_BUFFER_LEN (CHUNK_WORD_LEN * WORD_LEN * 2 * 2)

#define USE_SIZE_HELIX 0
#define TIME_POLL_DECODE 2

short audio_pp_buffer[PP_BUFFER_LEN];  //PING PONG BUFFER

int ppBufferWrite = 0;
int ppBufferRead = 0;

int audioStatus;

int poll_counter_debug =  0;
int audioFormat;

int bytesLeft;

int mp3dataLen = STREAM_LEN;

unsigned char * p2mp3record = 0;

HMP3Decoder p2mp3decoder;
MP3FrameInfo mp3FrameInfo;

AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t audio_txHandle) = {0};
edma_handle_t audio_dmaHandle = {0};

sai_transfer_t xfer;

/****************************************/
/*@brief continue_playing:
 * This function continue the process of playing a record. If some audio is pending, this function
 * will change the status to sending audio, but not necessary will send all the audio pending.
 *
 *@param none.
 *
 * @retval  none
 *
 *
 *
 *
 *@brief continue_playing:
 *
 * This function continue the process of playing a record. If some audio is pending, this function
 * will change the status to sending audio, but not necessary will send all the audio pending.
 *
 *@param none.
 *
 * @retval  none.
 */
int decode_chunk_mp3(short * audio_pp_pointer);
void continue_playing(void);
void continue_playing_wav(void);
void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
void init_sai_com(void);

void init_audio_player(void){

	//init codec_hal
	init_sai_com();
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
		audioFormat = AUDIO_MP3;
		p2mp3record = (unsigned char *) readFlash(&mp3dataLen, tag); //obtain mp3 pointer

		bytesLeft = STREAM_LEN;  //It´s important to initialize this global variable.


		if (decode_chunk_mp3(audio_pp_buffer) != -1){
			xfer.data = audio_pp_buffer;
			xfer.dataSize = mp3FrameInfo.outputSamps * 2;
			if(SAI_TransferSendEDMA(DEMO_SAI, &audio_txHandle, &xfer) == 0)
			{
				audioStatus = AUDIO_PROCESSING;  //Turn on flag
				//enable DMArequest (FIFO I2S triggers DMA)
				//assure that first DMA transfer can start at this point!
				ppBufferWrite = (int)(PP_BUFFER_LEN/2);

				EnableTimer(AUDIO_PLAYER_TIMER);
				SetTimer(AUDIO_PLAYER_TIMER,TIME_POLL_DECODE, continue_playing);
			}
			else{
				printf("error sending\n");
				stop_playing();
			}


		}
		else if((audioInputFormat == AUDIO_WAV) && (audioOutputFormat == AUDIO_I2S_STEREO_DECODED) && (audioStatus ==AUDIO_IDLE)) {
			audioFormat = AUDIO_WAV;
			//p2mp3record = (unsigned char *) GrabacionEmergencia_wavarray;
			//bytesLeft = BUFF_LEN_WAV;  //It´s important to initialize this global variable.

			if(SAI_TransferSendEDMA(DEMO_SAI, &audio_txHandle, &xfer) == 0)
			{
				audioStatus = AUDIO_PROCESSING;  //Turn on flag
				ppBufferWrite = (int)(PP_BUFFER_LEN/2);

				EnableTimer(AUDIO_PLAYER_TIMER);
				SetTimer(AUDIO_PLAYER_TIMER,TIME_POLL_DECODE, continue_playing_wav);
			}
			else{
				printf("error sending\n");
				stop_playing();
			}
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
		poll_counter_debug++;

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

void continue_playing_wav(void){
	DisableTimer(AUDIO_PLAYER_TIMER);
	if ((audioStatus == AUDIO_PROCESSING) && (p2mp3record != 0) && (bytesLeft > 0)){
		//memcpy(audio_pp_buffer + ppBufferWrite, p2mp3record + BUFF_LEN_WAV - bytesLeft, 1152);
		bytesLeft -= 1152;
		poll_counter_debug++;

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
	SAI_TransferTerminateSendEDMA(I2S0, &audio_txHandle);
	deinit_audio_player();
	PRINTF("\n\r SAI EDMA example finished!\n\r ");
	printf("n decodifications: %d\n", poll_counter_debug);
	//desabilitar DMA REQUEST

}


int decode_chunk_mp3(short * audio_pp_pointer){
	int ret = -1;
	//obtain next sinc word
	int offset = MP3FindSyncWord(p2mp3record, bytesLeft);
	bytesLeft -= offset; //refresh bytes left to decode
	//p2mp3record += mp3dataLen - bytesLeft;
	p2mp3record += offset;

	while ( (offset != -1) && (MP3GetNextFrameInfo(p2mp3decoder, &mp3FrameInfo, p2mp3record) == ERR_MP3_INVALID_FRAMEHEADER) ){
		//ignore frame and continue with nextOne.
		offset = MP3FindSyncWord(p2mp3record  + 1, bytesLeft);
		bytesLeft -= offset; //refresh bytes left to decode
		//p2mp3record += mp3dataLen - bytesLeft;
		p2mp3record += (offset + 1);
	}
	//if there is something to decode and the decoding is ok...
	//the decoding starts with the sync word and the result is save in a ping pong buffer..
	//who use this function have to suit the pointer to buffer having into acount ping pong buffer logic..
	int result_decoding = MP3Decode(p2mp3decoder, &p2mp3record, &bytesLeft, audio_pp_pointer, USE_SIZE_HELIX);
	if ( (offset != -1) && ( result_decoding == 0) ){

		MP3GetLastFrameInfo(p2mp3decoder, &mp3FrameInfo);
		ret = 0;
	}


	return ret;
}
int debug_counter = 0;
#if !DEBUG_SAI_EDMA_TRANSFER_1
void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData){
	if(audioStatus != AUDIO_IDLE){
		xfer.data = audio_pp_buffer + ppBufferRead;
		xfer.dataSize = mp3FrameInfo.outputSamps * 2;
		if(SAI_TransferSendEDMA(base, handle, &xfer) == 0){

			SetTimer(AUDIO_PLAYER_TIMER,TIME_POLL_DECODE, continue_playing);
			debug_counter++;
			if (debug_counter > 3){
				debug_counter = 0;
			}
		}
		else{
			printf("error sending\n");
			stop_playing();
		}

	}
	else{
		stop_playing();
	}
}
#endif

void audio_player_PreConfig(void){
	BOARD_I2C_ReleaseBus();
}

void init_sai_com(void){

	sai_config_t config;
	uint32_t mclkSourceClockHz = 0U;
	sai_transfer_format_t format;
	edma_config_t dmaConfig = {0};


	EDMA_GetDefaultConfig(&dmaConfig);
	EDMA_Init(EXAMPLE_DMA, &dmaConfig);
	EDMA_CreateHandle(&audio_dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL);


	DMAMUX_Init(DMAMUX0);
	DMAMUX_SetSource(DMAMUX0, EXAMPLE_CHANNEL, EXAMPLE_SAI_TX_SOURCE);
	DMAMUX_EnableChannel(DMAMUX0, EXAMPLE_CHANNEL);


	/* Init SAI module */
	/*
	 * config.masterSlave = kSAI_Master;
	 * config.mclkSource = kSAI_MclkSourceSysclk;
	 * config.protocol = kSAI_BusLeftJustified;
	 * config.syncMode = kSAI_ModeAsync;
	 * config.mclkOutputEnable = true;
	 */
	SAI_TxGetDefaultConfig(&config);
	SAI_TxInit(DEMO_SAI, &config);

	/* Configure the audio format */
	format.bitWidth = kSAI_WordWidth16bits;
	format.channel = 0U;
	format.sampleRate_Hz = kSAI_SampleRate16KHz;
	format.masterClockHz = OVER_SAMPLE_RATE * format.sampleRate_Hz;
	format.protocol = config.protocol;
	format.stereo = kSAI_Stereo;
	format.isFrameSyncCompact = false;
	format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;

	/* Use default setting to init codec */
	set_config_audio_codec(format.masterClockHz, format.sampleRate_Hz, format.bitWidth);

	int delayCycle = 10000000U;
	while (delayCycle)
	{
	  __ASM("nop");
	  delayCycle--;
	}

	SAI_TransferTxCreateHandleEDMA(DEMO_SAI, &audio_txHandle, callback, NULL, &audio_dmaHandle);

	mclkSourceClockHz = DEMO_SAI_CLK_FREQ;
	SAI_TransferTxSetFormatEDMA(DEMO_SAI, &audio_txHandle, &format, mclkSourceClockHz, format.masterClockHz);

}

void deinit_audio_player(void){
	MP3FreeDecoder(p2mp3decoder);
}
