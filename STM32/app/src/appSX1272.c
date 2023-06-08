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

/**
 * @brief État actuel du module SX1272.
 *
 * Cette variable globale externe est déclarée dans le fichier SX1272.h.
 * Elle est utilisée pour suivre l'état actuel du module SX1272.
 */

extern SX1272status currentstate;

///////////////////////////////////////////////////////////////
// D�claration variables globales
///////////////////////////////////////////////////////////////
static char LgMsg = 0;
static char Message[] =
		"Bonjour ceci est un test de transmission entre Nicolas et Evan en LoRa";
static float waitPeriod = 0; //en ms
static int cp = 0;  //compteur de paquets transmis
static int type_modulation = TypeModulation;
static uint16_t RegBitRate = BitRate;
static uint16_t RegFdev = Fdev;

// status variables
static int8_t e;
static uint8_t ConfigOK = 1;

// Debug Constants
#define DEBUG_FLAG 2
#define SEND_MSG_ENCODED_FLAG 1
#define SEND_ACK_ENCODED_FLAG 0

///////////////////////////////////////////////////////////////
// Setup function
///////////////////////////////////////////////////////////////

/**
 * @brief Fonction de configuration de l'application SX1272.
 *
 * Cette fonction configure le module SX1272 en spécifiant la modulation,
 * la fréquence, la puissance de sortie, etc.
 *
 * @param[in] device Structure d'identification du périphérique.
 */

