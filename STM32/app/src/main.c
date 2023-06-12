/*
 * main.c
 *
 *  Created on: 23 ao�t 2020
 *      Author: Arnaud
 */

#include "main.h"
#include "config.h"
#include "stm32f0xx.h"
#include "bsp.h"
#include "comSX1272.h"
#include "SX1272.h"
#include "appSX1272.h"
#include "LED_control.h"
#include "delay.h"
#include "stdlib.h"
#include "device.h"
#include <string.h>

static void SystemClock_Config();

int main()
{
	uint32_t i = 0;
	id_frame_t device;

#ifdef TRANSMITTER
	char msg_to_send[] = "Evan";
	uint8_t		transmit_status;
	msg_frame_t transmit_msg;
	#endif

#ifdef RECEIVER
	uint8_t		receive_status;
	msg_frame_t received_msg;
	ack_frame_t received_ack;
	#endif

	// Initialize System clock to 48MHz from external clock
	SystemClock_Config();
	// Initialize timebase
	BSP_TIMER_Timebase_Init();
	// Initialize NVIC
	BSP_NVIC_Init();
	// Initialize SPI interface
	BSP_SPI1_Init();
	// Initialize Debug Console
	BSP_Console_Init();
	my_printf("\nMain : Console ready!\r\n");

	// Initialize LEDs
	BSP_LED_CONTROL_ledInit();
	my_printf("Main : Leds ready!\r\n");

	APP_DEVICE_init(&device);
	my_printf("Main : Device init!\r\n");

	// Channels LED test
	BSP_LED_CONTROL_ledChannelTest();
	my_printf("Main: Leds test channel\r\n");

	APP_SX1272_setup(device);



	while(1)
	{
#ifdef TRANSMITTER
		transmit_msg.src.address = device.address;
		transmit_msg.src.channel = device.channel;
		transmit_msg.dest.address = 2;
		transmit_msg.dest.channel = device.channel;
		if(strlen(msg_to_send) <= SIZE_MSG_MAX) {
			strcpy((char*)transmit_msg.msg, msg_to_send);
			transmit_msg.size = strlen(msg_to_send);
		}
		else {
			strcpy((char*)transmit_msg.msg, "TEST");
			transmit_msg.size = strlen("TEST");
		}
		transmit_status = APP_SX1272_SendMsg(device, &transmit_msg, 5);
		my_printf("Transmit status : %d\r\n", transmit_status);
		#endif

#ifdef RECEIVER
		receive_status = APP_SX1272_runReceive(device, &received_msg, &received_ack);
		if(receive_status == RECEIVE_MSG_RECEIVED_ACK_TRANSMITTED){
			my_printf("Main_Receiver : MSG for me received and ACK transmitted\r\n");
		}
		else if(receive_status == RECEIVE_MSG_RECEIVED_ACK_NOT_TRANSMITTED){
			my_printf("Main_Receiver : MSG for me received and ACK not transmitted\r\n");
		}
		else
		{
			my_printf("Main_Receiver : ERROR\r\n");
		}
		#endif
		i++;
	}
}
/*
 * 	Clock configuration for the Nucleo STM32F072RB board
 * 	HSE input Bypass Mode 			-> 8MHz
 * 	SYSCLK, AHB, APB1 				-> 48MHz
 *  	PA8 as MCO with /16 prescaler 		-> 3MHz
 *
 *  Laurent Latorre - 05/08/2017
 */
static void SystemClock_Config()
{
	uint32_t	HSE_Status;
	uint32_t	PLL_Status;
	uint32_t	SW_Status;
	uint32_t	timeout = 0;

	timeout = 1000000;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	do
	{
		HSE_Status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((HSE_Status == 0) && (timeout > 0));

	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 <<RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 <<RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Wait until PLL is ready
	do
	{
		PLL_Status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((PLL_Status == 0) && (timeout > 0));

        // Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	/* --- Until this point, MCU was still clocked by HSI at 8MHz ---*/
	/* --- Switching to PLL at 48MHz Now!  Fasten your seat belt! ---*/

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	do
	{
		SW_Status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((SW_Status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	/* --- Here we go! ---*/

	/*--- Use PA8 as MCO output at 48/16 = 3MHz ---*/

	// Set MCO source as SYSCLK (48MHz)
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOSEL_SYSCLK;

	// Set MCO prescaler to /16 -> 3MHz
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOPRE_DIV16;

	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 as Alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER8_Pos);

	// Set to AF0 (MCO output)
	GPIOA->AFR[1] &= ~(0x0000000F);
	GPIOA->AFR[1] |=  (0x00000000);

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
}
