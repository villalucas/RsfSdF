/**
 * @file appSX1272.c
 * @brief Contient les fonctions pour la gestion de l'application SX1272.
 *
 * Le fichier appSX1272.c contient les définitions des fonctions pour la
 * configuration et l'exécution de l'application SX1272.
 */

#include "main.h"
#include "appSX1272.h"
#include "SX1272.h"
#include "comSX1272.h"
#include "string.h"
#include "delay.h"
#include "LED_control.h"
#include "stdlib.h"
#include "config.h"
#include "bsp.h"

/**
 * @brief État actuel du module SX1272.
 *
 * Cette variable globale externe est déclarée dans le fichier SX1272.h.
 * Elle est utilisée pour suivre l'état actuel du module SX1272.
 */

extern SX1272status currentstate;

// Declaration variables globales
static char LgMsg = 0;
static char Message[] =
		"Bonjour ceci est un test de transmission entre Nicolas et Evan en LoRa";
static float waitPeriod = 0; //en ms
static int cp = 0;  //compteur de paquets transmis
static int type_modulation = TypeModulation;
static uint16_t RegBitRate = BitRate;
static uint16_t RegFdev = Fdev;

// Status variables
static int8_t e;
static uint8_t ConfigOK = 1;


/**
 * @brief Fonction de configuration de l'application SX1272.
 *
 * Cette fonction configure le module SX1272 en spécifiant la modulation,
 * la fréquence, la puissance de sortie, etc.
 *
 * @param[in] device Structure d'identification du périphérique.
 */

