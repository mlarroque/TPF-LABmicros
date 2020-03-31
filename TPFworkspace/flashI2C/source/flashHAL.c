/*
 * flashHAL.c
 *
 *  Created on: 30 mar. 2020
 *      Author: G5
 */

#include "fsl_ftfx_controller.h"
#include <stdbool.h>

#define MAX_STREAMS_ALLOWED 5

typedef struct
{
	char * tag;
	int tagLen;
	int dataLen;   //length in bytes
	int startAdress;
	_Bool isDataEmpty;
}metaData_t;

metaData_t dataStreams[MAX_STREAMS_ALLOWED];

int indexFree = 0;


ftfx_config_t flashConfig;

status_t  flashStatus;

bool isFlashINIT = false;

void resetMetaData(uint32_t startAdress);

_Bool isMetaDataValid(void);

_Bool dataTagExists(char * dataTag, int dataTagLen, int * indexFound);

int flashINIT(int previousInfoFlag)
{

	int ret = 0;
	int isMetaDataCorrupted = false;

	if(!isFlashINIT)
	{
		config.flashDesc.type = kFTFx_MemTypePflash;
		config.flashDesc.index = 0;
		flashStatus = FTFx_API_Init(&flashConfig);   //initialize flash controller and
													//fill flashConfig according to hardware features
		//fill metaData
		if(previousInfoFlag != -1) //there is previous info in flash to take into account
		{
			flashStatus = FTFx_CMD_ReadResource(flashConfig, flashConfig.flashDesc.blockBase, (uint8_t *) dataStreams, (uint32_t) (sizeof(dataStreams[0]) * MAX_STREAMS_ALLOWED), kFTFx_ResourceOptionFlashIfr);//readPreviousMetaData (fill isMetaData corrupted = true if there is some reading error)
		}
		else  //reset metaData
		{
			resetMetaData(flashConfig.flashDesc.blockBase + (sizeof(dataStreams[0]) * MAX_STREAMS_ALLOWED)); //the sector allowed to be programmed is after the sector of metaData.
			//may be here It could be programmed metaData, but for now It's not necessary.
		}
	}
	if(flashStatus != kStatus_FTFx_Success || (isMetaDataCorrupted) || (!isMetaDataValid()))
	{
		ret = -1;   //error detected
	}
	else
	{
		isFlashINIT = true;
	}
	return ret;
}


int flashAlloc(int * data, int dataLen, char * dataTag, int dataTagLen)
{
	int ret = 0;
	int auxIndexFound;  //just for call dataTagExists, but the content of auxIndexFound is not used.
	uint32_t sizeMetaDataBuffer = sizeof(dataStreams[0]) * MAX_STREAMS_ALLOWED;
	if(isFlashINIT && (!dataTagExists(dataTag, dataTagLen, &auxIndexFound)) && (dataStreams[indexFree].isDataEmpty) && (dataLen > 0))
	{
		flashStatus = FTFx_CMD_Erase(&flashConfig, (uint32_t) dataStreams[indexFree].startAdress, (uint32_t) dataLen,  kFTFx_ApiEraseKey);
		flashStatus = FTFx_CMD_Program( &flashConfig, (uint32_t) dataStreams[indexFree].startAdress, (uint8_t *) data, (uint32_t) dataLen);

		//refresh metaData
		dataStreams[indexFree].isDataEmpty = false;
		dataStreams[indexFree].dataLen = dataLen;
		dataStreams[indexFree].tagLen = (dataTagLen <= MAX_TAG_LEN)? dataTagLen : MAX_TAG_LEN;
		dataStreams[indexFree].tag = dataTag;
		if(indexFree < MAX_STREAMS_ALLOWED - 1)
		{
			dataStreams[indexFree + 1].startAdress = dataStreams[indexFree].startAdress + dataLen;
		}
		refreshIndex();    //refresh indexFree   (++ or set in 0 if It´s equal to MAX_STREAMS_ALLOWED

		//program metaData in flash for future usage.
		flashStatus = FTFx_CMD_Erase(&flashConfig, (uint32_t) flashConfig.flashDesc.blockBase, (uint32_t) sizeMetaDataBuffer,  kFTFx_ApiEraseKey);
		flashStatus = FTFx_CMD_Program( &flashConfig, (uint32_t) flashConfig.flashDesc.blockBase, (uint8_t *) dataStreams, (uint32_t) sizeMetaDataBuffer);

		if(flashStatus != kStatus_FTFx_Success )
		{
			ret = -1;   //error detected
		}
	}
	else
	{
		ret = -1;
	}

	return ret;
}


