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
#include "main.h"


/*
 * \fn BSP_FRAMES_computeCrc(uint16_t crc, uint8_t data, uint16_t polynomial)
 * \brief Fonction de calcul du CRC.
 *
 * \param crc octet du crc pour un octet de la donnée.
 * \param data octet de la donnée.
 * \param polynomial 2 octets pour le polynome utilisé par le calcul du CRC.
 *
*/
uint16_t BSP_FRAMES_computeCrc(uint16_t crc, uint8_t data, uint16_t polynomial){
	uint8_t i;

	for(i = 0; i < 8; i++){

		if( ( ( (crc & 0x8000) >> 8 ) ^ (data & 0x80) ) != 0 ){
			crc <<= 1;			// décalage à gauche de 1
			crc ^= polynomial;	// XOR avec le polynome
		}
		else
		{
			crc <<= 1;			// décalage à gauche de 1
		}
		data <<= 1;				// prochain bit de donnée
	}
	return crc;
}

/*
 * \fn BSP_FRAMES_packetComputeCrc(uint8_t *data, uint8_t datalength)
 * \brief Fonction de calcul du CRC pour l'ensemble de la frame..
 *
 * \param data Pointeur sur le tableau d'octet qui est la frame.
 * \param datalength taille du tableau.
 *
*/
uint16_t BSP_FRAMES_packetComputeCrc(uint8_t *data, uint8_t datalength){
	uint8_t i;
	uint16_t crc;
	uint16_t polynomial;

	polynomial = POLYNOMIAL_CCITT;
	crc = CRC_CCITT_SEED;

	for(i = 0; i < datalength; i++){
		crc = BSP_FRAMES_computeCrc(crc, data[i], polynomial);
	}

	return (uint16_t)(~crc);
}


/*
 * \fn BSP_FRAMES_decodeMsgFrame(uint8_t *payload, msg_frame_t *output_frame)
 * \brief Fonction de décodage des messages receptionner dans la payload.
 *
 * \param payload Pointeur d'octets sur la base de la payload.
 * \param output_frame Pointeur de la structure d'objet msg_frame_t qui est définie par le protocole.
 *
 *
*/
uint8_t BSP_FRAMES_decodeMsgFrame(uint8_t *payload, msg_frame_t *output_frame){

	output_frame->sof = payload[0];

	output_frame->src.channel = (payload[1]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->src.address = payload[1] & ADDRESS_MASK;

	output_frame->dest.channel = (payload[2]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->dest.address = payload[2] & ADDRESS_MASK;

	output_frame->size = payload[3];

	uint8_t i = 0;
	for(i = 0; i < output_frame->size; i++){
		output_frame->msg[i] = payload[i+4];
	}
	output_frame->crc = (uint16_t)payload[(output_frame->size)+4];
	output_frame->crc = (output_frame->crc << CRC_SHIFT ) | (uint16_t)payload[output_frame->size+5]; // frist Byte MSB second Byte LSB
	output_frame->eof = payload[output_frame->size+6];

	uint16_t crc = 0;
	crc = BSP_FRAMES_packetComputeCrc(payload, (output_frame->size) +4);

	if (crc == output_frame->crc)
	{
		my_printf("CRC identiques !\n\r");
		return 0;
	}else
	{
		my_printf(" Probleme de CRC... \n\r");
		return 1;
	}
	my_printf("\n\r");
}

/*
 * \fn BSP_FRAMES_decodeAckFrame(uint8_t *payload, ack_frame_t *output_frame)
 * \brief Fonction de décodage des messages receptionner dans la payload.
 *
 * \param payload Pointeur d'octets sur la base de la payload.
 * \param output_frame Pointeur de la structure d'objet msg_frame_t qui est définie par le protocole.
 *
 *
*/
uint8_t BSP_FRAMES_decodeAckFrame(uint8_t *payload, ack_frame_t *output_frame){

	output_frame->sof = payload[0];

	output_frame->src.channel = (payload[1]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->src.address = payload[1] & ADDRESS_MASK;

	output_frame->dest.channel = (payload[2]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->dest.address = payload[2] & ADDRESS_MASK;

	output_frame->crc = (uint16_t)payload[3];
	output_frame->crc = (output_frame->crc << CRC_SHIFT ) | (uint16_t)payload[4]; // frist Byte MSB second Byte LSB
	output_frame->eof = payload[5];

	uint16_t crc = 0;
	crc = BSP_FRAMES_packetComputeCrc(payload, 3);

	if (crc == output_frame->crc)
	{
		my_printf("CRC identiques !\n\r");
		return 0;
	}else
	{
		my_printf("Probleme de CRC \n\r");
		return 1;
	}
	my_printf("\n\r");
}

/*
 * \fn BSP_FRAMES_encodeAckFrame(ack_frame_t ack_to_encode, uint8_t *frame_encoded)
 * \brief Fonction d'encodage des messages d'acquittement.
 *
 * \param ack_to_encode Pointeur de la structure d'objet ack_frame_t qui est définie par le protocole.
 *
*/
void BSP_FRAMES_encodeAckFrame(ack_frame_t ack_to_encode, uint8_t *frame_encoded){
	frame_encoded[0]= SOF_ACK_SYMBOL;
	frame_encoded[1]= (ack_to_encode.src.channel << CHANNEL_SHIFT)
					| (ack_to_encode.src.address & ADDRESS_MASK);
	frame_encoded[2]= (ack_to_encode.dest.channel << CHANNEL_SHIFT)
					| (ack_to_encode.dest.address & ADDRESS_MASK);
	uint16_t crc = 0;
	crc = BSP_FRAMES_packetComputeCrc(frame_encoded, 3);
	frame_encoded[3]= (uint8_t)(crc  >> CRC_SHIFT); 	// fisrt Byte MSB
	frame_encoded[4]= (uint8_t)(crc & CRC_MASK); 		// second Byte LSB
	frame_encoded[5]= EOF_SYMBOL;
}


/*
 * \fn BSP_FRAMES_encodeMsgFrame(msg_frame_t msg_to_encode, uint8_t *frame_encoded)
 * \brief Fonction d'encodage des messages .
 *
 * \param ack_to_encode Pointeur de la structure d'objet ack_frame_t qui est définie par le protocole.
 *
*/
void BSP_FRAMES_encodeMsgFrame(msg_frame_t msg_to_encode, uint8_t *frame_encoded){
	frame_encoded[0]= SOF_MSG_SYMBOL;
	frame_encoded[1]= (msg_to_encode.src.channel << CHANNEL_SHIFT)
					| (msg_to_encode.src.address & ADDRESS_MASK);
	frame_encoded[2]= (msg_to_encode.dest.channel << CHANNEL_SHIFT)
					| (msg_to_encode.dest.address & ADDRESS_MASK);

	frame_encoded[3] = msg_to_encode.size;

	uint8_t i = 0;
	for(i = 0; i < msg_to_encode.size; i++){
		frame_encoded[i+4] = msg_to_encode.msg[i];
	}
	uint16_t crc = 0;
	crc = BSP_FRAMES_packetComputeCrc(frame_encoded, msg_to_encode.size+4);
	frame_encoded[msg_to_encode.size+4]= (uint8_t)(crc >> CRC_SHIFT); 	// fisrt Byte MSB
	frame_encoded[msg_to_encode.size+5]= (uint8_t)(crc & CRC_MASK); 	// second Byte LSB
	frame_encoded[msg_to_encode.size+6]= EOF_SYMBOL;
}

