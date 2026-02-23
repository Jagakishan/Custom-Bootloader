#include <stdint.h>
#include "image_header.h"

__attribute__((section(".image_header")))
__attribute((used))
const image_header_struct header={
		.magic=0xB00170AD,
		.imageSize=0,
		.crc=0,
		.confirmationFlag=1,
		.rejectionFlag=0,
		.OTAflag=0,
		.version=1
};