void APP_SX1272_setup(id_frame_t device) {
	// Power ON the module
	e = BSP_SX1272_ON(type_modulation);
	if (e == 0) {
#if DEBUG_FLAG > 0
			my_printf("SX1272 Module on\r\n");
		#endif
	} else {
		//my_printfln(F("Problem of activation of SX1272 Module !"));
		ConfigOK = 0;
	}
	// Select frequency channel
	switch (device.channel) {
	case 0:
		e = BSP_SX1272_setChannel(CH_868v1);
		LedOn(LED_CH0);
		LedOff(LED_CH1);
		LedOff(LED_CH2);
		break;
	case 1:
		e = BSP_SX1272_setChannel(CH_868v3);
		LedOn(LED_CH1);
		LedOff(LED_CH0);
		LedOff(LED_CH2);
		break;
	case 2:
		e = BSP_SX1272_setChannel(CH_868v5);
		LedOn(LED_CH2);
		LedOff(LED_CH0);
		LedOff(LED_CH1);
		break;
	default:
		break;
	}
#if DEBUG_FLAG > 0
		my_printf("Frequency channel ");
		my_printf("%d",CH_868v1);
	#endif

	if (e == 0) {
#if DEBUG_FLAG > 0
			my_printf(" has been successfully set.\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 0
			my_printf(" has not been set !\r\n");
		#endif
		ConfigOK = 0;
	}
	// Select output power
	e = BSP_SX1272_setPower(OutPower);
#if DEBUG_FLAG > 0
	  my_printf("Output power ");
	  my_printf("%d",OutPower);
	#endif
	if (e == 0) {
#if DEBUG_FLAG > 0
			my_printf(" has been successfully set.\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 0
			my_printf(" has not been set !\r\n");
		#endif
		ConfigOK = 0;
	}

	if (ConfigOK == 1) {
		//////////////////////////////////////////////////////////////////////
		//config suppl�mentaire mode LORA
		//////////////////////////////////////////////////////////////////////
		if (type_modulation == 0) {
			// Set header
			e = BSP_SX1272_setHeaderON();
			// Set transmission mode
			e = BSP_SX1272_setCR(paramCR);    // CR_5 : CR = 4/5
			e = BSP_SX1272_setSF(paramSF);   	// SF = 12
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
		//////////////////////////////////////////////////////////////////////
		//config suppl�mentaire mode FSK
		//////////////////////////////////////////////////////////////////////
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
			BSP_SX1272_Write(REG_FDEV_MSB, (RegFdev >> 8) & 0x00FF);// FDA = Fstep*FDEV = 61Hz*Fdev : ex: 0x7FF*61 = 125kHz ex2: 0X52*61=5kHz
			BSP_SX1272_Write(REG_FDEV_LSB, RegFdev & 0x00FF);	//...
			BSP_SX1272_Write(REG_BITRATE_MSB, (RegBitRate >> 8) & 0x00FF);//FXOSC=32Mz, BR = FXOSC/(Bitrate + BitrateFrac/16), ex: FXOSC/0x682B = 1200 bauds, ex2: FXOSC/0x200=62.5 kbauds
			BSP_SX1272_Write(REG_BITRATE_LSB, RegBitRate & 0x00FF);	//...

		}
#if DEBUG_FLAG > 0
		my_printf("SX1272 successfully configured !\r\n");
		#endif
	} else {
#if DEBUG_FLAG > 0
		my_printf("SX1272 initialization failed !\r\n");
		#endif
	}

	waitPeriod = PeriodTransmission;

	BSP_DELAY_ms(1000);
}


/**
 * @brief Fonction pour exécuter la transmission de paquets SX1272
 * @param device Structure contenant les informations du périphérique
 */

uint8_t APP_SX1272_runTransmitMsg(id_frame_t device, msg_frame_t *message) {

	uint8_t dest_address = TX_Addr;
	LedOn(LED_TX);	// TX LED On state for transmission

	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1) {
			uint8_t message_encoded[SIZE_MSG_MAX+7];
			BSP_FRAMES_encodeMsgFrame(message, message_encoded);
			e = BSP_SX1272_sendPacketTimeout(dest_address, (char*)message_encoded,WaitTxMax);

		if (type_modulation) {
			BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
		}

		if (e == 0) {
#if DEBUG_FLAG > 0
				my_printf("\n Packet number ");
				my_printf("%d",cp);
				my_printf(" ;Rx node address ");
				my_printf("%d\r\n",dest_address);
				cp++;
			#endif
			return TRANSMIT_NO_ERROR;
		} else {
#if DEBUG_FLAG > 0
				my_printf("\n Trasmission problem !\r\n");
			#endif
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
	LedOff(LED_TX);
}


uint8_t APP_SX1272_runTransmitAck(id_frame_t device, ack_frame_t *ack) {
	uint8_t dest_address = TX_Addr;

	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1) {
			uint8_t ack_encoded[SIZE_MSG_MAX+7];
			BSP_FRAMES_encodeAckFrame(ack, ack_encoded);
			e = BSP_SX1272_sendPacketTimeout(dest_address, (char*)ack_encoded,WaitTxMax);

		if (type_modulation) {
			BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
		}

		if (e == 0) {
#if DEBUG_FLAG > 0
				my_printf("\n Packet number ");
				my_printf("%d",cp);
				my_printf(" ;Rx node address ");
				my_printf("%d\r\n",dest_address);
				cp++;
			#endif
			return TRANSMIT_NO_ERROR;
		} else {
#if DEBUG_FLAG > 0
				my_printf("\n Trasmission problem !\r\n");
			#endif
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
 * @param device Structure contenant les informations du périphérique
 */


uint8_t APP_SX1272_runReceive(id_frame_t device, msg_frame_t *message_decode, ack_frame_t *ack_decode) {

	char StatusRXMessage = '0';
	uint8_t frame_type = 0;
	uint8_t crc_check;
	LedOn(LED_RX);	// TX LED On state for transmission
	//////////////////////////////////////////////////////////////////////////////////
	// Receive packets continuously
	if (ConfigOK == 1) {
		//affichage ent�te
		//statut (correct = 1 or bad = 0 or non received = 2)
#if DEBUG_FLAG > 0
		my_printf("\n \r\n");
		my_printf("Packet status ; Packet number ; Received Lg ; Received data ; RSSI packet (dBm) ; source address; PER (%); BER (%)\r\n");
		my_printf("\n \r\n");
		#endif
		e = BSP_SX1272_receivePacketTimeout(WaitRxMax);
		//paquet re�u, correct ou non
		if (e == 0) {
			StatusRXMessage = '0';
			if (currentstate._reception == CORRECT_PACKET) {
				// Check if the received packet is correct
				// The length and the content of the packet is checked
				// if it is valid, the cpok counter is incremented
				LgMsg = strlen(Message);
				if (currentstate.packet_received.length >= LgMsg) //check the length
						{
					if (memcmp(Message, currentstate.packet_received.data, LgMsg) == 0)       //check the content
							{
						StatusRXMessage = '1';
					}
				}
			}
		}
		// RX Timeout !! No packet received
		else {
			StatusRXMessage = '2';
		}

#if DEBUG_FLAG > 0
		my_printf("%d", StatusRXMessage);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.packnum);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.length);
		my_printf(" ;\n\r");
		my_printf("DATA : ");
		for (uint8_t i = 0;
				i < currentstate.packet_received.length - OFFSET_PAYLOADLENGTH;
				i++) {
			my_printf("%c", currentstate.packet_received.data[i]);
			my_printf(" ");
		}
#endif
		LedOff(LED_RX);
		//Check if payload is an ack or a message
		switch(currentstate.packet_received.data[0])
		{
		case SOF_MSG_SYMBOL :
			frame_type = SOF_MSG_SYMBOL;
			crc_check = BSP_FRAMES_decodeMsgFrame(currentstate.packet_received.data, message_decode);
			break;
		case SOF_ACK_SYMBOL :
			frame_type = SOF_ACK_SYMBOL;
			crc_check = BSP_FRAMES_decodeAckFrame(currentstate.packet_received.data, ack_decode);
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
				return RECEIVE_MSG_RECEIVED;
			}
			//The message wasn't for this device, abort. Not an error as everything is normal
			else return RECEIVE_WRONG_DESTINARY;
		}

#if DEBUG_FLAG > 0
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
	}

#endif
	return RECEIVE_ERROR_UNKNOWN_CASE;

}

/*
   Fonction : Configure le module SX1272 avec la fréquence passée en argument

   Retour :
   - 2 : La commande n'a pas été exécutée
   - 1 : Une erreur s'est produite lors de l'exécution de la commande
   - 0 : La commande a été exécutée sans erreur
*/
uint8_t APP_SX1272_setFreq(id_frame_t device)
{
    BSP_SX1272_Write(REG_OP_MODE, LORA_STANDBY_MODE); // LORA standby mode to switch off the RF field
    uint32_t freq;
    switch (device.channel) {
	case 0:
		freq = CH_868v1;
		break;
	case 1:
		freq = CH_868v3;
		break;
	case 2:
		freq = CH_868v5;
		break;
	default:
		break;
	}
    // Select frequency channel
    e = BSP_SX1272_setChannel(freq);
	#if (DEBUG_FLAG > 0)
		my_printf("Frequency channel ");
		my_printf("%d",freq);
    #endif
    if (e == 0)
    {
		#if (DEBUG_FLAG > 0)
        	my_printf(" has been successfully set.\r\n");
		#endif

      return 0;
    }
    else
    {
		#if (DEBUG_FLAG > 0)
			my_printf(" has not been set !\r\n");
		#endif
		ConfigOK = 0;
      return 1;
    }
	return 2;
}
