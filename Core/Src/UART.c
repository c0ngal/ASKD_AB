//========================================================
//       КОММУНИКАЦИОННЫЕ ПРОЦЕДУРЫ  (порт COM2)          
//========================================================
#include "UART.h"
//#include "global.h"
#include "stm32f4xx_hal.h"                  // Device header
#include <stdarg.h>
#include <stdint.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

static UART_HandleTypeDef *UART_ACTIVE = NULL;

extern uint8_t F_DEBUG;
extern const uint8_t AdrRY;

void uart_use1(void) { UART_ACTIVE = &huart1; }
void uart_use6(void) { UART_ACTIVE = &huart6; }

void sendDebug(const char *fmt, ...)
{
    if (!F_DEBUG) return;
    if (!fmt) return;

    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {
        SendByte(AdrRY, (uint8_t)*fmt++);
    }

    va_end(ap);
}

/*Передача байта*/
 void SendByte(unsigned char uartNumber, uint8_t sendByte){/*sendByte- передаваемый байт, uartNumber - номер uart (1 или 2)*/
   if (uartNumber <= 1){		 
		 HAL_UART_Transmit(&huart6,&sendByte,1,1000);
   }
   else{ //uartNumber != 1
		 HAL_UART_Transmit(&huart6/*NB!!!*/,&sendByte,1,1000);
   }
   //delay_ms(1);/*задержка приблизительно 1 mS*/
} 
 
//========================================================
//Прием байта
//Кч- принимаемый байт, uartNumber - номер uart (1 или 2)
//возвращаемое значение - принимаемый байт, uartNumber - номер uart (1 или 2)
uint8_t GetByte(unsigned char uartNumber){
	uint8_t Rx = 0;
   if (uartNumber <= 1){		 
		 HAL_UART_Receive(&huart6,&Rx,1,1000);
   }
   else{ //uartNumber != 1
		 HAL_UART_Receive(&huart6,&Rx,1,1000);
   }
	return Rx;
}

//Прием байта c timeout-ом
//Rx - принимаемый байт, uartNumber - номер uart (1 или 2)
//возвращаемое значение - принимаемый байт, uartNumber - номер uart (1 или 2)
// timeout - timeout c мс
uint8_t GetByteTimeout(unsigned char uartNumber, uint32_t timeout){
	uint8_t Rx = 0;
	HAL_StatusTypeDef status;
	//status = HAL_UART_Receive(&huart6,&Rx,1,timeout);
   if (uartNumber <= 1){		 
		 status = HAL_UART_Receive(&huart1,&Rx,1,timeout);
   }
   else{ //uartNumber != 1
		 status = HAL_UART_Receive(&huart6 /*NB!!!&huart2*/,&Rx,1,timeout);
   }
/*	if ((status != HAL_OK)&&(status != HAL_TIMEOUT)){
		status = HAL_UART_Receive(&huart6,&Rx,1,timeout);
	}*/
	return Rx;
}

_uart_status uart_receive(uint8_t *data, uint16_t length)
{
  _uart_status status = UART_ERROR;

  if (HAL_OK == HAL_UART_Receive(&huart6, data, length, UART_TIMEOUT))
  {
    status = UART_OK;
  }

  return status;
}

/**
 * @brief   Transmits a string to UART.
 * @param   *data: Array of the data.
 * @return  status: Report about the success of the transmission.
 */
_uart_status uart_transmit_str(uint8_t *data)
{
  _uart_status status = UART_ERROR;
  uint16_t length = 0u;

  /* Calculate the length. */
  while ('\0' != data[length])
  {
    length++;
  }

  if (HAL_OK == HAL_UART_Transmit(&huart6, data, length, UART_TIMEOUT))
  {
    status = UART_OK;
  }

  return status;
}

/**
 * @brief   Transmits a single char to UART.
 * @param   *data: The char.
 * @return  status: Report about the success of the transmission.
 */
_uart_status uart_transmit_ch(uint8_t data)
{
  _uart_status status = UART_ERROR;

  /* Make available the UART module. */
  if (HAL_DMA_STATE_TIMEOUT == HAL_UART_GetState(&huart6)) //TODO проверить правильно ли выбрано HAL_DMA_STATE_TIMEOUT
  {
    HAL_UART_Abort(&huart6);
  }

  if (HAL_OK == HAL_UART_Transmit(&UART_ACTIVE, &data, 1u, UART_TIMEOUT))
  {
    status = UART_OK;
  }
  return status;
}

_uart_status uart_transmit(uint8_t *data, uint16_t size)
{
  _uart_status status = UART_ERROR;


  if (HAL_OK == HAL_UART_Transmit(&huart6, data, size, UART_TIMEOUT))
  {
    status = UART_OK;
  }

  return status;
}

//функция посылки отладочной информации по UART
/*uint8_t sendDebug(char* mes){
	int i=0;
	if (F_DEBUG){
		for (i=0;mes[i]!=0;i++)	
		SendByte(AdrRY,mes[i]);
	}
	return 0;
}*/
