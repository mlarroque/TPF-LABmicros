/*
 * audioPlayer.c
 *
 *  Created on: 26 ene. 2021
 *      Author: Matias
 */

#include "audioPlayer.h"
#include "flashHal.h"
#include "fsl_edma.h"
#include "fsl_sai.h"
/*
void init_audio_player(void){

}
*/


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

}


void start_playing(audioTag_t tag, audioFormat_t audioInputFormat, audioFormat_t audioOutputFormat){

}


playerStatus_t get_player_status(void){

}


void continue_playing(void){

}


audioResult_t stop_playing(void){

}
