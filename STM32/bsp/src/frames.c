/*
 * frames.c
 *
 *  Created on: 17 mai 2023
 *      Author: Nicolas
 */
#include <stdint.h>
#include "frames.h"

void decode_frame(uint8_t *payload, msg_frame_t *output_frame){
	output_frame->SOF = payload[0];

	output_frame->src.channel = (payload[1]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->src.address = payload[1] & ADDRESS_MASK;

	output_frame->dest.channel = (payload[2]>>CHANNEL_SHIFT) & CHANNEL_MASK;
	output_frame->dest.address = payload[2] & ADDRESS_MASK;

	output_frame->size = payload[3];


}

void encode_ack_frame(msg_frame_t ack_to_encode, uint8_t *frame_encoded){
	frame_encoded[0]= SOF_ACK_SYMBOL;
	frame_encoded[1]= (ack_to_encode.src.channel << CHANNEL_SHIFT)
					| (ack_to_encode.src.address & ADDRESS_MASK);
	frame_encoded[2]= (ack_to_encode.dest.channel << CHANNEL_SHIFT)
					| (ack_to_encode.dest.address & ADDRESS_MASK);
	uint16_t CRC = 0; //TODO : add CRC calculation
	frame_encoded[3]= (CRC>>CRC_SHIFT);
	frame_encoded[4]= (CRC&CRC_MASK);
	frame_encoded[5]= EOF_SYMBOL;
}

void encode_msg_frame(msg_frame_t ack_to_encode, uint8_t *frame_encoded){


}

