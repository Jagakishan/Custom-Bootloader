#include <stdio.h>

typedef struct{
	uint32_t magic;
	uint32_t crc;
	uint32_t imageSize;
	uint32_t confirmationFlag;
	uint32_t rejectionFlag;
	uint32_t OTAflag;
	uint32_t version;
}image_header_struct;
