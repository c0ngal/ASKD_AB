#ifndef __UART_H
#define __UART_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

// таймаут по умолчанию (в мс)
#ifndef UART_TIMEOUT
#define UART_TIMEOUT 1000u
#endif

void uart_use1(void);
void uart_use6(void);

// статусные коды UART
typedef enum
{
    UART_OK = 0,
    UART_ERROR
} _uart_status;

// базовые функции
void     SendByte(unsigned char uartNumber, uint8_t sendByte);
uint8_t  GetByte(unsigned char uartNumber);
uint8_t  GetByteTimeout(unsigned char uartNumber, uint32_t timeout);

// расширенные функции
_uart_status uart_receive(uint8_t *data, uint16_t length);
_uart_status uart_transmit_str(uint8_t *data);
_uart_status uart_transmit_ch(uint8_t data);
_uart_status uart_transmit(uint8_t *data, uint16_t size);

#endif
