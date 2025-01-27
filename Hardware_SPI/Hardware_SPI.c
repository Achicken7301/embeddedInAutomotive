#include "Hardware_SPI.h"
 
void SPIx_Init()
{
	// Initialization struct
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Step 1: Initialize SPI
	RCC_APB2PeriphClockCmd(SPIx_RCC, ENABLE); // Pulse line - very important, check if APB1 or APB2
	//SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // 72 / 128 = 0.5625MHz
	
	
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV16; // Divine APB2 / 16
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // APB2 / 16 / 256 = 0.28125MHz
	
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_Init(SPIx, &SPI_InitStruct); 
	SPI_Cmd(SPIx, ENABLE);
	
	// Step 2: Initialize GPIO
	RCC_APB2PeriphClockCmd(SPI_GPIO_RCC, ENABLE);
	// GPIO pins for MOSI, MISO, and SCK
	GPIO_InitStruct.GPIO_Pin = SPI_PIN_MOSI | SPI_PIN_MISO | SPI_PIN_SCK;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // NOTE: GPIO_Mode_AF_PP - can be INPUT or OUTPUT -> I2C SDA_PIN
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	// GPIO pin for SS
	GPIO_InitStruct.GPIO_Pin = SPI_PIN_SS;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
	// Or using Dio library
	
	// Disable SPI slave device
	SPIx_DisableSlave();
}

uint8_t SPIx_Transfer(uint8_t data)
{
	// Write data to be transmitted to the SPI data register
	SPIx->DR = data;
	// Wait until transmit complete
	while (!(SPIx->SR & (SPI_I2S_FLAG_TXE)));
	// Wait until receive complete
	while (!(SPIx->SR & (SPI_I2S_FLAG_RXNE)));
	// Wait until SPI is not busy anymore
	//while (SPIx->SR & (SPI_I2S_FLAG_BSY));
	// Return received data from SPI data register
	return SPIx->DR;
}

void SPIx_EnableSlave()
{
	// Set slave SS pin low
	SPI_GPIO->BRR = SPI_PIN_SS;
}

void SPIx_DisableSlave()
{
	// Set slave SS pin high
	SPI_GPIO->BSRR = SPI_PIN_SS;
}
