/**
 * @file comSX1272.c
 * @brief Fonctions de communication avec le module SX1272.
 *
 * Ce fichier contient les définitions des fonctions permettant la communication avec le module SX1272.
 * Ces fonctions sont spécifiques à l'application et peuvent varier en fonction des besoins.
 *
 * @date 24 août 2020
 * @author Arnaud
 */


#include "comSX1272.h"

/**
 * @brief Fonction interne pour envoyer et recevoir des octets via l'interface SPI.
 *
 * Cette fonction envoie un octet via l'interface SPI et retourne l'octet reçu en réponse.
 *
 * @param tx_byte L'octet à envoyer.
 * @return L'octet reçu en réponse.
 */
static uint8_t BSP_SPI_SendReceive(uint8_t tx_byte);


/**
 * @brief Initialise la configuration de l'interface SPI1.
 *
 * Cette fonction configure l'interface SPI1 ainsi que les broches associées pour l'utilisation du module SPI.
 * Elle initialise les registres de configuration pour établir les paramètres suivants :
 * - Utilisation de PB6 comme broche de sélection du périphérique (CS).
 * - Utilisation de PA5, PA6 et PA7 comme broches d'interface SPI1.
 * - Configuration des broches en mode alternatif (AF) pour l'utilisation de l'interface SPI.
 * - Configuration du SPI1 en mode maître avec une vitesse de transmission de 6 MHz.
 *
 * @note Cette fonction suppose que les registres RCC, GPIOB, et GPIOA ont déjà été initialisés correctement.
 * @note Les détails spécifiques concernant les registres sont documentés dans le code source.
 */
void BSP_SPI1_Init()
{
	// SPI_SCK  -> PA5 (AF0)
	// SPI_MISO -> PA6 (AF0)
	// SPI_MOSI -> PA7 (AF0)
	// CS pin	-> PB6 (GPIO)

	/////////////////////////////////////////////////
	// Configure PB6 as CS pin
	//
	// Enable GPIOB clock
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB6 as output
	GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk;
	GPIOB->MODER |= (0x01 <<GPIO_MODER_MODER6_Pos);

	// Configure PB6 as Push-Pull output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;

	// Configure PB6 as High-Speed Output
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR6_Msk;
	GPIOB->OSPEEDR |= (0x03 <<GPIO_OSPEEDR_OSPEEDR6_Pos);

	// Disable PB6 Pull-up/Pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_Msk;

	// Idle state is high
	GPIOB->BSRR = GPIO_BSRR_BS_6;

	///////////////////////////////////////////////////
	// Configure PA5, PA6, PA7 as SPI1 pins (AF0)
	//
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA5, PA6, PA7 as AF mode
	GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOA->MODER |= (0x02 <<10U) |(0x02 <<12U) |(0x02 <<14U);

	// Connect to SPI1 (AF0)
	GPIOA->AFR[0] &= ~(0xFFF00000);
	GPIOA->AFR[0] |=  (0x00000000);

	// Enable SPI1 Clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Configure SPI with default config
	// 4-wire = full-duplex (MISO/MOSI on separated lines)
	// 8-bit, no CRC, MSB first
	// CPHA = 0, CPOL = 0 -> SPI mode 0 (first clock rising edge)
	SPI1->CR1 = 0x0000;
	SPI1->CR2 = 0x0000;

	// Set the baudrate to 48MHz /8 = 6MHz (maximum speed 10MHz)
	SPI1->CR1 |= 0x02 <<SPI_CR1_BR_Pos;

	// Set data size to 8-bit
	SPI1->CR2 |= 0x07 <<SPI_CR2_DS_Pos;

	// Set as master (SSI must be high), with software managed NSS
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;

	// Enable SPI1
	SPI1->CR1 |= SPI_CR1_SPE;
}


