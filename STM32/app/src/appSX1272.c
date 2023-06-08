/*
 * appSX1272.c
 *
 *  Created on: 25 ao�t 2020
 *      Author: Arnaud
 */

#include "main.h"
#include "appSX1272.h"
#include "SX1272.h"
#include "comSX1272.h"
#include "string.h"
#include "delay.h"

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

#define DEBUG_FLAG 2
#define SEND_MSG_ENCODED_FLAG 0
#define SEND_ACK_ENCODED_FLAG 1

///////////////////////////////////////////////////////////////
// Setup function
///////////////////////////////////////////////////////////////
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
		break;
	case 1:
		e = BSP_SX1272_setChannel(CH_868v3);
		break;
	case 2:
		e = BSP_SX1272_setChannel(CH_868v5);
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

void APP_SX1272_runTransmit(id_frame_t device) {
	uint8_t dest_address = TX_Addr;

	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1) {
#if SEND_MSG_ENCODED_FLAG

			LgMsg=strlen(Message);
			msg_frame_t message;
			message.src.channel = 1;
			message.src.address = 4;
			message.dest.channel = 1;
			message.dest.address = 2;
			message.size = 4;
			message.msg[0] = 'T';
			message.msg[1] = 'E';
			message.msg[2] = 'S';
			message.msg[3] = 'T';
			uint8_t message_encoded[SIZE_MSG_MAX+7];
			BSP_FRAMES_encodeMsgFrame(message, message_encoded);

			e = BSP_SX1272_sendPacketTimeout(dest_address, (char*)message_encoded,WaitTxMax);
		#endif

#if SEND_ACK_ENCODED_FLAG

		ack_frame_t message_ack;
		message_ack.src.channel = device.channel;
		message_ack.src.address = device.address;
		message_ack.dest.channel = 1;
		message_ack.dest.address = 1;

		uint8_t ack_encoded[SIZEOF_ACK];
		BSP_FRAMES_encodeAckFrame(message_ack, ack_encoded);

		e = BSP_SX1272_sendPacketTimeout(dest_address, (char*) ack_encoded, WaitTxMax);
#endif

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
		} else {
#if DEBUG_FLAG > 0
				my_printf("\n Trasmission problem !\r\n");
			#endif
		}
		BSP_DELAY_ms(waitPeriod); //delay to send packet every PeriodTransmission
	}
}

void APP_SX1272_runReceive(id_frame_t device) {
	char StatusRXMessage = '0';

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
		uint8_t crc_check;
		//////////////////////////////////////////////////////////////////////////////////
#if SEND_MSG_ENCODED_FLAG
		msg_frame_t message_decode;
		crc_check = BSP_FRAMES_decodeMsgFrame(currentstate.packet_received.data, &message_decode);
	#endif

#if SEND_ACK_ENCODED_FLAG
		ack_frame_t ack_decode;
		crc_check = BSP_FRAMES_decodeAckFrame(currentstate.packet_received.data, &ack_decode);
		if(ack_decode.dest.address == device.address)
		{
#if DEBUG_FLAG > 0
			my_printf("Bon destinataire\r\n");
			#endif
		}
		#endif
		//////////////////////////////////////////////////////////////////////////////////

		// Plot receive packets in the serial monitor
#if DEBUG_FLAG > 0
		my_printf("%d", StatusRXMessage);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.packnum);
		my_printf(" ; ");
		my_printf("%d", currentstate.packet_received.length);
		my_printf(" ;\n\r");
		my_printf("DATA : ");
		#endif
		for (uint8_t i = 0;
				i < currentstate.packet_received.length - OFFSET_PAYLOADLENGTH;
				i++) {
			my_printf("%c", currentstate.packet_received.data[i]);
			my_printf(" ");
		}
		///////////////////////////////////////////////////////////////////////////////////
		//CRC CHECK
#if DEBUG_FLAG > 0
		my_printf(" \n\r");
		my_printf("CRC Check : %d\n\r", crc_check);
		#endif

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
	BSP_DELAY_ms(1000);
}

/*
 Function: It set the SW1272 module on the frequence pass as argument of the function
   return = 2  --> The command has not been executed
   return = 1  --> There has been an error while executing the command
   return = 0  --> The command has been executed with no errors
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
