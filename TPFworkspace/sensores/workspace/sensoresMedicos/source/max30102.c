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
#include "i2s.h"
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define TIMER_TIME 160 //Tiempo en milisegundos para llamar al callback

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
#define LED2_PA2	0X0D
#define MULTI_LED1	0X11
#define MULTI_LED2	0X12

	//Tipos de datos
typedef enum{FIFTEEN_BITS=0, SIXTEEN_BITS, SEVENTEEN_BITS, EIGHTEEN_BITS}adc_res_t;
typedef enum{
	i0=0,	i0_2,	i0_4,	i0_6,	i0_8,	i1,	i1_2,	i1_4,	i1_6, i1_8,
	i2,		i2_2,	i2_4,	i2_6,	i2_8,	i3,	i3_2,	i3_4,	i3_6, i3_8,
	i4,		i4_2,	i4_4,	i4_6,	i4_8,	i5, i5_2,	i5_4,	i5_6, i5_8,
	i6,		i6_2,	i6_4,	i6_6,	i6_8,	i7, i7_2,	i7_4,	i7_6, i7_8,
	i8,		i8_2,	i8_4,	i8_6,	i8_8,	i9, i9_2,	i9_4,	i9_6, i9_8,
	i10,	i10_2,	i10_4,	i10_6,	i10_8,	i11, i11_2,	i11_4,	i11_6, i11_8,
	i12,	i12_2,	i12_4,	i12_6,	i12_8,	i13, i13_2,	i13_4,	i13_6, i13_8,
	i14,
}led_current_t; //Valores de las corrientes en mA (El _ indica comienzo de decimales)

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/
void LedInit(led_init_t data){
	WriteByte();

	return regmap_write(data->regmap, MAX30102_REG_IR_LED_CONFIG, reg);
}
void ConfigureMax30102(void){
	/* setup LED current settings */
	LedInit(data);
	if (ret)
		return ret;

	/* configure 18-bit HR + SpO2 readings at 400Hz */
	ret = regmap_write(data->regmap, MAX30102_REG_SPO2_CONFIG,
				(MAX30102_REG_SPO2_CONFIG_ADC_4096_STEPS
				 << MAX30102_REG_SPO2_CONFIG_ADC_MASK_SHIFT) |
				(MAX30102_REG_SPO2_CONFIG_SR_400HZ
				 << MAX30102_REG_SPO2_CONFIG_SR_MASK_SHIFT) |
				 MAX30102_REG_SPO2_CONFIG_PULSE_411_US);
	if (ret)
		return ret;

	/* average 4 samples + generate FIFO interrupt */
	ret = regmap_write(data->regmap, MAX30102_REG_FIFO_CONFIG,
				(MAX30102_REG_FIFO_CONFIG_AVG_4SAMPLES
				 << MAX30102_REG_FIFO_CONFIG_AVG_SHIFT) |
				 MAX30102_REG_FIFO_CONFIG_AFULL);
	if (ret)
		return ret;

	/* enable FIFO interrupt */
	return regmap_update_bits(data->regmap, MAX30102_REG_INT_ENABLE,
				 MAX30102_REG_INT_ENABLE_MASK,
				 MAX30102_REG_INT_ENABLE_FIFO_EN);

}

/********************************************************
 * 					FUNCIONES DEL HEADER
 ********************************************************/
void InitializeHardware(max_init_t* init_data){
	ConfigureMax30102();
	SetTimer(OXIMETER, init_data->timeout, init_data->callback);
}


