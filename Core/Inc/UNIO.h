#ifndef __UNIO_H
#define __UNIO_H
#include "stm32f4xx_hal.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1; // шина к slave#1 (UNIO/ADC)
extern SPI_HandleTypeDef hspi2; // шина к периферийным платам (digital in/out)

// Chip Select пины (GPIO Output, Active Low)
#define SPI1_CS_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_8

#define SPI2_CS_GPIO_Port GPIOC
#define SPI2_CS_Pin GPIO_PIN_12

// Протокол
#define START_BYTE 	0xF0
#define ACK_BYTE 	0x06
#define NAK_BYTE 	0x15
#define ERR_NUM 	0x0A
#define ERR_CMD 	0x0F

extern const uint8_t  DIGITAL_IN_ID;
extern const uint16_t delaySPIBetweenByte;


// Команды
/*#define GET_UNIO_CMD 	0x01
#define GET_ADC_CMD 	0x02
#define SET_UNIO_CMD	0x03
#define SET_DAC_CMD 	0x04
#define SET_PARAM_CMD 	0x05

#define DIGITAL_IN_ID 	0x00
#define DIGITAL_OUT_ID 	0x01*/

// Тайминги из ASKD_AB
#define DELAY_SPI_START_MS 			20
#define DELAY_SPI_BETWEEN_BYTE_MS 	1
#define SPI_POLL_TIMEOUT_MS 		50 // Общий timeout ожидания ответа

// Публичные функции
int setUNIO_SPI(uint8_t num, uint8_t value);
int getUINO_SPI(uint8_t num);
uint16_t getADC_SPI(uint8_t num);
int8_t getDigitalInSPI(uint8_t num);
int8_t setDigitalOutSPI(uint8_t num, uint8_t value);

void delay_us(uint32_t us);

#endif
