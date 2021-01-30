/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include "queue.h"
#include "audioPlayer.h"

//IO
#include "timer.h"
#include "timer_queue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_HB 120
#define MIN_HB 60
#define MAX_SPO2 ??
#define MIN_SPO2 ??
#define INIT_ECG_COUNTER ??
#define INIT_OX_COUNTER ??

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// events
static UserData_t userData;
static ev_name nameEvent;
ECG_init_t ECG_init;
oxi_init_t oxi_init;

// counters
uint8_t ecgCounter = 0;
uint8_t oxCounter = 0;
uint8_t tempCounter = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init(void){
	// IO Initialization
	InitializeECG(&ECG_init);
	InitializeOximetry(&oxi_init);
}

dispatchOxEvent(userData.oxUd);
disptachEcgEvent(userData.ecgUd);
dispatchTempEvent(userData.tempUd);

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void){
	if(getEvent(&userData){
		switch(userData.name){
			case MEAS_OX_EV:
				dispatchOxEvent(userData.oxUd);
				break;
			case MEAS_ECG_EV:
				disptachEcgEvent(userData.ecgUd);
				break;
			case MEAS_TEMP_EV:
				dispatchTempEvent(userData.tempUd);
				break;
			default:
				break;
	}		
}

// typedef enum {ALERTA_0, ALERTA_1} audioTag_t;
// typedef enum {AUDIO_MP3, AUDIO_I2S_STEREO_DECODED} audioFormat_t;

dispatchOxEvent(oxUd_t oxUd){
	AddInputSamples(oxUd.sample_red, oxUd.sample_ir);
	if(oxCounter == 0){
		oxCounter = INIT_OX_COUNTER;
		CalculateSpO2();
		int32_t spo2 = GetSpO2();
		if((spo2 > MAX_SPO2) || (spo2 < MIN_SPO2)){
			start_playing(audioTag_t tag, audioFormat_t audioInputFormat, audioFormat_t audioOutputFormat); // PONER TAG Y FORMAT!!!
		}
	}
	else{
		ecgCounter = ecgCounter-1;
	}
}

disptachEcgEvent(ecgUd_t ecgUd){
	AddEcgSample(ecgUd.sample); 
	if(ecgCounter == 0){
		ecgCounter = INIT_ECG_COUNTER;
		CalculateHeartBeat();
		uint16_t hb = GetHeartBeat();
		if((hb > MAX_HB) || (hb < MIN_HB)){
			start_playing(audioTag_t tag, audioFormat_t audioInputFormat, audioFormat_t audioOutputFormat); // PONER TAG Y FORMAT!!!
		}
	}
	else{
		ecgCounter = ecgCounter-1;
	}
}

dispatchTempEvent(tempUd_t tempUd){
	//llamar a add ecg sample
	//reducir counter
	//si counter es 0 calculo params
	//si params son alaramantes (con getter) --> entonces llamo a funcion de mati
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
