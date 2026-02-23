#ifndef FLASH_DEF
#define FLASH_DEF

#include "main.h"

HAL_StatusTypeDef flash_write(uint32_t addr, uint32_t *data, uint32_t size);

#endif