void APP_SX1272_setup(id_frame_t device) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 setup : start\r\n");
	#endif
	// Power ON the module
	e = BSP_SX1272_ON(type_modulation);
	if (e == 0) {
#if DEBUG_FLAG > 1
		my_printf("SX1272 setup : module on\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 2
		my_printfln(F("SX1272 setup : Problem of activation of SX1272 Module !"));
		#endif
		ConfigOK = 0;
	}
	// Select frequency channel
	switch (device.channel) {
	case 0:
		e = BSP_SX1272_setChannel(CH_868v1);
#if DEBUG_FLAG > 2
		my_printf("SX1272 setup : Frequency channel ");
		my_printf("%d\n\r",CH_868v1);
		#endif
		BSP_LED_CONTROL_ledOn(LED_CH0);
		BSP_LED_CONTROL_ledOff(LED_CH1);
		BSP_LED_CONTROL_ledOff(LED_CH2);
		break;
	case 1:
		e = BSP_SX1272_setChannel(CH_868v3);
#if DEBUG_FLAG > 2
		my_printf("SX1272 setup : Frequency channel ");
		my_printf("%d\n\r",CH_868v3);
		#endif
		BSP_LED_CONTROL_ledOn(LED_CH1);
		BSP_LED_CONTROL_ledOff(LED_CH0);
		BSP_LED_CONTROL_ledOff(LED_CH2);
		break;
	case 2:
		e = BSP_SX1272_setChannel(CH_868v5);
#if DEBUG_FLAG > 2
		my_printf("SX1272 setup : Frequency channel ");
		my_printf("%d\n\r",CH_868v5);
		#endif
		BSP_LED_CONTROL_ledOn(LED_CH2);
		BSP_LED_CONTROL_ledOff(LED_CH0);
		BSP_LED_CONTROL_ledOff(LED_CH1);
		break;
	default:
		break;
	}

	if (e == 0) {
#if DEBUG_FLAG > 2
			my_printf("SX1272 setup : has been successfully set.\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 2
			my_printf("SX1272 setup : has not been set !\r\n");
		#endif
		ConfigOK = 0;
	}
	// Select output power
	e = BSP_SX1272_setPower(OutPower);
#if DEBUG_FLAG > 2
    my_printf("SX1272 setup : Output power ");
    my_printf("%d\r\n",OutPower);
	#endif
	if (e == 0) {
#if DEBUG_FLAG > 2
		my_printf("SX1272 setup : has been successfully set.\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 2
		my_printf("SX1272 setup : has not been set !\r\n");
		#endif
		ConfigOK = 0;
	}

	if (ConfigOK == 1) {
		// Config suppl�mentaire mode LORA
		if (type_modulation == 0) {
			// Set header
			e = BSP_SX1272_setHeaderON();
			// Set transmission mode
			e = BSP_SX1272_setCR(paramCR);    // CR_5 : CR = 4/5
			e = BSP_SX1272_setSF(paramSF);    // SF = 12
			e = BSP_SX1272_setBW(paramBW);    // BW = 125 KHz
			// Set CRC
			e = BSP_SX1272_setCRC_ON();
			// Set the node address
			e = BSP_SX1272_setNodeAddress(RX_Addr);
			// Set the length of preamble
			e = BSP_SX1272_setPreambleLength(PreambLong);
			// Set the number of transmission retries
			currentstate._maxRetries = MaxNbRetries;
		}
		// Config supplementaire mode FSK
		else {
			// Set CRC
			e = BSP_SX1272_setCRC_ON();
			// Set the node address
			e = BSP_SX1272_setNodeAddress(RX_Addr);
			// Set the length of preamble
			e = BSP_SX1272_setPreambleLength(PreambLong);
			// Set the number of transmission retries
			currentstate._maxRetries = MaxNbRetries;

			BSP_SX1272_Write(REG_SYNC_VALUE1, 0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE2, 0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE3, 0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE4, 0x05);

			//Set the frequency deviation an bit rate parameters
			BSP_SX1272_Write(REG_FDEV_MSB, (RegFdev >> 8) & 0x00FF);	// FDA = Fstep*FDEV = 61Hz*Fdev : ex: 0x7FF*61 = 125kHz ex2: 0X52*61=5kHz
			BSP_SX1272_Write(REG_FDEV_LSB, RegFdev & 0x00FF);			//...
			BSP_SX1272_Write(REG_BITRATE_MSB, (RegBitRate >> 8) & 0x00FF);	//FXOSC=32Mz, BR = FXOSC/(Bitrate + BitrateFrac/16), ex: FXOSC/0x682B = 1200 bauds, ex2: FXOSC/0x200=62.5 kbauds
			BSP_SX1272_Write(REG_BITRATE_LSB, RegBitRate & 0x00FF);			//...
		}
#if DEBUG_FLAG > 1
		my_printf("SX1272 setup : successfully configured !\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 1
		my_printf("SX1272 setup : initialization failed !\r\n");
		#endif
	}
	waitPeriod = PeriodTransmission;
	BSP_DELAY_ms(1000);
}

/**
 * @brief Fonction d envoi de message par le biais de l application SX1272 (limite par un nombre de tentative maximum).
 *
 * Cette fonction envoi un message par l application SX1272 en encapsulant notre protocol par competition.
 *
 * @param[in] device Structure d identification du peripherique.
 * @param[in] *message Structure du message a envoyer.
 * @param[in] nbRetries Octet non signe permettant de specifier le nombre de tentative maximal a effectuer en cas de problem lors de l envoie.
 */
uint8_t APP_SX1272_SendMsg(id_frame_t device, msg_frame_t *message, uint8_t nbRetries) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 sendMsg : start\r\n");
	#endif

	uint8_t transmit_status; // Status de la transmission
	transmit_status = APP_SX1272_trySendMsg(device, message);

	if (transmit_status == TRANSMIT_ERROR){
		return TRANSMIT_ERROR; // Erreur lors de la transmission
	}
	else if (transmit_status == RECEIVE_ACK_RECEIVED){
		return RECEIVE_ACK_RECEIVED; // Acquittement bien recu apres l envoi
	}
	else // Acquittement non recu
	{
		uint8_t i = 0;
		// Tentatives de renvoi
		while(i < nbRetries && transmit_status != RECEIVE_ACK_RECEIVED){
			uint32_t numrand =(uint32_t) ((rand() % (MSG_RETRY_DELAY_MAX - MSG_RETRY_DELAY_MIN)) + MSG_RETRY_DELAY_MIN);
#if DEBUG_FLAG > 0
			my_printf("SX1272 sendMsg : retry number %d ", (i+1));
			my_printf("and waiting %u ms\r\n", numrand);
			#endif
			BSP_DELAY_ms(numrand);
			transmit_status = APP_SX1272_trySendMsg(device, message);
			i++;
		}
		return transmit_status;
	}
}

/**
 * @brief Fonction d envoi de message par le biais de l application SX1272.
 *
 * Cette fonction envoi un message par l application SX1272 en encapsulant notre protocol par competition.
 *
 * @param[in] device Structure d identification du peripherique.
 * @param[in] *message Structure du message a envoyer.
 */
uint8_t APP_SX1272_trySendMsg(id_frame_t device, msg_frame_t *message) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 trySendMsg : start\r\n");
	#endif

	uint8_t transmit_status, receive_status; // Status de la transmission et de la reception
	uint32_t timeout_init = 0;
	msg_frame_t received_msg;
	ack_frame_t received_ack;

	transmit_status = APP_SX1272_runTransmitMsg(device, message);

	if (transmit_status == TRANSMIT_NO_ERROR) {
		timeout_init = BSP_millis();
		do {
			receive_status = APP_SX1272_runReceive(device, &received_msg,&received_ack);
			if ((BSP_millis() - timeout_init)> 5000 && receive_status != RECEIVE_ACK_RECEIVED) { //Second condition to avoid receive status being overwritten
				receive_status = RECEIVE_ERROR_ACK_TIMEOUT;
			}
		}
		while (receive_status != RECEIVE_ACK_RECEIVED && receive_status != RECEIVE_ERROR_ACK_TIMEOUT);
		if (receive_status == RECEIVE_ACK_RECEIVED) {
			return RECEIVE_ACK_RECEIVED;
		}
		else if (receive_status == RECEIVE_ERROR_ACK_TIMEOUT) {
#if DEBUG_FLAG > 0
			my_printf("SX1272 trySendMsg : RECEIVE_ERROR_ACK_TIMEOUT\r\n");
			#endif
			return RECEIVE_ERROR_ACK_TIMEOUT;
		}
	}
	else {
		return TRANSMIT_ERROR;
	}
	return	RECEIVE_ERROR_UNKNOWN_CASE;
}

/**
 * @brief Fonction pour executer la transmission de message SX1272.
 * @param device Structure contenant les informations du périphérique.
 * @param *message Structure contenant le message a envoyer dans le format du protocol.
 */
uint8_t APP_SX1272_runTransmitMsg(id_frame_t device, msg_frame_t *message) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 runTransmitMsg : start\r\n");
	#endif

	uint8_t dest_address = TX_Addr;
	uint8_t size_payload = message->size + OFFSET_MSG_PAYLOAD_SIZE; // Msg size + 7 Bytes used by the protocol
	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1) {
			BSP_LED_CONTROL_ledOn(LED_TX);	// TX LED On state for transmission

			uint8_t message_encoded[SIZE_MSG_MAX+7];
			BSP_FRAMES_encodeMsgFrame(message, message_encoded);
			e = BSP_SX1272_sendPacketTimeout(dest_address, (char*)message_encoded, WaitTxMax, size_payload);

			BSP_LED_CONTROL_ledOff(LED_TX);
		if (type_modulation) {
			BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
		}

		if (e == 0) {
			return TRANSMIT_NO_ERROR;
		}
		else {
			if (e == 2){
				return TRANSMIT_TIMEOUT_ERROR;
			}
			else {
				return TRANSMIT_ERROR;
			}
		}
	}
	else {
		return TRANSMIT_ERROR_CONFIG;
	}
}

