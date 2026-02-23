#ifndef FLASH_DEF
#define FLASH_DEF

#include "main.h"

HAL_StatusTypeDef flash_write_header(uint32_t addr, uint32_t *data, uint32_t size);
HAL_StatusTypeDef flash_app_erase(uint32_t addr);
HAL_StatusTypeDef flash_write_app(uint32_t addr, uint8_t *data, uint32_t size);

#endif
