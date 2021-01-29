/*
 * max30102.c
 *
 *  Created on: 26 Jan 2021
 *      Author: user
 */

/*******************************************************
 * 					HEADERS UTILIZADOS
 *******************************************************/
#include "max30102.h"
#include "timer.h"
#include "i2c.h"
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define TIMER_TIME 160 //Tiempo en milisegundos para llamar al callback

#define MAX30102_ADDRESS 0X57
#define FIFO_DEPTH 32
#define BYTES_PER_SAMPLE 6
	//Direcciones de los registros
//Interrupt
#define INT_STATUS1	0X00
#define INT_STATUS2	0X01
#define INT_ENABLE1	0X02
#define INT_ENABLE2	0X03
//FIFO
#define FIFO_WRITE	0X04
#define OVF_COUNT	0X05
#define FIFO_READ	0X06
#define FIFO_DATA	0X07
#define FIFO_CONFIG	0X08
//Mode
#define MODE_CONFIG 0X09
//SP02
#define SP02_CONFIG	0X0A
//Leds
#define LED1_PA		0X0C
#define LED2_PA		0X0D
#define MULTI_LED1	0X11
#define MULTI_LED2	0X12

	//Mascaras y shifteos
#define SPO2_SR_SHIFT	2
#define SPO2_ADC_RGE	5

#define FIFO_AVG_SHIFT	5


	//Tipos de datos
//FIFO
typedef enum{NO_AVERAGE=0, AVG_2, AVG_4, AVG_8, AVG_16, AVG32}avg_samples_t;
typedef enum{EMPTY_0, EMPTY_1, EMPTY_2}fifo_a_full_t;

//Modo
typedef enum{HEART_RATE=2, SP02=3, MULTI_LED=7}mode_t;

//SP02 config
typedef enum{i2048=0, i4096, i8192, i16384}adc_range_t;

typedef enum{fs_50Hz=0, fs_100Hz,	 fs_200Hz, fs_400Hz, fs_800Hz,
			fs_1000Hz,	fs_1600Hz,	fs_3200Hz}sample_rate_t;

