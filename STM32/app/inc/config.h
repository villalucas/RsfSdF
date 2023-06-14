/*
 * config.h
 *
 *  Created on: 9 juin 2023
 *      Author: Nicolas
 */

#ifndef APP_INC_CONFIG_H_
#define APP_INC_CONFIG_H_

//Includes
#include "frames.h"
#include "SX1272.h"


//Defines
#define MSG_RETRY_DELAY_MIN			200
#define MSG_RETRY_DELAY_MAX			1500

#define	FORCE_CRC_ERROR				0

#define CYCLE_CHANNELS				1

//Frequency defines
#define CHANNEL_0_FREQUENCY			CH_00_900
#define CHANNEL_1_FREQUENCY			CH_01_900
#define CHANNEL_2_FREQUENCY			CH_02_900

// TARGET Transmitter mode
#ifdef TRANSMITTER
	#define DEFAULT_ADRESS			1
	#define DEFAULT_CHANNEL			0
	#endif

// TARGET Reveiver mode
#ifdef RECEIVER
	#define DEFAULT_ADRESS			2
	#define DEFAULT_CHANNEL			1
	#endif


#endif /* APP_INC_CONFIG_H_ */
