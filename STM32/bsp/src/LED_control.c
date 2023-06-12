/*
 * LED_control.c
 *
 *  Created on: 17 mai 2023
 *      Author: Loan
 */

#include "LED_control.h"

/**
 * @brief Fonction d inititialisation des leds externes commandees par gpio.
 *
 * Initialize LED pin (PB0, PB1, PB2, PC6, PC8) as a High-Speed Push-Pull output.
 * Set LED initial state to OFF.
 * Blue LED PB0 	->  	Channel 0
 * Green LED PB1 	->		Channel 1
 * Red LED PB2		->		Channel 2
 * Yellow LED PC6 	->		RX
 * Blue LED PC8 	->		TX
 */

void BSP_LED_CONTROL_ledInit() {
	// Enable GPIOB clock
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Enable GPIOC clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// Configure PB0 as output
	GPIOB->MODER &= ~GPIO_MODER_MODER0_Msk;
	GPIOB->MODER |= (0x01 << GPIO_MODER_MODER0_Pos);

	// Configure PB1 as output
	GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOB->MODER |= (0x01 << GPIO_MODER_MODER1_Pos);

	// Configure PB2 as output
	GPIOB->MODER &= ~GPIO_MODER_MODER2_Msk;
	GPIOB->MODER |= (0x01 << GPIO_MODER_MODER2_Pos);

	// Configure PC6 as output
	GPIOC->MODER &= ~GPIO_MODER_MODER6_Msk;
	GPIOC->MODER |= (0x01 << GPIO_MODER_MODER6_Pos);

	// Configure PC8 as output
	GPIOC->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOC->MODER |= (0x01 << GPIO_MODER_MODER8_Pos);

	// Configure PB0 as Push-Pull output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_0;

	// Configure PB1 as Push-Pull output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_1;

	// Configure PB2 as Push-Pull output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_2;

	// Configure PC6 as Push-Pull output
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_6;

	// Configure PC8 as Push-Pull output
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_8;

	// Configure PB0 as High-Speed Output
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR0_Msk;
	GPIOB->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR0_Pos);

	// Configure PB1 as High-Speed Output
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR1_Msk;
	GPIOB->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR1_Pos);

	// Configure PB2 as High-Speed Output
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR2_Msk;
	GPIOB->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR2_Pos);

	// Configure PC6 as High-Speed Output
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR6_Msk;
	GPIOC->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR6_Pos);

	// Configure PC8 as High-Speed Output
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR8_Msk;
	GPIOC->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR8_Pos);

	// Disable PB0 Pull-up/Pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0_Msk;

	// Disable PB1 Pull-up/Pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR1_Msk;

	// Disable PB2 Pull-up/Pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR2_Msk;

	// Disable PC6 Pull-up/Pull-down
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR6_Msk;

	// Disable PC8 Pull-up/Pull-down
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR8_Msk;

	// Set Initial State OFF
	GPIOB->BSRR |= GPIO_BSRR_BR_0;
	GPIOB->BSRR |= GPIO_BSRR_BR_1;
	GPIOB->BSRR |= GPIO_BSRR_BR_2;
	GPIOC->BSRR |= GPIO_BSRR_BR_6;
	GPIOC->BSRR |= GPIO_BSRR_BR_8;

	/*
	 * 	LED Test Routine
	 * 	All LEDs On for 500ms
	 * 	All LEDs Off
	 */

	GPIOB->BSRR = GPIO_BSRR_BS_0;	// Channel 0 LED On
	GPIOB->BSRR = GPIO_BSRR_BS_1;	// Channel 1 LED On
	GPIOB->BSRR = GPIO_BSRR_BS_2;	// Channel 2 LED On
	GPIOC->BSRR = GPIO_BSRR_BS_6;	// RX LED On
	GPIOC->BSRR = GPIO_BSRR_BS_8;	// TX LED On

	BSP_DELAY_ms(500);

	GPIOB->BSRR = GPIO_BSRR_BR_0;	// Channel 0 LED Off
	GPIOB->BSRR = GPIO_BSRR_BR_1;	// Channel 1 LED Off
	GPIOB->BSRR = GPIO_BSRR_BR_2;	// Channel 2 LED Off
	GPIOC->BSRR = GPIO_BSRR_BR_6;	// RX LED Off
	GPIOC->BSRR = GPIO_BSRR_BR_8;	// TX LED Off

	BSP_DELAY_ms(500);
}

/**
 * @brief Fonction pour allumer une led externe specifiquement.
 *
 * @param led Octet non signe contenant les informations de la led a allumer.
 */

