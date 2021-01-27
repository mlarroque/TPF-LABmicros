/*
 * audioPlayer.c
 *
 *  Created on: 26 ene. 2021
 *      Author: Matias
 */

#include "audioPlayer.h"
#include "mp3dec.h"
#include "flashHal.h"
#include "fsl_edma.h"
#include "fsl_sai.h"

#define WORD_LEN 4   //4bytes
#define N_CHUNKS 64
#define CHUNK_WORD_LEN ((STREAM_LEN/WORD_LEN)/N_CHUNKS)

short audio_pp_buffer[CHUNK_WORD_LEN * WORD_LEN * 2];  //PING PONG BUFFER

int audioStatus;

HMP3Decoder p2mp3decoder;

void init_audio_player(void){
	p2mp3decoder = MP3InitDecoder();
	audioStatus = AUDIO_IDLE;
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

	int errorCode = 0; //0 MEANS NO ERROR CODE...

	int mp3dataLen = 0;
	char * p2mp3record = 0;

	int bytesLeft;
	int useSize = 0; //MODE XXX

	if( (audioInputFormat == AUDIO_MP3) && (audioOutputFormat == AUDIO_I2S_STEREO_DECODED) ){

		audioStatus = AUDIO_PROCESSING;   //Turn on flag

		p2mp3record = readFlash(&mp3dataLen, tag); //obtain mp3 pointer

		erroCode = MP3Decode(p2mp3decoder, p2mp3record, &bytesLeft, audio_pp_buffer, useSize); //decode the first chunk
																		//saving at starting point of ping pong buffer.

		//enable software and hardware DMA request with link in major loop (ping pong buffer)
		//software trigger to start transfer.
		//disable software requests???

	}


}


playerStatus_t get_player_status(void){

}


void continue_playing(void){

}


audioResult_t stop_playing(void){

}