/**
 * @brief Fonction pour executer la transmission d un acquittement SX1272.
 * @param device Structure contenant les informations du périphérique.
 * @param *ack Structure contenant l acquittement a envoyer dans le format du protocol.
 */
uint8_t APP_SX1272_runTransmitAck(id_frame_t device, ack_frame_t *ack) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 runTransmitAck : start\r\n");
	#endif

	uint8_t dest_address = TX_Addr;

	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1) {
			BSP_LED_CONTROL_ledOn(LED_TX);	// TX LED On state for transmission

			uint8_t ack_encoded[SIZE_MSG_MAX+7];
			uint8_t size_payload = OFFSET_ACK_PAYLOAD_SIZE;

			BSP_FRAMES_encodeAckFrame(ack, ack_encoded);
			e = BSP_SX1272_sendPacketTimeout(dest_address, (char*)ack_encoded,WaitTxMax, size_payload);

			BSP_LED_CONTROL_ledOff(LED_TX);
		if (type_modulation) {
			BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
		}

		if (e == 0) {
			return TRANSMIT_NO_ERROR;
		} else {
			if (e == 2){
				return TRANSMIT_TIMEOUT_ERROR;
			}
			else{
				return TRANSMIT_ERROR;
			}
		}
	}
	else{
		return TRANSMIT_ERROR_CONFIG;
	}
}