void BSP_LED_CONTROL_ledOn(uint8_t led) {
	switch (led) {
	case 0:
		GPIOB->BSRR = GPIO_BSRR_BS_0;	// Channel 0 LED On
		break;
	case 1:
		GPIOB->BSRR = GPIO_BSRR_BS_1;	// Channel 1 LED On
		break;
	case 2:
		GPIOB->BSRR = GPIO_BSRR_BS_2;	// Channel 2 LED On
		break;
	case 3:
		GPIOC->BSRR = GPIO_BSRR_BS_6;	// RX LED On
		break;
	case 4:
		GPIOC->BSRR = GPIO_BSRR_BS_8;	// TX LED On
		break;
	case 5:
		GPIOB->BSRR = GPIO_BSRR_BS_0;	// Channel 0 LED On
		GPIOB->BSRR = GPIO_BSRR_BS_1;	// Channel 1 LED On
		GPIOB->BSRR = GPIO_BSRR_BS_2;	// Channel 2 LED On
		GPIOC->BSRR = GPIO_BSRR_BS_6;	// RX LED On
		GPIOC->BSRR = GPIO_BSRR_BS_8;	// TX LED On
		break;
	}
}

/**
 * @brief Fonction pour eteindre une led externe specifiquement.
 *
 * @param led Octet non signe contenant les informations de la led a eteindre.
 */
void BSP_LED_CONTROL_ledOff(uint8_t led) {
	switch (led) {
	case 0:
		GPIOB->BSRR = GPIO_BSRR_BR_0;	// Channel 0 LED Off
		break;
	case 1:
		GPIOB->BSRR = GPIO_BSRR_BR_1;	// Channel 1 LED Off
		break;
	case 2:
		GPIOB->BSRR = GPIO_BSRR_BR_2;	// Channel 2 LED Off
		break;
	case 3:
		GPIOC->BSRR = GPIO_BSRR_BR_6;	// RX LED Off
		break;
	case 4:
		GPIOC->BSRR = GPIO_BSRR_BR_8;	// TX LED Off
		break;
	case 5:
		GPIOB->BSRR = GPIO_BSRR_BR_0;	// Channel 0 LED Off
		GPIOB->BSRR = GPIO_BSRR_BR_1;	// Channel 1 LED Off
		GPIOB->BSRR = GPIO_BSRR_BR_2;	// Channel 2 LED Off
		GPIOC->BSRR = GPIO_BSRR_BR_6;	// RX LED Off
		GPIOC->BSRR = GPIO_BSRR_BR_8;	// TX LED Off
		break;
	}
}

/**
 * @brief Fonction pour inverser l etat d une led externe specifiquement.
 *
 * @param led Octet non signe contenant les informations de la led a inverser.
 */
void BSP_LED_CONTROL_ledToggle(uint8_t led) {
	switch (led) {
	case 0:
		GPIOB->ODR ^= GPIO_ODR_0;		// Channel 0 LED Toggle
		break;
	case 1:
		GPIOB->ODR ^= GPIO_ODR_1;		// Channel 1 LED Toggle
		break;
	case 2:
		GPIOB->ODR ^= GPIO_ODR_2;		// Channel 2 LED Toggle
		break;
	case 3:
		GPIOC->ODR ^= GPIO_ODR_6;		// RX LED Toggle
		break;
	case 4:
		GPIOC->ODR ^= GPIO_ODR_8;		// TX LED Toggle
		break;
	case 5:
		GPIOB->ODR ^= GPIO_ODR_0;		// Channel 0 LED Toggle
		GPIOB->ODR ^= GPIO_ODR_1;		// Channel 1 LED Toggle
		GPIOB->ODR ^= GPIO_ODR_2;		// Channel 2 LED Toggle
		GPIOC->ODR ^= GPIO_ODR_6;		// RX LED Toggle
		GPIOC->ODR ^= GPIO_ODR_8;		// TX LED Toggle
		break;
	}
}

/**
 * @brief Fonction pour tester toute les leds externe liees au status des cannaux.
 */
void BSP_LED_CONTROL_ledChannelTest()
{
	// setup SX1272 on channel 0
	id_frame_t device;
	device.address = 0;
	device.channel = 0; // channel 0, 1 or 2
	APP_SX1272_setup(device);

	BSP_DELAY_ms(500);

	// setup SX1272 on channel 1
	device.address = 1;
	device.channel = 1; // channel 0, 1 or 2
	APP_SX1272_setup(device);

	BSP_DELAY_ms(500);

	// setup SX1272 on channel 2
	device.address = 2;
	device.channel = 2; // channel 0, 1 or 2
	APP_SX1272_setup(device);

	BSP_DELAY_ms(500);

	BSP_LED_CONTROL_ledOff(LED_ALL);
}
