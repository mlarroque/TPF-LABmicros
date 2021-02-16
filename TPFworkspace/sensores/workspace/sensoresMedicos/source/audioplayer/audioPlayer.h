/*
 * audioPlayer.h
 *
 *  Created on: 26 ene. 2021
 *      Author: Matias
 */

#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

#include "sai_edma_hal.h"

typedef enum {ALERTA_0, ALERTA_1} audioTag_t;
typedef enum {AUDIO_SUCCES, AUDIO_ERROR} audioResult_t;
typedef enum {AUDIO_PENDING, AUDIO_NOT_PENDING, AUDIO_SENDING} playerStatus_t;
typedef enum {AUDIO_MP3, AUDIO_I2S_STEREO_DECODED} audioFormat_t;
typedef enum {AUDIO_IDLE, AUDIO_PROCESSING} audioStatus_t;



typedef struct{
	char * p2audioData;
	int audioDataLen;
	audioTag_t audioTag;
	audioFormat_t audioFormat;
}audioData_t;

/*@brief init_audio_player:
 *initialize audio player tools..
 */
void init_audio_player(sai_edma_callback_t callback, void *userData);

void init_audio_RTOS(void);

/*@brief save_record:
 * This function save an audio record in a non-volatile,
 * and manage the configuration necessary to play the record later.
 * @param audioData_t *: pointer to audioData, in which the user points the
 * 			pointer to audio data buffer, its length, a tag that identify audio record
 * 			and the audio format type (encodedFormat is recomended).
 *
 * @retval  audioResult_t return.
 */
audioResult_t save_record(audioData_t * audioData);


/*@brief save_record:
 * This function read from non-volatile memory a record previously recorded, to a user structure.
 *
 * @param audioData_t *: pointer to audioData user structure, in which the function fill
 * 					the audioData. The function needs that the user specify
 * 					the audioTag_t field in the structure.
 *
 * @retval  audioResult_t return, if SUCCES then audioData was filled with record data.
 */

audioResult_t read_record(audioData_t * audioData);

/*@brief start_playing:
 * This function starts sending an audio previously recorded to output pins.
 *
 *
 * @param audioData_t *: pointer to audioData user structure, in which the function fill
 * 					the audioData. The function needs that the user specify
 * 					the audioTag_t field in the structure and a format to ouptput playing.
 *
 * @retval  none.
 */
void start_playing(audioTag_t tag, audioFormat_t audioInputFormat, audioFormat_t audioOutputFormat);

/*@brief get_player_status:
 * This function verify if there is something pending to play.
 *
 *@param none
 *
 * @retval  playerStatus_t that indicate if there is some playing pending.
 */
audioStatus_t get_player_status(void);

/*@brief stop_playing:
 * This function force a break in audio playing.
 *
 *@param none.
 *
 * @retval  audioResult to indicate success or error.
 */

void stop_playing(void);

void free_audio_player(void);

#endif /* AUDIOPLAYER_H_ */
