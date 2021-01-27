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

HMP3Decoder p2mp3decoder;

void init_audio_player(void){
	p2mp3decoder = MP3InitDecoder();
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
	unsigned char **inbuf;
	int *bytesLeft;
	short *outbuf;
	int useSize;
	int a = MP3Decode(p2mp3decoder, inbuf, bytesLeft, outbuf, useSize);

	//1)Turn on a decoding flag...

	//2)Start decoding the first chunk (think in a CHUNK_WORDS_LEN and in WORD_LEN),
	//accesing to the encoded record and calling MP3decode.

	//3)Trigger first dma request and enable major loop dma link.

}


playerStatus_t get_player_status(void){

}


void continue_playing(void){

}


audioResult_t stop_playing(void){

}
