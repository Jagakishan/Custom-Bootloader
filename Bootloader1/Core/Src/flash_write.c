#include "main.h"
#include <stdint.h>

#define APP_MAX_SIZE 0x8000
#define PAGE_SIZE 0x800

HAL_StatusTypeDef flash_write_header(uint32_t addr, uint32_t *data, uint32_t size){
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

HAL_StatusTypeDef flash_app_erase(uint32_t addr){
	FLASH_EraseInitTypeDef eraser;
	uint32_t error;

	HAL_FLASH_Unlock();

	eraser.TypeErase=FLASH_TYPEERASE_PAGES;
	eraser.PageAddress=addr;
	eraser.NbPages=APP_MAX_SIZE/PAGE_SIZE;

	HAL_FLASHEx_Erase(&eraser, &error);

	HAL_FLASH_Lock();

	return HAL_OK;
}

HAL_StatusTypeDef flash_write_app(uint32_t addr, uint8_t *data, uint32_t size){
	HAL_FLASH_Unlock();
	uint32_t *tempData;

	for(int i=0;i<size;i+=4){
		tempData=(uint32_t *)(data + i);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+(i*4), tempData[i]);
	}

	HAL_FLASH_Lock();

	return HAL_OK;
}
