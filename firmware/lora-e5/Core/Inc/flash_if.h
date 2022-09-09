#ifndef __FLASH_IF_H__
#define __FLASH_IF_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "platform.h"


enum
{
  FLASH_PARAM_ERROR = -5,
  FLASH_LOCK_ERROR = -4,
  FLASH_WRITE_ERROR = -3,
  FLASH_ERASE_ERROR  = -2,
  FLASH_ERROR  = -1,
  FLASH_OK     = 0,
  FLASH_BUSY   = 1
};

#define PAGE(__ADDRESS__) (uint32_t)((((__ADDRESS__) - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE)

int32_t FLASH_IF_Write(uint32_t  address, uint8_t *data, uint32_t size, uint8_t *dataTempPage);
int32_t FLASH_IF_Write64(uint32_t address, uint64_t data);
int32_t FLASH_IF_EraseByPages(uint32_t page, uint16_t n, int32_t interrupt);
void HWCB_FLASH_EndOfCleanup(void);


#ifdef __cplusplus
}
#endif

#endif
