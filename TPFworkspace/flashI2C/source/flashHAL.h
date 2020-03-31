/*
 * flashHAL.h
 *
 *  Created on: 30 mar. 2020
 *      Author: G5
 */

#ifndef FLASHHAL_H_
#define FLASHHAL_H_

#define MAX_TAG_LEN 9
/*@brief Initialize program flash sector according to hardware features
 *
 * This function initialize flash memory and allow using other API functions.
 *
 * @param previousInfoFlag: -1 if there is no usable data in flash sector programmed before. != -1 in other case.
 * @retval #-1 if error. != -1 if success.
 */
int flashINIT(int previousInfoFlag);



/*@brief Program data to flash sector since the last address programmed
 * 			    or since the beggining if nothing was programmed yet.
 *
 * This function allocate data in flash, which can be read later indicating the dataTag.
 *
 * @param * data: pointer to data buffer.
 * @param dataLen: size of data buffer.
 * @param * dataTag: string that identifies the data sector programmed.
 * @param dataTagLen: number of tag's characters (maximum MAX_TAG_LEN words). if It's higher than maximum,
 * 					dataTagLen It's replaced for the maximum value.
 * @retval #-1 if error. != -1 if success.
 */
int flashAlloc(int * data, int dataLen, char * dataTag, int dataTagLen);



/*@brief Read data from flash sector indicated by dataTag
 *
 *
 * This function only read data from flash memory according to the dataTag passed by parameter.
 * To read successfully, previously is needed a call to flashAlloc with the same "dataTag" (may be
 * by a program which had been run before).
 *
 * @param * data: pointer to data buffer in which is written the flash data.
 * @param dataLen: size of data words desired to be written in data buffer.
 * @param * dataTag: string that identifies the data sector which is desired to be read.
 * @param dataTagLen: number of tag's characters (maximum MAX_TAG_LEN words). if It's higher than maximum,
 * 					dataTagLen It's replaced for the maximum value.
 * @retval #-1 if error. != -1 if success.
 */
int readFlash(int * data, int dataLen, char * dataTag, int dataTagLen);



#endif /* FLASHHAL_H_ */