/**
 * @brief Lit une valeur à partir d'un registre spécifié du module SX1272.
 *
 * Cette fonction lit la valeur d'un registre spécifié du module SX1272 via l'interface SPI1.
 * Elle configure le seuil FIFO sur 1 octet, sélectionne l'esclave en mettant la broche de sélection (CS) à l'état bas,
 * envoie l'adresse du registre à lire, reçoit la valeur du registre via l'interface SPI, puis libère l'esclave en mettant
 * la broche CS à l'état haut.
 *
 * @param register_address L'adresse du registre à lire.
 * @return La valeur lue à partir du registre spécifié.
 *
 * @note Cette fonction suppose que les registres SPI1, GPIOB et GPIOA ont déjà été initialisés correctement.
 * @note Les détails spécifiques concernant les registres sont documentés dans le code source.
 */
uint8_t BSP_SX1272_Read(uint8_t register_address)
{
	uint8_t data=0;
	// Set FIFO threshold to 1-byte
	SPI1->CR2 |= SPI_CR2_FRXTH;

	// Select slave (CS -> low)
	GPIOB->BSRR = GPIO_BSRR_BR_6;

	// Send register address to read from
	BSP_SPI_SendReceive(register_address);

	data = BSP_SPI_SendReceive(0x00);

	// Release slave (CS -> High)
	GPIOB->BSRR = GPIO_BSRR_BS_6;

	return data;
}


/**
 * @brief Écrit une valeur dans un registre spécifié du module SX1272.
 *
 * Cette fonction écrit une valeur spécifiée dans un registre spécifié du module SX1272 via l'interface SPI1.
 * Elle configure le seuil FIFO sur 1 octet, sélectionne l'esclave en mettant la broche de sélection (CS) à l'état bas,
 * envoie l'adresse du registre à écrire en lui ajoutant le bit d'écriture (0x80), envoie la donnée à écrire,
 * puis libère l'esclave en mettant la broche CS à l'état haut.
 *
 * @param register_address L'adresse du registre dans lequel écrire.
 * @param data La valeur à écrire dans le registre spécifié.
 *
 * @note Cette fonction suppose que les registres SPI1, GPIOB et GPIOA ont déjà été initialisés correctement.
 * @note Les détails spécifiques concernant les registres sont documentés dans le code source.
 */
void BSP_SX1272_Write(uint8_t register_address, uint8_t data)
{
	// Set FIFO threshold to 1-byte
	SPI1->CR2 |= SPI_CR2_FRXTH;

	// Select slave (CS -> low)
	GPIOB->BSRR = GPIO_BSRR_BR_6;

	// Send register address to write to
	BSP_SPI_SendReceive(register_address | 0x80);

	// Send data to write
	BSP_SPI_SendReceive(data);

	// Release slave (CS -> High)
	GPIOB->BSRR = GPIO_BSRR_BS_6;
}

/**
 * @brief Envoie un octet via l'interface SPI1 et retourne l'octet reçu.
 *
 * Cette fonction envoie un octet spécifié via l'interface SPI1 et attend la réception d'un octet en retour.
 * Elle s'assure que le drapeau de transmission (TXE) est positionné avant d'envoyer les données,
 * envoie l'octet spécifié, attend que les données entrantes arrivent et lit l'octet reçu.
 * L'octet reçu est ensuite retourné par la fonction.
 *
 * @param tx_byte L'octet à envoyer via l'interface SPI1.
 *
 * @return L'octet reçu via l'interface SPI1.
 *
 * @note Cette fonction suppose que le registre SPI1 a déjà été initialisé correctement.
 * @note Les détails spécifiques concernant les registres sont documentés dans le code source.
 */
static uint8_t BSP_SPI_SendReceive(uint8_t tx_byte)
{
	uint8_t	rx_byte;

	// Make sure TXE is set before sending data
	while((SPI1->SR & SPI_SR_TXE_Msk) == 0);

	// Send tx_byte
	*(__IO uint8_t *)&SPI1->DR = tx_byte;

	// Wait until incoming data has arrived
	while((SPI1->SR & SPI_SR_RXNE_Msk) == 0);

	// Read data
	rx_byte = *(__IO uint8_t *)&SPI1->DR;

	return rx_byte;
}