/**
 * @brief Fonction pour exécuter la réception de paquets SX1272
 * @param device Structure contenant les informations du périphérique.
 * @param *message_decode Structure contenant message a recevoir dans le format du protocol.
 * @param *ack_decode Structure contenant l acquittement a recevoir dans le format du protocol.
 */
uint8_t APP_SX1272_runReceive(id_frame_t device, msg_frame_t *message_decode, ack_frame_t *ack_decode) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 runReceive : start\r\n");
	#endif

	char StatusRXMessage = '0';

	uint8_t frame_type = 0;
	uint8_t crc_check;
	uint8_t	transmit_status;
	ack_frame_t transmit_ack;

	// Receive packets continuously
	if (ConfigOK == 1) {
		BSP_LED_CONTROL_ledOn(LED_RX);	// RX LED On state for reception
		// Affichage entete
		// Statut (correct = 1 or bad = 0 or non received = 2)
#if DEBUG_FLAG > 2
		my_printf("\n \r\n");
		my_printf("Packet status ; Packet number ; Received Lg ; Received data ; RSSI packet (dBm) ; source address; PER (%); BER (%)\r\n");
		my_printf("\n \r\n");
		#endif
		e = BSP_SX1272_receivePacketTimeout(WaitRxMax);
		// Paquet recu, correct ou non
		if (e == 0) {
			StatusRXMessage = '0';
			if (currentstate._reception == CORRECT_PACKET) {
				// Check if the received packet is correct
				// The length and the content of the packet is checked
				// If it is valid, the cpok counter is incremented
				LgMsg = strlen(Message);
				if (currentstate.packet_received.length >= LgMsg) { // Check the length
					if (memcmp(Message, currentstate.packet_received.data, LgMsg) == 0) { // Check the content
						StatusRXMessage = '1';
					}
				}
			}
		}
		// RX Timeout !! No packet received
		else {
			StatusRXMessage = '2';
		}

#if DEBUG_FLAG > 2
		my_printf("%d", StatusRXMessage);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.packnum);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.length);
		my_printf(" ;\n\r");
		#endif
#if DEBUG_FLAG > 2
		my_printf("\n\rDATA : ");
		for (uint8_t i = 0;	i < currentstate.packet_received.length - OFFSET_PAYLOADLENGTH;	i++) {
			my_printf("%c", currentstate.packet_received.data[i]);
			my_printf(" ");
		}
		my_printf("\r\n");
		#endif

		BSP_LED_CONTROL_ledOff(LED_RX);
		//Check if payload is an ack or a message
		switch(currentstate.packet_received.data[0])
		{
		case SOF_MSG_SYMBOL :
			frame_type = SOF_MSG_SYMBOL;
			crc_check = BSP_FRAMES_decodeMsgFrame(currentstate.packet_received.data, message_decode);
#if	DEBUG_FLAG > 0
			BSP_FRAMES_printMSGFrame(message_decode);
#endif
			break;
		case SOF_ACK_SYMBOL :
			frame_type = SOF_ACK_SYMBOL;
			crc_check = BSP_FRAMES_decodeAckFrame(currentstate.packet_received.data, ack_decode);
#if	DEBUG_FLAG > 0
			BSP_FRAMES_printACKFrame(ack_decode);
#endif
			break;
		default :
			return RECEIVE_SOF_ERROR;
			
			break;
		}
