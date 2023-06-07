/**
 * \file frame.h
 * \brief Blibliothèque frame pour l'encodage et le décodage.
 * \author Nicolas, Evan
 * \version 0.1
 * \date 17 mai 2023
 *
 */

#ifndef APP_INC_FRAMES_H_
#define APP_INC_FRAMES_H_

#define SIZE_MSG_MAX		15

#define SOF_MSG_SYMBOL		'S'
#define SOF_ACK_SYMBOL		'A'
#define EOF_SYMBOL			'E'

#define CHANNEL_SHIFT 		6
#define CHANNEL_MASK 		0x03
#define ADDRESS_MASK 		0x3F
#define CRC_SHIFT 			8
#define CRC_MASK 			0x0F

// polynomial = X^16 + X^12 + X^5 + 1
#define POLYNOMIAL_CCITT 	0x1021
#define CRC_CCITT_SEED		0x1D0F

/**
 * \struct ID_frame
 * \brief Structure d'identifiant définie par le protocole.
 *
 * ID_frame contient le numéro de canal 0, 1 ou 2 et l'adresse du dispotitif
 */
typedef struct
{
	uint8_t channel;
	uint8_t address;

}ID_frame;

/**
 * \struct msg_frame_t
 * \brief Structure pour l'obget message qui est définie par le protocole.
 *
 * msg_frame_t contient :
 * SOF (start of frame)
 * ID src
 * ID dest
 * msg size
 * msg
 * CRC
 * EOF (end of frame)
 *
 */
typedef struct
{
	uint8_t SOF;
	ID_frame src;
	ID_frame dest;
	uint8_t size;
	uint8_t msg[SIZE_MSG_MAX];
	uint16_t crc;
	uint8_t EOF;//
}msg_frame_t;


/**
 * \struct ack_frame_t
 * \brief Structure pour l'obget acquittement qui est définie par le protocole.
 *
 * ack_frame_t contient :
 * SOF (start of frame)
 * ID src
 * ID dest
 * CRC
 * EOF (end of frame)
 *
 */
typedef struct
{
	uint8_t SOF;
	ID_frame src;
	ID_frame dest;
	uint16_t crc;
	uint8_t EOF;
}ack_frame_t;


/* Function */
uint16_t compute_crc(uint16_t crc, uint8_t data, uint16_t polynomial);
uint16_t packet_compute_crc(uint8_t *data, uint8_t datalength);
void decode_frame(uint8_t *payload, msg_frame_t *output_frame);
void encode_ack_frame(ack_frame_t ack_to_encode, uint8_t *frame_encoded);
void encode_msg_frame(msg_frame_t msg_to_encode, uint8_t *frame_encoded);


#endif /* APP_INC_FRAMES_H_ */
