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


//Defines
#define MSG_RETRY_DELAY_MIN			50
#define MSG_RETRY_DELAY_MAX			500


// TARGET Transmitter mode
#ifdef TRANSMITTER
	#define DEFAULT_ADRESS			1
	#define DEFAULT_CHANNEL			1
	#endif

// TARGET Reveiver mode
#ifdef RECEIVER
	#define DEFAULT_ADRESS			2
	#define DEFAULT_CHANNEL			1
	#endif


#endif /* APP_INC_CONFIG_H_ */
