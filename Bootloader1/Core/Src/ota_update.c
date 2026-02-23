#include "ota_update.h"
#include "ota_image.h"
#include "image_header.h"
#include "main.h"
#include <stdio.h>
#include "flash_write.h"
#include "crc.h"

#define OTA_MAGIC 0xB00170AD
#define APP_MAX_SIZE 0x8000
#define APP1_START 0x08008800U

extern const uint8_t ota_image[];
extern const uint32_t ota_image_size;

image_header_struct *tempStruct=(image_header_struct *)ota_image;

void clearOTAflag(){
	uint32_t tempBuff[7]={0};
	for(volatile uint8_t i=0;i<7;i++){
		tempBuff[i]=*(uint32_t *)(APP1_HDR_START+i*0x4);
	}

	tempBuff[5]=0x0;
	flash_write_header(APP1_HDR_START, tempBuff, 7);
}

static uint8_t verify_update(){
	if(tempStruct->magic!=OTA_MAGIC){
		printf("OTA failed. Magic mismatch\r\n");
		return 0;
	}

	if(ota_image_size>APP_MAX_SIZE){
		printf("OTA failed. Not enough memory\r\n");
		return 0;
	}

	return 1;
}

void OTA_update(){
	printf("OTA update....\r\n");

	if(verify_update()){
		uint32_t preciseAppSize=ota_image_size-0x800; //2KB of flash for header.

		uint32_t calcCRC=calculate_CRC((uint32_t)ota_image+2048, preciseAppSize);

		if(tempStruct->crc!=calcCRC){
			printf("OTA failed. CRC mismatch\r\n");
			return;
		}

		flash_app_erase(APP1_START);

		uint8_t *writeBuffer;
		uint32_t writeAddress;
		uint16_t writeCycles=(preciseAppSize +255)/256;

		for(volatile uint16_t i=0;i<writeCycles;i++){
			writeBuffer=(uint8_t *)ota_image + 2048+(i*256);
			writeAddress=2048+i*256;
			if(flash_write_app(writeAddress, writeBuffer, 256)!=HAL_OK){
				printf("OTA flashing failed\r\n");
				return;
			}
		}

		printf("OTA firmware update successful\r\n");
	}

	return;
}
