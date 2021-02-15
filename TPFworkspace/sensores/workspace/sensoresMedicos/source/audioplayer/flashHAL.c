/*
 * flashHAL.c
 *
 *  Created on: 30 mar. 2020
 *      Author: G5
 */

#include "flashHAL.h"
#include "fsl_ftfx_flash.h"
#include <stdbool.h>

uint32_t pflashBlockBase  = 0;
uint32_t pflashTotalSize  = 0;
uint32_t pflashSectorSize = 0;

typedef struct
{
	int indexTag;
	int dataLen;   //length in bytes
	volatile uint8_t * startAdress;
	_Bool isDataEmpty;
}metaData_t;

metaData_t dataStreams[MAX_STREAMS_ALLOWED];


static flash_config_t flashConfig;

status_t  flashStatus;

bool isFlashINIT = false;

void resetMetaData(uint32_t startAdress);

_Bool isMetaDataValid(void);

void refreshIndex(void);

void prepareStream(char * p2data, int dataLen);

int flashINIT(void)
{

	int ret = 0;
	int i = 0;

	if(!isFlashINIT)
	{

		/* Clean up Flash structure*/
		memset(&flashConfig, 0, sizeof(flash_config_t));

		flashStatus = FLASH_Init(&flashConfig);

		/* Get flash properties*/
		FLASH_GetProperty(&flashConfig, kFLASH_PropertyPflash0BlockBaseAddr, &pflashBlockBase);
		FLASH_GetProperty(&flashConfig, kFLASH_PropertyPflash0TotalSize, &pflashTotalSize);
		FLASH_GetProperty(&flashConfig, kFLASH_PropertyPflash0SectorSize, &pflashSectorSize);

		/*allow to alloc flash with config*/

		flashConfig.ftfxConfig[0].ifrDesc.resRange.dflashIfrStart = pflashTotalSize - (MAX_STREAMS_ALLOWED* STREAM_LEN);
		flashConfig.ftfxConfig[0].ifrDesc.resRange.pflashSwapIfrStart = pflashTotalSize - (MAX_STREAMS_ALLOWED* STREAM_LEN);
		flashConfig.ftfxConfig[0].ifrDesc.resRange.ifrMemSize = MAX_STREAMS_ALLOWED*STREAM_LEN;
	}
	if(flashStatus != kStatus_FTFx_Success)
	{
		ret = -1;   //error detected
	}
	else   //if all ok:1) Fill the metadata streams start address. 2) Turn on flag isFlashInit.
	{
		dataStreams[0].startAdress= (volatile uint8_t *)(pflashBlockBase + (pflashTotalSize - (MAX_STREAMS_ALLOWED * STREAM_LEN) ));
		for (i = 1; i < MAX_STREAMS_ALLOWED; i++){
			dataStreams[1].startAdress = (volatile uint8_t *)(dataStreams[i - 1].startAdress + STREAM_LEN);
		}

		isFlashINIT = true;
	}
	return ret;
}


int flashAlloc(int * data, int dataLen, int tagIndex)
{
	int ret = 0;

	if(isFlashINIT && (tagIndex < MAX_STREAMS_ALLOWED) && (dataLen > 0))
	{
		prepareStream((char *)data, dataLen);
		flashStatus = FLASH_Erase(&flashConfig, (uint32_t) dataStreams[tagIndex].startAdress, STREAM_LEN, kFTFx_ApiEraseKey);
//		flashStatus = FLASH_Program( &flashConfig, (uint32_t) dataStreams[tagIndex].startAdress, (uint8_t *) stream, STREAM_LEN);
		flashStatus = FLASH_Program( &flashConfig, (uint32_t) dataStreams[tagIndex].startAdress, (uint8_t *) data, STREAM_LEN);
		//refresh metaData
		dataStreams[tagIndex].isDataEmpty = false;
		dataStreams[tagIndex].dataLen = dataLen;
		dataStreams[tagIndex].indexTag = tagIndex;


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


char *  readFlash(int * dataLen, int dataTag)
{
	if(dataTag < MAX_STREAMS_ALLOWED)
	{
		dataLen[0] = STREAM_LEN;
		//memcpy(stream, dataStreams[dataTag].startAdress, STREAM_LEN);
		return (char *)dataStreams[dataTag].startAdress;
		//return (volatile char *)(dataStreams[indexFound].startAdress);

		/*flashStatus = FLASH_ReadResource(&flashConfig, (uint32_t) (dataStreams[indexFound].startAdress), (uint8_t *) stream, (uint32_t) STREAM_LEN, kFTFx_ResourceOptionFlashIfr);
		if(flashStatus != kStatus_FTFx_Success )
		{
			ret = -1;   //error detected
		}*/
		//memcpy((void *)stream, (void *)dataStreams[indexFound].startAdress, STREAM_LEN);
	}
	else
	{
		return 0; //error
	}


}




/*void resetMetaData(uint32_t startAdress)
{
	int i = 0;
	for(i = 0; i < MAX_STREAMS_ALLOWED; i++)
	{
		dataStreams[i].isDataEmpty = false;
		dataStreams[i].startAdress = startAdress;
		dataStreams[i].dataLen = 0;
	}
	indexFree = 0;
}*/

/*_Bool isMetaDataValid(void)
{
	return (dataStreams[0].startAdress == flashConfig.flashDesc.blockBase) &&
			((dataStreams[MAX_STREAMS_ALLOWED - 1].startAdress  + dataStreams[MAX_STREAMS_ALLOWED - 1].dataLen)
					== (flashConfig.flashDesc.blockBase + flashConfig.flashDesc.totalSize));
}*/

void prepareStream(char * p2data, int dataLen)
{
	//memset(stream, 0, STREAM_LEN);
	int i = 0;
	if (dataLen > STREAM_LEN){
		dataLen = STREAM_LEN;
	}
	for (i= 0; i < dataLen; i++){
		//stream[i] = p2data[i];
	}
}

void refreshIndex(void)
{/*
	streamIndex2program++;
	if(indexFree >= MAX_STREAMS_ALLOWED)
	{
		indexFree = 0;
	}*/
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


