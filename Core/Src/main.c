/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UART.h"
#include "UNIO.h"
#include <stdio.h>
#include <stdarg.h>
#include "Flash_SSt25.h"
#include "delay.h"
#include <stdbool.h>
#include "mainProgram.h"
#include "ServiceFunctions.h"
#include "stm32f4xx_hal.h"
#include "flash_ext.h"
#include "MemoryFunctions.h"

//extern struct tLCTableFile;
//extern void Chip_Erase(void);

//#include "mem_selftest.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CS_L()  HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET)
#define CS_H() HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET)
#define FLASH_CS_Port  GPIOA
#define FLASH_CS_Pin   GPIO_PIN_4



#ifndef CANAL_FILE_ADDR
#define CANAL_FILE_ADDR  canalFileAdr
#endif

#define CANAL_NRECS        100
#define CANAL_STRIDE       8            // 7 + 1
#define CANAL_TOTAL_BYTES  (CANAL_NRECS * CANAL_STRIDE)   // 800
extern void ReadData(uint32_t addr, void* dst, uint32_t len);


#define RX_BUF_SZ 1024
volatile uint8_t  rxbuf[RX_BUF_SZ];
volatile uint16_t rxlen = 0;

#define TEK_LEN 766
extern void SendByte(unsigned char adr, uint8_t b);
extern const uint16_t AdrRY;

#define KT  0x03
extern uint8_t ZnUTekI[TEK_LEN];


uint8_t cmdStart   = 0xF0;
uint8_t channel    = 0x06;
uint8_t dummy      = 0x00;
uint8_t hi, lo, statusByte;
uint16_t adcValue;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
//extern RTC_HandleTypeDef hrtc;
extern int8_t setUNIO(uint8_t num,uint8_t val);
#define DIG_OUT_TEST_CH 0

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
struct tLCTableFile FileLCTable;


volatile uint8_t rx_ring[256];
volatile uint16_t wr = 0;

uint8_t Rx = 0x01;
uint8_t tmpVal;
uint16_t resVal = 0;
uint8_t cmd[2] = {0xF0, 0};
uint8_t num = 0;

uint8_t tx = 0xAA;
uint8_t rx = 0;

#define FLASH_CS_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define FLASH_CS_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

uint8_t Flash_ReadStatus(void) {
    uint8_t cmd = 0x05; // RDSR
    uint8_t status = 0;

    FLASH_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    HAL_SPI_Receive(&hspi1, &status, 1, 100);
    FLASH_CS_HIGH();

    return status;
}
void ByteProgram(int adr, uint8_t data);
void Flash_WaitBusyClear(void) {
    while (Flash_ReadStatus() & 0x01) {
        // бит 0 = WIP (Write In Progress)
    }
}

int Rd_IDexROM(void);

void Chip_Erase(void);

void Flash_WriteEnable(void) {
    uint8_t cmd = 0x06; // WREN
    FLASH_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    FLASH_CS_HIGH();
}

void Flash_Erase4K(uint32_t addr) {
    uint8_t cmd[4];
    cmd[0] = 0x20; // 4KB Sector Erase
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8)  & 0xFF;
    cmd[3] = (addr)       & 0xFF;

    Flash_WriteEnable();

    FLASH_CS_LOW();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    FLASH_CS_HIGH();

    Flash_WaitBusyClear();
}

void Sector32KB_Erase(int adr);
void Sector4KB_Erase(int adr);

void Flash_Read(uint32_t addr, uint8_t *data, uint16_t len);
/*void Flash_PageProgram(uint32_t addr, uint8_t *data, uint16_t len) {
    uint8_t cmd[4];
    cmd[0] = 0x02; // PAGE PROGRAM
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8)  & 0xFF;
    cmd[3] = (addr)       & 0xFF;

    uint32_t adr = addr;
    uint8_t dr;
    for ( ; adr < addr + len; ++adr) {
    	ByteProgram(adr, data[adr]);
    	//dr = data[adr];
    	//Flash_Read(adr, &dr, 1);
    }

}*/


