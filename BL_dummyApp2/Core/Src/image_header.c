#include <stdint.h>

#define IMAGE_MAGIC 0xB00710ADU

typedef struct{
	uint32_t magic;
	uint32_t image_size;
	uint32_t crc32;
	uint32_t version;
	uint32_t pending_flag;
	uint32_t reserved[27];  //27 elements because total 128 bytes alignment required for VTOR.
}image_struct;

__attribute__((section(".image_header")))
__attribute__((used))
const image_struct img_header={
		.magic=(uint32_t)IMAGE_MAGIC,
		.image_size=0x1238, //Will be changed during patching from python
		.crc32=0xABCDEF12, //Will be changed patching from python
		.version=0x00000010,
		.pending_flag=0x01,
};
