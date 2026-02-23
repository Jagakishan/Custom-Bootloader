#include <stdint.h>
#include "main.h"

uint32_t calculate_CRC(uint32_t startAddress, uint32_t size){
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	CRC->CR=CRC_CR_RESET;

	uint32_t *data=(uint32_t *)startAddress;
	uint32_t words=(size+3)/4;

	for(uint32_t i=0;i<words;i++){
		CRC->DR=data[i];
	}

	return CRC->DR;
}