#if DEBUG_FLAG > 0
		my_printf(" \n\r");
		my_printf("CRC Check : %d\n\r", crc_check);
		#endif
		//Check for good CRC reception
		if(crc_check != CRC_NO_ERROR){
			return RECEIVE_CRC_ERROR;
		}

		//Check if payload is meant for this device
		if(frame_type == SOF_ACK_SYMBOL){
			if(ack_decode->dest.address == device.address && ack_decode->dest.channel == device.channel ){
				return RECEIVE_ACK_RECEIVED;
			}
			//The message wasn't for this device, abort and return WRONG_DESTINARY
			else return RECEIVE_WRONG_DESTINARY;
		}
		if(frame_type == SOF_MSG_SYMBOL){
			if(message_decode->dest.address == device.address && message_decode->dest.channel == device.channel ){
				transmit_ack.src.address = device.address;
				transmit_ack.src.channel = device.channel;
				transmit_ack.dest.address = message_decode->src.address;
				transmit_ack.dest.channel = message_decode->src.channel;
				BSP_LED_CONTROL_ledOn(LED_TX);	// TX LED On state for transmission
				transmit_status = APP_SX1272_runTransmitAck(device, &transmit_ack);
				BSP_LED_CONTROL_ledOff(LED_TX);
				if (transmit_status == TRANSMIT_NO_ERROR){
					return RECEIVE_MSG_RECEIVED_ACK_TRANSMITTED;
				}
				else
				{
					/*uint8_t retries = 0;
					do
					{
						transmit_status = APP_SX1272_runTransmitAck(device, &transmit_ack);
						retries++;
					}while(retries < 5 && transmit_status != TRANSMIT_NO_ERROR);*/
					return RECEIVE_MSG_RECEIVED_ACK_NOT_TRANSMITTED;
				}
			}
			//The message wasn't for this device, abort. Not an error as everything is normal
			else return RECEIVE_WRONG_DESTINARY;
		}
#if DEBUG_FLAG > 2
		// Plot RSSI
		// LORA mode
		if (TypeModulation == 0) {
			e = BSP_SX1272_getRSSIpacket();

			my_printf("RSSI :%d\r\n", currentstate._RSSIpacket);
		}
		// FSK mode
		else {
			//e = BSP_SX1272_getRSSI() done during RX, no packet RSSI available in FSK mode;
			//my_printf("%d\r\n",currentstate._RSSI);
		}
#endif
	}
	return RECEIVE_ERROR_UNKNOWN_CASE;
}

/*
   Fonction : Configure le module SX1272 avec la fréquence passée en argument

   Retour :
   - 2 : La commande n'a pas été exécutée
   - 1 : Une erreur s'est produite lors de l'exécution de la commande
   - 0 : La commande a été exécutée sans erreur
*/
uint8_t APP_SX1272_setFreq(id_frame_t device) {
#if DEBUG_FLAG > 1
	my_printf("SX1272 setFreq : start\r\n");
	#endif

    BSP_SX1272_Write(REG_OP_MODE, LORA_STANDBY_MODE); // LORA standby mode to switch off the RF field
    uint32_t freq;
    switch (device.channel) {
	case 0:
		freq = CH_868v1;
		BSP_LED_CONTROL_ledOff(LED_CH1);
		BSP_LED_CONTROL_ledOff(LED_CH2);
		BSP_LED_CONTROL_ledOn(LED_CH0);
		break;
	case 1:
		freq = CH_868v3;
		BSP_LED_CONTROL_ledOff(LED_CH0);
		BSP_LED_CONTROL_ledOff(LED_CH2);
		BSP_LED_CONTROL_ledOn(LED_CH1);
		break;
	case 2:
		freq = CH_868v5;
		BSP_LED_CONTROL_ledOff(LED_CH0);
		BSP_LED_CONTROL_ledOff(LED_CH2);
		BSP_LED_CONTROL_ledOn(LED_CH2);
		break;
	default:
		break;
	}
    // Select frequency channel
    e = BSP_SX1272_setChannel(freq);
#if DEBUG_FLAG > 2
	my_printf("SX1272 setFreq : Frequency channel ");
	my_printf("%d\r\n",freq);
    #endif

	if (e == 0) {
#if DEBUG_FLAG > 2
		my_printf("SX1272 setFreq : has been successfully set.\r\n");
		#endif
		return 0;
    }
    else {
#if DEBUG_FLAG > 2
		my_printf("SX1272 setFreq : has not been set !\r\n");
		#endif
		ConfigOK = 0;
		return 1;
    	}
	return 2;
}