/*void Flash_Read(uint32_t addr, uint8_t *data, uint16_t len) {
	uint8_t CmdDataRead = 0x03;
	uint8_t adrArr[3] = {0};
	uint8_t nullTxForRx[20] = {0};
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,data,len,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}*/

void Flash_Test(void) {
    uint8_t tx[16];
    uint8_t rx[16];
    // 1) Заполняем буфер каким-нибудь понятным паттерном
    for (int i = 0; i < 16; ++i) {
        tx[i] = 0xA0 + i;   // A0 A1 A2 ... AF
        rx[i] = 0;
    }

    // 2) Стираем сектор (важно!)
    //Enable_Write_Status_Reg();
    //Write_Status_Reg(0);
    Sector4KB_Erase(0x000000);
    ByteProgram(0x000000, 1);
    uint8_t dt;
    Flash_Read(0x000000, &dt, 1);

    // 3) Пишем эти 16 байт в начало
    Flash_PageProgram(0x000000, tx, 16);

    // 4) Читаем обратно
    Flash_Read(0x000000, rx, 16);

    int ok = 1;

}


void Flash_ReadID()
{
	FLASH_CS_HIGH();
	HAL_Delay(2);
    uint8_t cmd = 0x05;
    uint8_t rx[3] = {0};

    FLASH_CS_LOW();

    // отправляем команду 0x9F
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);

    // читаем 3 байта ID
    HAL_SPI_Receive(&hspi1, rx, 3, 100);

    FLASH_CS_HIGH();

}








/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_SET);

  Enable_Write_Status_Reg();
  Write_Status_Reg(0);

//while (1) {
  //Flash_ReadID();
//}


  setDigitalOutSPI(0, 1);

  RTekI();
  //izmU();
  //Uakb();

  for (i=0;i<32;i++){	//инициализация платы вывода (чтобы при reset гасли не нужные лампочну, например при выходе из режима ТО)
  	//setDigitalOutSPI(i,1);
  	//setDigitalOutSPI(i,1);
  	setDigitalOutSPI(i,0);
  }


  	setDigitalOutSPI(24,0);
  	setDigitalOutSPI(26,0);
  setUNIO(29,1);
  setUNIO(27,1);



//прочитать память (была уже готовая функция)


