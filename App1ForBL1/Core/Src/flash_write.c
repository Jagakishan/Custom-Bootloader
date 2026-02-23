#include "main.h"
#include <stdint.h>

HAL_StatusTypeDef flash_write(uint32_t addr, uint32_t *data, uint32_t size){
	FLASH_EraseInitTypeDef eraser;
	uint32_t pageError;

	HAL_FLASH_Unlock();

	eraser.TypeErase=FLASH_TYPEERASE_PAGES;
	eraser.PageAddress=addr;
	eraser.NbPages=1;

	HAL_FLASHEx_Erase(&eraser, &pageError);

	for(int i=0;i<size;i++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+(i*4), data[i]);
	}

	HAL_FLASH_Lock();

	return HAL_OK;
}
