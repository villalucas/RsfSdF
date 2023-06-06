/**
 * \file frame.c
 * \brief Fonctions frame pour l'encodage et le décodage.
 * \author Nicolas, Evan
 * \version 0.1
 * \date 17 mai 2023
 *
 */
#include <stdint.h>
#include "frames.h"

/*
 * \fn void decode_frame(uint8_t *payload, msg_frame_t *output_frame)
 * \brief Fonction de décodage des messages receptionner dans la payload.
 *
 * \param payload Pointeur d'octets sur la base de la payload.
 * \param output_frame Pointeur de la structure d'objet msg_frame_t qui est définie par le protocole.
 *
 *
*/
void decode_frame(uint8_t *payload, msg_frame_t *output_frame){

	output_frame->SOF = payload[0];

	output_frame->src.channel = (payload[1]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->src.address = payload[1] & ADDRESS_MASK;

	output_frame->dest.channel = (payload[2]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->dest.address = payload[2] & ADDRESS_MASK;

	output_frame->size = payload[3];

	uint8_t i = 0;
	for(i = 0; i < output_frame->size; i++){
		output_frame->msg[i] = payload[i+4];
	}
	output_frame->CRC = (uint16_t)payload[output_frame->size+4];
	output_frame->CRC = (output_frame->CRC << CRC_SHIFT ) | (uint16_t)payload[output_frame->size+5]; // frist Byte MSB second Byte LSB
	output_frame->EOF = payload[output_frame->size+6];
	uint16_t CRC = 0; //TODO : add CRC calculation
}


/*
 * \fn encode_ack_frame(ack_frame_t ack_to_encode, uint8_t *frame_encoded)
 * \brief Fonction d'encodage des messages d'acquittement.
 *
 * \param ack_to_encode Pointeur de la structure d'objet ack_frame_t qui est définie par le protocole.
 *
*/
void encode_ack_frame(ack_frame_t ack_to_encode, uint8_t *frame_encoded){
	frame_encoded[0]= SOF_ACK_SYMBOL;
	frame_encoded[1]= (ack_to_encode.src.channel << CHANNEL_SHIFT)
					| (ack_to_encode.src.address & ADDRESS_MASK);
	frame_encoded[2]= (ack_to_encode.dest.channel << CHANNEL_SHIFT)
					| (ack_to_encode.dest.address & ADDRESS_MASK);
	uint16_t CRC = 0; //TODO : add CRC calculation
	frame_encoded[3]= (uint8_t)(CRC  >> CRC_SHIFT); // fisrt Byte MSB
	frame_encoded[4]= (uint8_t)(CRC & CRC_MASK); // second Byte LSB
	frame_encoded[5]= EOF_SYMBOL;
}

/*
 * \fn encode_msg_frame(msg_frame_t msg_to_encode, uint8_t *frame_encoded)
 * \brief Fonction d'encodage des messages .
 *
 * \param ack_to_encode Pointeur de la structure d'objet ack_frame_t qui est définie par le protocole.
 *
*/
void encode_msg_frame(msg_frame_t msg_to_encode, uint8_t *frame_encoded){
	frame_encoded[0]= SOF_MSG_SYMBOL;
	frame_encoded[1]= (msg_to_encode.src.channel << CHANNEL_SHIFT)
					| (msg_to_encode.src.address & ADDRESS_MASK);
	frame_encoded[2]= (msg_to_encode.dest.channel << CHANNEL_SHIFT)
					| (msg_to_encode.dest.address & ADDRESS_MASK);

	frame_encoded[3] = msg_to_encode.size;

	uint8_t i = 0;
	for(i = 0; i < output_frame->size; i++){
		frame_encoded[i+4] = msg_to_encode.msg[i];
	}
	uint16_t CRC = 0; //TODO : add CRC calculation
	frame_encoded[msg_to_encode.size+4]= (uint8_t)(CRC >> CRC_SHIFT); // fisrt Byte MSB
	frame_encoded[msg_to_encode.size+5]= (uint8_t)(CRC & CRC_MASK); // second Byte LSB
	frame_encoded[msg_to_encode.size+6]= EOF_SYMBOL;
}
