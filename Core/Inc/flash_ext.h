#ifndef FLASH_EXT_H
#define FLASH_EXT_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stddef.h>

// Размеры
#define FLASH_PAGE_SIZE    256u      // размер страницы для PAGE PROGRAM
#define FLASH_SECTOR_SIZE  0x1000u   // 4 КБ сектор

// Таймаут по умолчанию (можешь подправить, если надо)
#define FLASH_TIMEOUT_MS   100u

// Команды флешки
#define CMD_READ_DATA      0x03
#define CMD_PAGE_PROGRAM   0x02
#define CMD_SECTOR_ERASE   0xD8      // крупный сектор (для 4К был бы 0x20)
#define CMD_WRITE_ENABLE   0x06
#define CMD_READ_STATUS1   0x05
#define CMD_READ_ID        0x9F

// Бит занятости в статусе
#define SR_WIP             0x01

// Инициализация драйвера (передаёшь сюда свой hspi и CS)
void flash_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

// Ожидание окончания операции (стирание/запись)
HAL_StatusTypeDef flash_wait_busy(uint32_t timeout_ms);

// Чтение / стирание / запись
HAL_StatusTypeDef flash_read(uint32_t addr, uint8_t *buf, size_t len);
HAL_StatusTypeDef flash_erase_sector(uint32_t sector_addr);
HAL_StatusTypeDef flash_write(uint32_t addr, const uint8_t *buf, size_t len);

// Чтение JEDEC ID (0x9F)
uint32_t flash_read_id(void);

#endif // FLASH_EXT_H
