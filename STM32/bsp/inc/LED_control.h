/*
 * LED_control.h
 *
 *  Created on: 17 mai 2023
 *      Author: rolla
 */

#ifndef BSP_INC_LED_CONTROL_H_
#define BSP_INC_LED_CONTROL_H_

#include "stm32f0xx.h"

/*
 * LED driver functions
 */

void LedInit(void);
void LedOn(uint8_t led);
void LedOff(uint8_t led);
void LedToggle(uint8_t led);

/*
 * LED defines
 */

#define LED_CH0		0x00	// Red LED for Channel 0
#define LED_CH1		0x01	// Green LED for Channel 1
#define LED_CH2		0x02	// Blue LED for Channel 2
#define LED_RX		0x03	// Green LED for RX
#define LED_TX		0x04	// Blue LED for TX
#define LED_ALL		0x05	// All LEDs control

#endif /* BSP_INC_LED_CONTROL_H_ */