int readFlash(int * data, int dataLen, char * dataTag, int dataTagLen)
{
	int indexFound, ret = 0;
	if(dataTagExists(dataTag, dataTagLen, &indexFound) && (dataLen > 0) && (dataLen <= dataStreams[indexFound].dataLen))
	{
		flashStatus = FTFx_CMD_ReadResource(flashConfig, (uint32_t) dataStreams[indexFound].startAdress, (uint8_t *) data, (uint32_t) dataLen, kFTFx_ResourceOptionFlashIfr);

		if(flashStatus != kStatus_FTFx_Success )
		{
			ret = -1;   //error detected
		}
	}
	else
	{
		ret = -1; //error
	}
	return ret;
}




void resetMetaData(uint32_t startAdress)
{
	int i = 0;
	for(i = 0; i < MAX_STREAMS_ALLOWED; i++)
	{
		dataStreams[i].isDataEmpty = false;
		dataStreams[i].startAdress = startAdress;
		dataStreams[i].dataLen = 0;
	}
	indexFree = 0;
}

_Bool isMetaDataValid(void)
{
	return (dataStreams[0].startAdress == flashConfig.flashDesc.blockBase) &&
			((dataStreams[MAX_STREAMS_ALLOWED - 1].startAdress  + dataStreams[MAX_STREAMS_ALLOWED - 1].dataLen)
					== (flashConfig.flashDesc.blockBase + flashConfig.flashDesc.totalSize));
}

_Bool dataTagExists(char * dataTag, int dataTagLen, int * indexFounded)
{
	int i = 0, j = 0;
	bool wordMissmatched = false, tagFound = false;
	if((dataTagLen > 0) && (dataTag != 0))
	{
		for(i = 0; (i < MAX_STREAMS_ALLOWED) && (!tagFound); i++)
		{
			if(!dataStreams[i].isDataEmpty && (dataStreams[i].tagLen == dataTagLen))
			{
				for(j = 0; (j < dataTagLen) && (j < MAX_TAG_LEN) && (!wordMissmatched); j++)
				{
					if ((dataStreams[i].tag)[j] != dataTag[j])
					{
						wordMissmatched = true; //wordMissmatched because of the difference between chars in dataTag
					}
				}
			}
			else
			{
				wordMissmatched = true; //wordMissmatched because of dataEmpy or 'len missmatched'
			}
			if(!wordMissmatched)
			{
				tagFound = true;
				*indexFound = i;
			}
		}
	}

	return tagFound;
}

void refreshIndex(void)
{
	indexFree++;
	if(indexFree >= MAX_STREAMS_ALLOWED)
	{
		indexFree = 0;
	}
}
/*@brief Erases the flash sectors encompassed by parameters passed into function.
 *
 * This function erases the appropriate number of flash sectors based on the
 * desired start address and length.
 *
 * @param config The pointer to the storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be erased.
 *              The start address does not need to be sector-aligned but must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *                      to be erased. Must be word-aligned.
 * @param key The value used to validate all flash erase APIs.
 *
 * @retval #kStatus_FTFx_Success API was executed successfully.
 * @retval #kStatus_FTFx_InvalidArgument An invalid argument is provided.
 * @retval #kStatus_FTFx_AlignmentError The parameter is not aligned with the specified baseline.
 * @retval #kStatus_FTFx_AddressError The address is out of range.
 * @retval #kStatus_FTFx_EraseKeyError The API erase key is invalid.
 * @retval #kStatus_FTFx_ExecuteInRamFunctionNotReady Execute-in-RAM function is not available.
 * @retval #kStatus_FTFx_AccessError Invalid instruction codes and out-of bounds addresses.
 * @retval #kStatus_FTFx_ProtectionViolation The program/erase operation is requested to execute on protected areas.
 * @retval #kStatus_FTFx_CommandFailure Run-time error during the command execution.
 */