//Leds
typedef enum{FIFTEEN_BITS=0, SIXTEEN_BITS, SEVENTEEN_BITS, EIGHTEEN_BITS}adc_res_t;
typedef enum{
	i0=0,	i0_2,	i0_4,	i0_6,	i0_8,	i1,	i1_2,	i1_4,	i1_6, i1_8,
	i2,		i2_2,	i2_4,	i2_6,	i2_8,	i3,	i3_2,	i3_4,	i3_6, i3_8,
	i4,		i4_2,	i4_4,	i4_6,	i4_8,	i5, i5_2,	i5_4,	i5_6, i5_8,
	i6,		i6_2,	i6_4,	i6_6,	i6_8,	i7, i7_2,	i7_4,	i7_6, i7_8,
	i8,		i8_2,	i8_4,	i8_6,	i8_8,	i9, i9_2,	i9_4,	i9_6, i9_8,
	i10,	i10_2,	i10_4,	i10_6,	i10_8,	i11, i11_2,	i11_4,	i11_6, i11_8,
	i12,	i12_2,	i12_4,	i12_6,	i12_8,	i13, i13_2,	i13_4,	i13_6, i13_8,
	i14,	i14_2,	i14_4,	i14_6,	i14_8,	i15, i15_2,	i15_4,	i15_6, i15_8,
	i16,	i16_2,	i16_4,	i16_6,	i16_8,	i17, i17_2,	i17_4,	i17_6, i17_8,
	i18,	i18_2,	i18_4,	i18_6,	i18_8,	i19, i19_2, i19_4,	i19_6, i19_8,
	i20,	i20_2,	i20_4,	i20_6,	i20_8,	i21, i21_2,	i21_4,	i21_6, i21_8,
	i22,	i22_2,	i22_4,	i22_6,	i22_8,	i23, i23_2,	i23_4,	i23_6, i23_8,
	i24,	i24_2,	i24_4,	i24_6,	i24_8,	i25, i25_2,	i25_4,	i25_6, i25_8,
	i26,	i26_2,	i26_4,	i26_6,	i26_8,	i27, i27_2,	i27_4,	i27_6, i27_8,
	i28,	i28_2,	i28_4,	i28_6,	i28_8,	i29, i29_2,	i29_4,	i29_6, i29_8,
	i30,	i30_2,	i30_4,	i30_6,	i30_8,	i31, i31_2,	i31_4,	i31_6, i31_8,
	i32,	i32_2,	i32_4,	i32_6,	i32_8,	i33, i33_2,	i33_4,	i33_6, i33_8,
	i34,	i34_2,	i34_4,	i34_6,	i34_8,	i35, i35_2,	i35_4,	i35_6, i35_8,
	i36,	i36_2,	i36_4,	i36_6,	i36_8,	i37, i37_2,	i37_4,	i37_6, i37_8,
	i38,	i38_2,	i38_4,	i38_6,	i38_8,	i39, i39_2,	i39_4,	i39_6, i39_8,
	i40,	i40_2,	i40_4,	i40_6,	i40_8,	i41, i41_2,	i41_4,	i41_6, i41_8,
	i42,	i42_2,	i42_4,	i42_6,	i42_8,	i43, i43_2,	i43_4,	i43_6, i43_8,
	i44,	i44_2,	i44_4,	i44_6,	i44_8,	i45, i45_2,	i45_4,	i45_6, i45_8,
	i46,	i46_2,	i46_4,	i46_6,	i46_8,	i47, i47_2,	i47_4,	i47_6, i47_8,
	i48,	i48_2,	i48_4,	i48_6,	i48_8,	i49, i49_2,	i49_4,	i49_6, i49_8,
	i50,	i50_2,	i50_4,	i50_6,	i50_8,	i51
	}led_current_t; //Valores de las corrientes en mA (El _ indica comienzo de decimales)

	typedef enum{DISABLED=0, RED_LED, IR_LED}led_slot_t;

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/
void SetMode(mode_t mode){
	uint8_t reg = mode;
	WriteByte(MAX30102_ADDRESS, MODE_CONFIG, reg);

	reg = 0;
	WriteByte(MAX30102_ADDRESS, FIFO_READ, reg);//Clear FIFO pointers
	WriteByte(MAX30102_ADDRESS, FIFO_WRITE, reg);
}
void LedInit(led_current_t red_current, led_current_t ir_current){
	uint8_t reg = red_current;
	WriteByte(MAX30102_ADDRESS, LED1_PA, reg);
	reg = ir_current;
	WriteByte(MAX30102_ADDRESS, LED2_PA, reg);
}
void SetSp02(adc_range_t range, adc_res_t res, sample_rate_t sr){
	uint8_t reg = (res) | (sr << SPO2_SR_SHIFT) | (range << SPO2_ADC_RGE);
	WriteByte(MAX30102_ADDRESS, SP02_CONFIG, reg);

}
void SetFIFO(fifo_a_full_t n_max, avg_samples_t n_avg){
	uint8_t reg = n_max | (n_avg<<FIFO_AVG_SHIFT) ;
	WriteByte(MAX30102_ADDRESS, FIFO_CONFIG, reg);
}
void ConfigureMax30102(void){
	SetMode(SP02);	//Uso modo Sp02
	LedInit(i4, i4); //Setteo corriente de ambos leds
	SetSp02(i4096, SIXTEEN_BITS, fs_200Hz);
	SetFIFO(EMPTY_0, NO_AVERAGE);

}

/********************************************************
 * 					FUNCIONES DEL HEADER
 ********************************************************/
void InitializeOxHardware(max_init_t* init_data){
	ConfigureMax30102();
	SetTimer(OXIMETER, init_data->timeout, init_data->callback);
}

uint8_t GetNumOfSamples(void){
	uint8_t write_val;
	uint8_t read_val;
	ReadByte(MAX30102_ADDRESS, FIFO_WRITE, &write_val);
	ReadByte(MAX30102_ADDRESS, FIFO_READ, &read_val);
	return (uint8_t) ( (FIFO_DEPTH + write_val - read_val)%FIFO_DEPTH );
}

max_sample_t GetLedSamples(void){
	max_sample_t sample;
	uint32_t red_aux;
	uint32_t ir_aux;

}
