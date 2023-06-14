/*
 * appSX1272.h
 *
 *  Created on: 25 ao�t 2020
 *      Author: Arnaud
 */

#ifndef APP_INC_APPSX1272_H_
#define APP_INC_APPSX1272_H_

#include "SX1272.h"

///////////////////////////////////////////////////////////////
// Configuration du module en mode LoRA ou FSK
///////////////////////////////////////////////////////////////

//Main parameters (to be changed)
#define TX_Addr BROADCAST_ADDR
#define RX_Addr 8 //address of the node

#define OutPower POW_14 //min -1, max 14
#define TypeModulation 0 //0: LORA, 1:FSK
#define PeriodTransmission 5000 //en ms

//LORA configuration (to be changed)
#define paramBW BW_125 //BW_125, BW_250 or BW_500
#define paramCR CR_5 //CR_5
#define paramSF SF_12 //SF_7 to SF_12

//FSK configuration (to be changed)
#define BitRate BR1200bauds //BR1200bauds or BR4800bauds
#define Fdev FDA5kHz//FDA5kHz

//Other common parameters (do not change!!)
#define PreambLong 12//12

#define MaxNbRetries 3
#define WaitTxMax 3000 //en ms
#define WaitRxMax 3000 //en ms

//Error Defines
#define RECEIVE_NO_ERROR								0
#define RECEIVE_CRC_ERROR 								1
#define RECEIVE_SOF_ERROR 								2
#define RECEIVE_WRONG_DESTINARY 						3
#define RECEIVE_ACK_RECEIVED	 			 			4
#define RECEIVE_MSG_RECEIVED_ACK_TRANSMITTED	 		5
#define RECEIVE_MSG_RECEIVED_ACK_NOT_TRANSMITTED	 	6
#define RECEIVE_ERROR_UNKNOWN_CASE						7
#define RECEIVE_ERROR_ACK_TIMEOUT						8

#define TRANSMIT_NO_ERROR				0
#define	TRANSMIT_ERROR					1
#define	TRANSMIT_TIMEOUT_ERROR			2
#define	TRANSMIT_ERROR_CONFIG			3

#define SEND_MSG_ENCODED_FLAG 			1
#define SEND_ACK_ENCODED_FLAG 			0

#define CAD_MSG_DETECTED_CH0			0
#define CAD_MSG_DETECTED_CH1			1
#define CAD_MSG_DETECTED_CH2			2
#define CAD_MSG_NOT_DETECTED			3
#define CAD_ERROR_UNKNOWN_CASE			4

#define OFFSET_MSG_PAYLOAD_SIZE	7
#define OFFSET_ACK_PAYLOAD_SIZE	6
// Defines debug
#define DEBUG_FLAG 		2

// Functions
void APP_SX1272_setup(id_frame_t device);
uint8_t APP_SX1272_SendMsg(id_frame_t device, msg_frame_t *message, uint8_t nbRetries);
uint8_t APP_SX1272_trySendMsg(id_frame_t device, msg_frame_t *message);
uint8_t APP_SX1272_runTransmitMsg(id_frame_t device, msg_frame_t *message);
uint8_t APP_SX1272_runTransmitAck(id_frame_t device, ack_frame_t *ack);
uint8_t APP_SX1272_runReceive(id_frame_t device, msg_frame_t *message_decode, ack_frame_t *ack_decode);
uint8_t APP_SX1272_setFreq(id_frame_t device);
uint8_t APP_SX1272_pollingCAD(id_frame_t *device, unsigned long timeoutCH0, unsigned long timeoutCH1, unsigned long timeoutCH2);

#endif /* APP_INC_APPSX1272_H_ */
