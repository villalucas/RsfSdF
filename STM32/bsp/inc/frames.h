/*
 * frames.h
 *
 *  Created on: 15 mai 2023
 *      Author: Nicolas
 */

#ifndef APP_INC_FRAMES_H_
#define APP_INC_FRAMES_H_

#define SIZE_MSG_MAX 15

#define SOF_MSG_SYMBOL 'S'
#define SOF_ACK_SYMBOL 'A'
#define EOF_SYMBOL	   'E'

#define CHANNEL_SHIFT 6
#define CHANNEL_MASK 0x03
#define ADDRESS_MASK 0x3F

// Structure ID:
typedef struct
{
	uint8_t channel;
	uint8_t address;

}ID_frame;

// Structure message msg_frame_t :
typedef struct
{
	uint8_t SOF = SOF_MSG_SYMBOL ;
	ID_frame src;
	ID_frame dest;
	uint8_t size;
	uint8_t msg[SIZE_MSG_MAX];
	uint16_t CRC;
	uint8_t EOF = EOF_SYMBOL;
}msg_frame_t;

// Structure acquittement ack_frame_t :
typedef struct
{
	uint8_t SOF = SOF_ACK_SYMBOL;
	ID_frame src;
	ID_frame dest;
	uint16_t CRC;
	uint8_t EOF = EOF_SYMBOL;
}ack_frame_t;


#endif /* APP_INC_FRAMES_H_ */
