/*
 * SysTick.h
 *
 *  Created on: 28 ene. 2021
 *      Author: Matias
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SYSTICK_ISR_FREQ_HZ 100000UL //Clk de periodo 10us



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize SysTic driver
 * @param funcallback Function to be call every SysTick
 * @return Initialization and registration succeed
 */
_Bool SysTick_Init (void (*funcallback)(void));


/*******************************************************************************
 ******************************************************************************/

#endif // _SYSTICK_H_
#endif /* SYSTICK_H_ */
