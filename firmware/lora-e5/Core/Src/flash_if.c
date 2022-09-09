#include "flash_if.h"
#include "stm32_mem.h"

enum { FLASH_EMPTY = 0, FLASH_NOT_EMPTY = 1 };
static int32_t FLASH_IF_Write_Buffer(uint32_t pDestination, uint8_t *pSource,
                                     uint32_t uLength);
static int32_t FLASH_IF_IsEmpty(uint8_t *addr, uint32_t size);


int32_t FLASH_IF_Write(uint32_t address, uint8_t *data, uint32_t size,
                       uint8_t *dataTempPage) {
  int32_t status = FLASH_OK;
  uint32_t page_start_index = PAGE(address);
  uint32_t page_end_index = PAGE(address + size - 1);
  uint32_t curr_size = size;
  uint32_t curr_dest_addr = address;
  uint32_t curr_src_addr = (uint32_t)data;

  if ((data == NULL) || ((size % sizeof(uint64_t)) != 0) ||
      ((address % sizeof(uint64_t)) != 0)) {
    return FLASH_PARAM_ERROR;
  }

  if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
    return FLASH_LOCK_ERROR;
  }

  if (page_start_index != page_end_index) {
    curr_size = FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE);
  }

  for (uint32_t idx = page_start_index; idx <= page_end_index; idx++) {
    if (FLASH_IF_IsEmpty((uint8_t *)curr_dest_addr, curr_size) != FLASH_EMPTY) {
      if (dataTempPage == NULL) {
        return FLASH_PARAM_ERROR;
      }

      UTIL_MEM_cpy_8(dataTempPage,
                     (uint8_t *)(idx * FLASH_PAGE_SIZE + FLASH_BASE),
                     FLASH_PAGE_SIZE);

      UTIL_MEM_cpy_8(
          &dataTempPage[((uint32_t)curr_dest_addr) % FLASH_PAGE_SIZE],
          (uint8_t *)curr_src_addr, curr_size);

      if (FLASH_IF_EraseByPages(idx, 1, 0) != FLASH_OK) {
        status = FLASH_ERASE_ERROR;
        break;
      } else {

        if (FLASH_IF_Write_Buffer(idx * FLASH_PAGE_SIZE + FLASH_BASE,
                                  dataTempPage, FLASH_PAGE_SIZE) != FLASH_OK) {
          status = FLASH_WRITE_ERROR;
          break;
        }
      }
    } else {
      if (FLASH_IF_Write_Buffer(curr_dest_addr, (uint8_t *)curr_src_addr,
                                curr_size) != FLASH_OK) {
        status = FLASH_WRITE_ERROR;
        break;
      }
    }

    curr_dest_addr += curr_size;
    curr_src_addr += curr_size;
    curr_size = size - curr_size;
  }

  return status;
}

int32_t FLASH_IF_Write64(uint32_t address, uint64_t data) {
  while (*(uint64_t *)address != data) {
    while (LL_FLASH_IsActiveFlag_OperationSuspended());
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
  }
  return FLASH_OK;
}

int32_t FLASH_IF_EraseByPages(uint32_t page, uint16_t n, int32_t interrupt) {

  HAL_StatusTypeDef hal_status;
  FLASH_EraseInitTypeDef erase_str;
  uint32_t page_error;

  erase_str.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_str.Page = page;
  erase_str.NbPages = n;

  if (interrupt) {
    hal_status = HAL_FLASHEx_Erase_IT(&erase_str);
  } else {
    hal_status = HAL_FLASHEx_Erase(&erase_str, &page_error);
  }

  return ((hal_status == HAL_OK)
              ? FLASH_OK
              : ((hal_status == HAL_BUSY) ? FLASH_BUSY : FLASH_ERASE_ERROR));
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue) {

  if (ReturnValue == 0xFFFFFFFFUL) {
    HWCB_FLASH_EndOfCleanup();
  }
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue) {}

void HWCB_FLASH_EndOfCleanup(void) {}

static int32_t FLASH_IF_Write_Buffer(uint32_t pDestination, uint8_t *pSource,
                                     uint32_t uLength) {

  uint8_t *pSrc = pSource;
  uint64_t src_value;
  int32_t status = FLASH_OK;

  for (uint32_t i = 0; i < (uLength / sizeof(uint64_t)); i++) {
    UTIL_MEM_cpy_8(&src_value, pSrc, sizeof(uint64_t));

    if (src_value != UINT64_MAX) {
      status = FLASH_IF_Write64(pDestination, src_value);
    }

    pDestination += sizeof(uint64_t);
    pSrc += sizeof(uint64_t);

    if (status != FLASH_OK) {

      break;
    }
  }

  return status;
}

static int32_t FLASH_IF_IsEmpty(uint8_t *addr, uint32_t size) {

  uint64_t *addr64;
  uint32_t i;

  while ((((uint32_t)addr) % sizeof(uint64_t)) != 0) {
    if (*addr++ != UINT8_MAX) {
      return FLASH_NOT_EMPTY;
    }
    size--;
  }

  addr64 = (uint64_t *)addr;
  for (i = 0; i < (size / sizeof(uint64_t)); i++) {
    if (*addr64++ != UINT64_MAX) {
      return FLASH_NOT_EMPTY;
    }
  }
  size -= sizeof(uint64_t) * i;

  addr = (uint8_t *)addr64;
  while (size != 0) {
    if (*addr++ != UINT8_MAX) {
      return FLASH_NOT_EMPTY;
    }
    size--;
  }
  return FLASH_EMPTY;
}