EraseRabArx();
WTekI();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //uint8_t rec[255];
	  //for (uint32_t i = 0; i < 255; ++i) ZnArxI[i] = i;
	  //fMemoryArxROverWrite = 0;
	  /*Sector4KB_Erase(0x0);
 	Flash_PageProgram(0x0, ZnArxI, 255);
 	Flash_Read(0x0, rec, 255);*/
	  //Sector4KB_Erase(addr);
 	//HAL_Delay(1000);
    //	  ZapRArx();
	  //ChtRArx();
	  //посмотреть на тот ли адрес записывается TODO
	  //пgоверить флешку через read_id
	  //write void SaveCanal(struct tCanalFile FileCanal)
	  /*int i=0,j=0;
		Sector4KB_Erase(canalFileAdr);
		Sector4KB_Erase(canalFileAdr+0x1000);
		HAL_Delay(30);
		for(i=0;i < 1000;i++){
			for(j=0;j < 7;j++)
				ByteProgram(canalFileAdr + i*7 + j, FileCanal.canals[i].F[j]);
		  ByteProgram(canalFileAdr + i*7 + 7, FileCanal.canals[i].P);*/
	  //read
		/*HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
		HAL_SPI_Transmit(&hspi1,adrArr,3,100);
		HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,(uint8_t*)res.data,20,100);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);*/ //struct tConfFile ReadConfFile(void)

	      // 1) отправляем команду 0xF0
	      /*HAL_SPI_TransmitReceive(&hspi2, &cmdStart, &dummy, 1, HAL_MAX_DELAY);

	      // 2) отправляем номер канала
	      HAL_SPI_TransmitReceive(&hspi2, &channel, &dummy, 1, HAL_MAX_DELAY);

	      // 3) читаем старший байт
	      HAL_SPI_TransmitReceive(&hspi2, &dummy, &hi, 1, HAL_MAX_DELAY);

	      // 4) читаем младший байт
	      HAL_SPI_TransmitReceive(&hspi2, &dummy, &lo, 1, HAL_MAX_DELAY);

	      // 5) читаем статус
	      HAL_SPI_TransmitReceive(&hspi2, &dummy, &statusByte, 1, HAL_MAX_DELAY);

	      // CS вверх
	      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

	      // склеиваем значение АЦП
	      adcValue = ((uint16_t)hi << 8) | lo;

	      // тут проверяешь statusByte == 0xAA и используешь adcValue
	      HAL_Delay(10);*/


	  /*HAL_StatusTypeDef st = HAL_SPI_Receive(&hspi2, &rx, 4, HAL_MAX_DELAY);
	  if (st == HAL_OK)
	  {
		  rx_ring[wr++] = b;
		  wr &= 0xFF;    // кольцевой буфер 0..255
	  }*/

	  // НАРИСУЕМ ВСЕ ДД ВКЛЮЧЕННЫМИ
	      /*for (int r = 1; r < 25; ++r) {
	          // ZnDD[r] = '1';
	          ZnTekI[257 + r] = '1';  // 258..281 — как в DtDv
	      }
	      ZnTekI[282] = '1'; // DD1
	      ZnTekI[283] = '1'; // DD6
	      ZnTekI[352] = 0.5;*/

	  //DtDv();

	  for (int i = 1; i <= 765; ++i) {
		  SendByte(6, ZnTekI[i]);
	  }
	  SendByte(6, KT);









	  //HAL_StatusTypeDef st = HAL_UART_Receive(&huart6, &b, 1, 10);

	  /*if (st == HAL_OK) {
	       // получили байт -> эхо + печать в hex
	       HAL_UART_Transmit(&huart6, &b, 1, HAL_MAX_DELAY);

	       char info[16];
	       int n = snprintf(info, sizeof(info), " [%02X]\r\n", (unsigned)b);
	       HAL_UART_Transmit(&huart6, (uint8_t*)info, n, HAL_MAX_DELAY);
	   }*/
	  /*for (;;) {
	          // Ждём блок: пауза между байтами ≥ 5 мс, общий таймаут ожидания 1000 мс
	          uint16_t n = uart_read_block_idle(&huart6, rxbuf, RX_BUF_SZ, 5, 1000);

	          if (n > 0) {
	        	  RTekI();
	        	  for (int i = 1; i <= 765; ++i) {
	        		  SendByte(6, ZnTekI[i]);
	        	  }
	        	  SendByte(6, KT);
	        	  //send_dummy_answer();

	        	  //for (int i = 1; i <= 765; ++i) {
	        		//  SendByte(AdrRY, (uint8_t)('A')); // или просто 'A'
	        	  //}
	        	      // конец пакета
	        	  SendByte(AdrRY, KT);
	        	  PC();
	        	  send_dummy_answer();


	              // и, например, вывести длину
	              char info[32];
	              //int m = snprintf(info, sizeof(info), "\r\n[bytes=%u]\
	              //HAL_UART_Transmit(&huart6, (uint8_t*)info, m, 50);
	          }
	  }*/


	  //getADC_SPI(0);

	  	//HAL_GPIO_WritePin(FSS_MK_GPIO_Port,FSS_MK_Pin,GPIO_PIN_RESET);
	    /*const char msg[] = "H\r\n";
	    HAL_UART_Transmit(&huart6, (uint8_t*)msg, sizeof(msg) - 1, HAL_MAX_DELAY);
	    HAL_Delay(500);*/




	    /*uint8_t n;
	    HAL_StatusTypeDef st = HAL_UART_Receive(&huart6, &n, 1, HAL_MAX_DELAY);
	    HAL_UART_Receive(&huart6, &n, 1, HAL_MAX_DELAY);
	    if (st == HAL_OK) {
	    	HAL_UART_Transmit(&huart6, &n, 1, HAL_MAX_DELAY);
	    	st = HAL_TIMEOUT;
	    }*/

