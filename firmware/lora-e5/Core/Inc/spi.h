#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32wlxx.h"
#include <ctype.h>
#include <stdbool.h>

/* W25Q Chip Standard SPI instruction set */
#define WRITE_ENABLE 			0x06U
#define VOLATILE_WRITE_ENABLE 		0x50U
#define WRITE_DISABLE 			0x04U
#define READ_STATUS_REG_1 		0x05U
#define READ_STATUS_REG_2		0x35U
#define WRITE_STATUS_REG		0x01U
#define PAGE_PROGRAM			0x02U
#define SECTOR_ERASE_4KB		0x20U
#define BLOCK_ERASE_32KB		0x52U
#define BLOCK_ERASE_64KB		0xD8U
#define CHIP_ERASE			0x60U
#define PROGRAM_SUSPEND			0x75U
#define PROGRAM_RESUME			0x7AU
#define POWER_DOWN			0xB9U
#define READ_DATA			0x03U
#define FAST_READ			0x0BU
#define RELEASE_POWER_DOWN		0xABU
#define MFG_DEVICE_ID			0x90U
#define JEDEC_ID			0x9FU
#define UNIQUE_ID			0x4BU
#define READ_SFDP_REG			0x5AU
#define ERASE_SECURITY_REG		0x44U
#define PROGRAM_SECURITY_REG		0x42U
#define READ_SECURITY_REG		0x48U
#define W25Q_ENABLE_RST			0x66U
#define W25Q_RST			0x99U

/* W25Q80DV/DL (8M-bit) External Serial Flash Config */
#define W25Q_FLASH_SIZE 		8U		// 8 M-Bit
#define W25Q_PAGE_SIZE			256U		// 1 page	: 256 bytes
#define W25Q_SECTOR_SIZE		4U		// 4 KB		: 16 pages
#define W25Q_SBLOCK_SIZE		32U		// 32 KB	: 128 pages
#define W25Q_BBLOCK_SIZE		64U		// 64 KB 	: 256 pages
#define W25Q_TOTAL_PAGES		4096U		// Total memory : 4096*256 bytes
#define W25Q_TOTAL_SECTORS		256U		// 4096/16	
#define W25Q_TOTAL_BLOCKS		16U		// 256/16

/* W25Q80DV/DL IDs */
#define W25Q_MFG_ID 			0xEFU
#define W25Q_DEVICE_ID 			0x13U


/* W25Q address ranges */
#define W25Q_ADD_BEGIN			0x000000U
#define W25Q_ADD_END			0x0FFFFFU


typedef enum{
	W25Q_OK = 0,
	W25Q_BUSY,
	W25Q_ERROR,
}W25Q_status;

typedef struct W25Q_status_reg {
	bool busy;
	bool write_enable_latch; 	// 1 - write enabled
	bool quad_mode;
	bool suspend_mode;
	bool curr_addr_mode;
	bool powerup_addr_mode;
	bool sleep;
}W25Q_status_reg;

bool W25Q_verify_mfg_chipID(void);
bool W25Q_read_uinque_chipID(uint8_t *buff);
bool RUN_W25Q_test_procedure(uint8_t* buff, double* pct);	// returns true on success, buff contains UID
W25Q_status W25Q_read_SFDP_register(uint8_t *buff);
bool W25Q_calc_flash_pct(double* pct);

extern SPI_HandleTypeDef hspi2;

void MX_SPI2_Init(void);	

/* W25Q functions */


#ifdef __cplusplus
}
#endif

#endif
