#ifndef OTA_DEF
#define OTA_DEF

#include "main.h"
#include "flash_write.h"
#include <stdio.h>

#define APP1_HDR_START 0x08008000U

void clearOTAflag();
void OTA_update();

#endif
