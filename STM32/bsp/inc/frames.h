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

#define SIZE_MSG_MAX	15

#define SOF_MSG_SYMBOL	'S'
#define SOF_ACK_SYMBOL	'A'
#define EOF_SYMBOL		'E'

#define CHANNEL_SHIFT 	6
#define CHANNEL_MASK 	0x03
#define ADDRESS_MASK 	0x3F
#define CRC_SHIFT 8
#define CRC_MASK 0x0F

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
	uint8_t SOF = SOF_MSG_SYMBOL ;
	ID_frame src;
	ID_frame dest;
	uint8_t size;
	uint8_t msg[SIZE_MSG_MAX];
	uint16_t CRC;
	uint8_t EOF = EOF_SYMBOL;
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
	uint8_t SOF = SOF_ACK_SYMBOL;
	ID_frame src;
	ID_frame dest;
	uint16_t CRC;
	uint8_t EOF = EOF_SYMBOL;
}ack_frame_t;


#endif /* APP_INC_FRAMES_H_ */