//status_t FTFx_CMD_Erase(ftfx_config_t *config,
 //                       uint32_t start,
 //                       uint32_t lengthInBytes,
 //                       uint32_t key);




/*@brief Programs flash with data at locations passed in through parameters.
*
* This function programs the flash memory with the desired data for a given
* flash area as determined by the start address and the length.
*
* @param config A pointer to the storage for the driver runtime state.
* @param start The start address of the desired flash memory to be programmed. Must be
*              word-aligned.
* @param src A pointer to the source buffer of data that is to be programmed
*            into the flash.
* @param lengthInBytes The length, given in bytes (not words or long-words),
*                      to be programmed. Must be word-aligned.
*
* @retval #kStatus_FTFx_Success API was executed successfully.
* @retval #kStatus_FTFx_InvalidArgument An invalid argument is provided.
* @retval #kStatus_FTFx_AlignmentError Parameter is not aligned with the specified baseline.
* @retval #kStatus_FTFx_AddressError Address is out of range.
* @retval #kStatus_FTFx_ExecuteInRamFunctionNotReady Execute-in-RAM function is not available.
* @retval #kStatus_FTFx_AccessError Invalid instruction codes and out-of bounds addresses.
* @retval #kStatus_FTFx_ProtectionViolation The program/erase operation is requested to execute on protected areas.
* @retval #kStatus_FTFx_CommandFailure Run-time error during the command execution.
*/
//status_t FTFx_CMD_Program(ftfx_config_t *config,
 //                        uint32_t start,
  //                       uint8_t *src,
   //                      uint32_t lengthInBytes);


/*!
 * @brief Reads the resource with data at locations passed in through parameters.
 *
 * This function reads the flash memory with the desired location for a given
 * flash area as determined by the start address and length.
 *
 * @param config A pointer to the storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be programmed. Must be
 *              word-aligned.
 * @param dst A pointer to the destination buffer of data that is used to store
 *        data to be read.
 * @param lengthInBytes The length, given in bytes (not words or long-words),
 *        to be read. Must be word-aligned.
 * @param option The resource option which indicates which area should be read back.
 *
 * @retval #kStatus_FTFx_Success API was executed successfully.
 * @retval #kStatus_FTFx_InvalidArgument An invalid argument is provided.
 * @retval #kStatus_FTFx_AlignmentError Parameter is not aligned with the specified baseline.
 * @retval #kStatus_FTFx_ExecuteInRamFunctionNotReady Execute-in-RAM function is not available.
 * @retval #kStatus_FTFx_AccessError Invalid instruction codes and out-of bounds addresses.
 * @retval #kStatus_FTFx_ProtectionViolation The program/erase operation is requested to execute on protected areas.
 * @retval #kStatus_FTFx_CommandFailure Run-time error during the command execution.
 */
//#if defined(FSL_FEATURE_FLASH_HAS_READ_RESOURCE_CMD) && FSL_FEATURE_FLASH_HAS_READ_RESOURCE_CMD
//status_t FTFx_CMD_ReadResource(ftfx_config_t *config,
 //                              uint32_t start,
  //                             uint8_t *dst,
   //                            uint32_t lengthInBytes,
    //                           ftfx_read_resource_opt_t option);
//#endif