/*
	  HAL_StatusTypeDef st = HAL_UART_Receive(&huart6, &Rx, 1, HAL_MAX_DELAY);

	  if (st == HAL_OK) {
	    	 HAL_UART_Transmit(&huart6, &Rx, 1, 1000);
	  }
*/

	  mainProgram();
	  //PC();
	  /*HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi2, cmd,1,1000);//байт начала сообщения
	  HAL_SPI_Transmit(&hspi2,&num, 1,1000);//номер ввода
	  //delay_us(delaySPIBetweenByte);// HAL_Delay(1);
	  HAL_Delay(1);
	  HAL_SPI_TransmitReceive(&hspi2,&num,&tmpVal,1,1000);//значение	ввода
	  resVal = tmpVal << 8;
	  HAL_Delay(1);
	  HAL_SPI_TransmitReceive(&hspi2,&num,&tmpVal,1,1000);//значение	ввода
	  HAL_Delay(1);
	  HAL_SPI_TransmitReceive(&hspi2,&num,&Rx,1,1000);//квитанция
	  HAL_Delay(1);
	  	//HAL_GPIO_WritePin(FSS_MK_GPIO_Port,FSS_MK_Pin,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);*/


	  delay_ms(200);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  /* USER CODE BEGIN Check_RTC_BKUP */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
  {
      // Первый запуск / RTC сброшен
      sTime.Hours = 0x13;
      sTime.Minutes = 0x12;
      sTime.Seconds = 0x30;
      sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
      sTime.StoreOperation = RTC_STOREOPERATION_RESET;
      if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) Error_Handler();

      sDate.WeekDay = RTC_WEEKDAY_MONDAY;
      sDate.Month = RTC_MONTH_JANUARY;
      sDate.Date = 0x30;
      sDate.Year = 0x26;
      if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) Error_Handler();

      // Записали "магическое" значение: RTC настроен
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
  }
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  /*sTime.Hours = 0x13;
  sTime.Minutes = 0x12;
  sTime.Seconds = 0x30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x30;
  sDate.Year = 0x26;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }*/
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DIR1_2_Pin|DIR1_3_Pin|DIR2_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|FSS_MK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|DIR1_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DIR2_2_Pin|DIR2_3_Pin|DIR1_4_Pin|DIR2_4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, OE1_Pin|OE2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 PF2 PF3
                           PF4 PF5 PF6 PF7
                           PF8 PF9 PF10 PF11
                           PF12 PF13 PF14 PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : DIR1_2_Pin DIR1_3_Pin DIR2_1_Pin FSS_ISA_Pin */
  GPIO_InitStruct.Pin = DIR1_2_Pin|DIR1_3_Pin|DIR2_1_Pin|FSS_ISA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 DIR2_2_Pin DIR2_3_Pin DIR1_4_Pin
                           DIR2_4_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|DIR2_2_Pin|DIR2_3_Pin|DIR1_4_Pin
                          |DIR2_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB12 DIR1_1_Pin FSS_MK_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12|DIR1_1_Pin|FSS_MK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG0 PG1 PG2 PG3
                           PG4 PG5 PG6 PG7
                           PG8 PG9 PG10 PG11
                           PG12 PG13 PG14 PG15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : OE1_Pin OE2_Pin */
  GPIO_InitStruct.Pin = OE1_Pin|OE2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
