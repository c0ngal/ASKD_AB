#include "UNIO.h"
#include "stm32f4xx_hal.h"                  // Device header
#include "main.h"

extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;
//задержки
//const uint16_t delayToClk = 200;
//const uint16_t delaySPIStart = 20;	//количество милисекунд от посылки стартового байта до следующего
const uint16_t delaySPIBetweenByte = 50;	//количество микросекунд  между посылкой номера канала и чтением байта ответа (для запроса состояние канала платы ввода)
//коды возможных байт ответа
#define NAK 0x15
#define ERR_NUM 0x0A	//неправильный номер (АЦП, ЦАП или UNIO)
#define ERR_CMD 0x0F	//неправильная/неизвестная команда (АЦП, ЦАП или UNIO)
//типы команд по SPI
const uint8_t GET_UNIO_CMD = 0x01;	//код команды запроса UNIO
const uint8_t GET_ADC_CMD = 0x02;	//код команды запроса АЦП
const uint8_t SET_UNIO_CMD = 0x03;	//код команды установки UNIO
const uint8_t SET_DAC_CMD = 0x04;	//код команды установки ЦАП
const uint8_t SET_PARAM_CMD = 0x05;	//код команды установки параметров (служебная TODO)
//адреса периферийных плат
const uint8_t DIGITAL_IN_ID = 0;	//ID периферийной платы вводы
const uint8_t DIGITAL_OUT_ID = 1;	//ID периферийной платы выводы

int8_t sostOuts[32] = {0};//сохранение действий над выводами платы, чтобы знать состояние

int8_t getSaveDigitalOut(uint8_t num){return sostOuts[num];};

/*void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000) * us;
    while ((DWT->CYCCNT - start) < ticks);
}*/

static uint32_t dwt_ticks_per_us;

void delay_init(void){
    DWT_Delay_Init();
    dwt_ticks_per_us = SystemCoreClock / 1000000U; // или HAL_RCC_GetHCLKFreq()
}

void delay_us(uint32_t us){
#ifndef NDEBUG
    // В режиме DEBUG не зацикливаемся: грубо округлим до миллисекунд
    if (us >= 1000U) { HAL_Delay(us/1000U); }
    else {
        // краткий busy-wait, если не будешь ставить брейкпоинт
        uint32_t start = DWT->CYCCNT;
        uint32_t ticks = us * dwt_ticks_per_us;
        while ((uint32_t)(DWT->CYCCNT - start) < ticks) { __NOP(); }
    }
#else
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * dwt_ticks_per_us;
    while ((uint32_t)(DWT->CYCCNT - start) < ticks) { __NOP(); }
#endif
}





uint8_t UNIO_NUM_TO_PORT[48] = {2,4,6,7,5,3,1,23,15,13,9,11,8,10,
12,14,20,21,22,0,19,18,17,16,26,28,30,31,29,27,25,47,
39,37,33,35,32,34,36,38,44,45,46,24,43,42,41,40};

uint16_t NUM_TO_GPIO_PIN[16] = {GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,GPIO_PIN_5,
	                             GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,
                               GPIO_PIN_11,GPIO_PIN_12,GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15};

int8_t setUNIO(uint8_t num,uint8_t val){
	uint8_t port = UNIO_NUM_TO_PORT[num];
	if (num > 47) return -1;
	if (port >= 32){	//portF
		port -= 32;
		if (val){
			HAL_GPIO_WritePin(GPIOF,NUM_TO_GPIO_PIN[port],GPIO_PIN_SET);// PORTF->SETTX = (1 << port);
		}
		else
			HAL_GPIO_WritePin(GPIOF,NUM_TO_GPIO_PIN[port],GPIO_PIN_RESET);//PORTF->CLRTX = (1 << port);
	}
	else if (port >= 16){	//portE
		port -=16;
		if (val){
			HAL_GPIO_WritePin(GPIOE,NUM_TO_GPIO_PIN[port],GPIO_PIN_SET);//PORTE->RXTX |= (1 << port);
		}
		else
			HAL_GPIO_WritePin(GPIOE,NUM_TO_GPIO_PIN[port],GPIO_PIN_RESET);//PORTE->RXTX &= ~(1 << port);
	}
	else{	//portG
		if (val){
			HAL_GPIO_WritePin(GPIOG,NUM_TO_GPIO_PIN[port],GPIO_PIN_SET);//PORTB->RXTX |= (1 << port);
		}
		else
			HAL_GPIO_WritePin(GPIOG,NUM_TO_GPIO_PIN[port],GPIO_PIN_RESET);//PORTB->RXTX &= ~(1 << port);
	}
	return 0;
}

int8_t setUNIOByte(uint8_t num,uint8_t val){
	uint8_t i=0;
	if (num >= 6) return -1;	//номер байта от 0 до 5
	for(i=0; i<8; i++)	{
		setUNIO(num*8 + i, val & (1 << i));
	}
	return 0;
}

int8_t getUNIO(uint8_t num){
	uint8_t port = UNIO_NUM_TO_PORT[num];
	uint8_t value = 0;
/*	if (num == CtrlNumUNIO) {
		PORTA->SETTX = 2; //Установка направления 5572 2DIR3 от A к B
		PORTF->OE |= 1 << 7;
		PORTF->PWR |= 3 << 14;
		PORTF->SETTX = 1 << 7;
		delay_5us(10);
		if (PORTF->RXTX & (1 << (port-32)))
			value = 1;
		PORTA->CLRTX = 2; //Установка направления 5572 2DIR3 обратно от B к A
		PORTF->OE = 0xFF00;
		PORTF->PWR = 0xFFFF0000;
		PORTF->CLRTX = 1 << 7;
	}*/

	if (num > 47) return -1;
	if (port >= 32){	//portF
		port -= 32;
		if (HAL_GPIO_ReadPin(GPIOF,NUM_TO_GPIO_PIN[port]) == GPIO_PIN_SET)//if (PORTF->RXTX & (1 << port))
			value = 1;
	}
	else if (port >= 16){	//portE
		port -=16;
		if (HAL_GPIO_ReadPin(GPIOE,NUM_TO_GPIO_PIN[port]) == GPIO_PIN_SET)//if (PORTE->RXTX & (1 << port))
			value = 1;
	}
	else{	//portG
		if (HAL_GPIO_ReadPin(GPIOG,NUM_TO_GPIO_PIN[port]) == GPIO_PIN_SET)//if (PORTB->RXTX & (1 << port))
			value = 1;
	}

	return value;
}



/*int setUNIO_SPI(uint8_t num,uint8_t value){
	uint16_t val[32] = {0};
	int i=0;
		//SPI одно на двоих (память и в/д с МК) => перед посылкой переопределяем параметры скорости и чип селекта
	RST_CLK->SSP_CLOCK |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN | 0x0002;
	PORTC->FUNC |= (2 << PORT_FUNC_MODE8_OFFS);	//CS включаем
	PORTD->FUNC &= 0xFFFFFF3F; //portD3 - альтернативная для Rx (при работе с МК отключаем этот вход)

		SSP1->DR = 0xF0;	//байт начала сообщения
		val[0] = SSP1->DR;
		delay_ms(delaySPIStart);
	  SSP1->DR = SET_UNIO_CMD;	//команда установки UNIO
		val[1] = SSP1->DR;
		delay_ms(delaySPIBetweenByte);
		SSP1->DR = num;	//байт номера
		val[2] = SSP1->DR;
		delay_ms(delaySPIBetweenByte);
		SSP1->DR = value;	//байт номера
		val[3] = SSP1->DR;
		while (val[3+i] != 0x06){
			if (val[3+i] == 0x15){
				delay_ms(delaySPIBetweenByte);
				SSP1->DR = 0x00;	//байт номера
				return -1;
			}
  		delay_ms(delaySPIBetweenByte);
			SSP1->DR = 0x00;	//байт номера
			val[4+i] = SSP1->DR;
			i++;
			if (i>16) return -1;
		}
	return 0;
}

int getUNIO_SPI(uint8_t num){
	uint16_t val[32] = {0};
	uint8_t value1 = 0;
	int i=0;
		//SPI одно на двоих (память и в/д с МК) => перед посылкой переопределяем параметры скорости и чип селекта
	RST_CLK->SSP_CLOCK |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN | 0x0002;
	PORTC->FUNC |= (2 << PORT_FUNC_MODE8_OFFS);	//CS включаем
	PORTD->FUNC &= 0xFFFFFF3F; //portD3 - альтернативная для Rx (при работе с МК отключаем этот вход)

		SSP1->DR = 0xF0;	//байт начала сообщения
		val[0] = SSP1->DR;
		delay_ms(delaySPIStart);
	  SSP1->DR = GET_UNIO_CMD;	//команда установки UNIO
		val[1] = SSP1->DR;
		delay_ms(delaySPIBetweenByte);
		SSP1->DR = num;	//байт номера
		val[2] = SSP1->DR;
		while (val[2+i] != 0x06 || !(value1)){
			if (val[2+i] == 0x15){
				delay_ms(delaySPIBetweenByte);
				SSP1->DR = 0x00;	//байт номера
				return -1;
			}
  		delay_ms(delaySPIBetweenByte);
			SSP1->DR = 0x00;	//байт номера
			if ((val[2+i] & 0xA0) == 0xA0) value1 = val[2+i];
			val[3+i] = SSP1->DR;
			i++;
			if (i>16) return -1;
		}
		value1 = value1 & 0x0F;
	return value1;
}*/

/*int getADC_SPI(uint8_t num){
	uint16_t val[45] = {0};
	uint8_t value1 = 0, value2 = 0,value3 = 0;
	int i=0;
	//SPI одно на двоих (память и в/д с МК) => перед посылкой переопределяем параметры скорости и чип селекта
	RST_CLK->SSP_CLOCK |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN | 0x0002;
	PORTC->FUNC |= (2 << PORT_FUNC_MODE8_OFFS);	//CS включаем
	PORTD->FUNC &= 0xFFFFFF3F; //portD3 - альтернативная для Rx (при работе с МК отключаем этот вход)
//!!!!!!!!!
	num +=2; //номер АЦП в Slave МК начинается с 2-ого (первые 2 под референсным уровнем)
	//!!!!!!!!!
		SSP1->DR = 0xF0;	//байт начала сообщения
  val[0] = SSP1->DR;	//буфер приемника не пуст
		delay_ms(1);//(delaySPIStart);
	SSP1->DR = GET_ADC_CMD;	//байт кода команды чтения с АЦП
		val[1] = SSP1->DR;
		delay_ms(delaySPIBetweenByte);
		SSP1->DR = num;	//байт номера АЦП
		val[2] = SSP1->DR;
	while (val[2+i] != 0x06 || !(value1 && value2 && value3)){	//ждем когда придет байт успешного окончания (и проверяем, что пришли все байты с значением АЦП)
			if (val[2+i] == 0x15){
				delay_ms(delaySPIBetweenByte);
				SSP1->DR = 0x00;	//чтобы отправился байт кода ошибки (TODO обработку)
				return -1;
			}
  		delay_ms(delaySPIBetweenByte);
			if (val[2+i] & 0x20) value1 = val[2+i];
			if (val[2+i] & 0x40) value2 = val[2+i];
			if (val[2+i] & 0x80) value3 = val[2+i];
			SSP1->DR = 0x00;	//байт номера
			val[3+i] = SSP1->DR;
			i++;
			if (i>16)
				return -1;
		}
		if (value1 && value2 && value3){
			value1 = value1 & 0x0F; value2 = value2 & 0x0F; value3 = value3 & 0x0F; 	//хранят только полбайта значащей информации, остальное - код части в общем значении
			return ((value1 << 8) | (value2 << 4) | value3);
		}
		else
			return -1;

}*/


uint16_t getADC_SPI(uint8_t num){
	uint8_t Rx = 0;
	uint8_t tmpVal = 0;
	uint16_t resVal = 0;
	uint8_t Cmd[2] = {0xF0,DIGITAL_IN_ID};

	//HAL_GPIO_WritePin(FSS_MK_GPIO_Port,FSS_MK_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,Cmd,1,1000);//байт начала сообщения
//	HAL_SPI_Transmit(&hspi3,&Cmd[1],1,1000);//номер платы
	HAL_SPI_Transmit(&hspi2,&num,1,1000);//номер ввода
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
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);

	if (Rx != 0x06)
		return -1;

	resVal = resVal | tmpVal;
	return resVal;

}

int8_t getDigitalInSPI(uint8_t num){
	uint8_t Rx = 0;
	uint8_t val = 0;
	uint8_t Cmd[2] = {0xF0,DIGITAL_IN_ID};

	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3,Cmd,1,1000);//байт начала сообщения
	HAL_SPI_Transmit(&hspi3,&Cmd[1],1,1000);//номер платы
	HAL_SPI_Transmit(&hspi3,&num,1,1000);//номер ввода
	//delay_us(delaySPIBetweenByte);// HAL_Delay(1);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi3,&num,&val,1,1000);//значение	ввода
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi3,&num,&Rx,1,1000);//квитанция
	HAL_Delay(1);

	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_SET);

	if (Rx != 0x06 || (val & 0xC0) != 0xC0)
		return -1;

	val = val & 0x0F;
	return val;
}

/*int8_t getDigitalInSPI(uint8_t num){
	uint8_t Rx = 0;
	uint8_t val = 0;
	uint8_t Cmd[2] = {0xF0,DIGITAL_IN_ID};

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,Cmd,1,1000);//байт начала сообщения
	HAL_SPI_Transmit(&hspi2,&Cmd[1],1,1000);//номер платы
	HAL_SPI_Transmit(&hspi2,&num,1,1000);//номер ввода
	//delay_us(delaySPIBetweenByte);// HAL_Delay(1);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi2,&num,&val,1,1000);//значение	ввода
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi2,&num,&Rx,1,1000);//квитанция
	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	if (Rx != 0x06 || (val & 0xC0) != 0xC0)
		return -1;

	val = val & 0x0F;
	return val;
}*/

uint16_t getDigitalInWordSPI(uint8_t num){
	uint8_t Rx = 0;
	uint8_t val1,val2;
	uint16_t res = 0;
	uint8_t Cmd[4] = {0xF0,DIGITAL_IN_ID,0xA1,0xA2};

	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3,Cmd,1,1000);//байт начала сообщения
	HAL_SPI_Transmit(&hspi3,&Cmd[1],1,1000);//номер платы
	if (num < 16)
		HAL_SPI_Transmit(&hspi3,&Cmd[2],1,1000);//номер ввода
	else
		HAL_SPI_Transmit(&hspi3,&Cmd[3],1,1000);//номер ввода
	//HAL_Delay(1);
	delay_us(delaySPIBetweenByte);// HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi3,&num,&val1,1,1000);//значение	ввода
	//HAL_Delay(1);
	delay_us(delaySPIBetweenByte);// HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi3,&num,&val2,1,1000);//значение	ввода
	HAL_SPI_TransmitReceive(&hspi3,&num,&Rx,1,1000);//квитанция

	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_SET);

	if (Rx != 0x06)
		return -1;

	res = val1 + (val2 << 8);
	return res;
}

int8_t setDigitalOutSPI(uint8_t num, uint8_t val){
	uint8_t Rx = 0;
	uint8_t Cmd[2] = {0xF0,DIGITAL_OUT_ID};

	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3,Cmd,1,1000);//байт начала сообщения
	HAL_SPI_Transmit(&hspi3,&Cmd[1],1,1000);//номер платы
	HAL_SPI_Transmit(&hspi3,&num,1,1000);//номер ввода
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi3,&val,&Rx,1,1000);//значение
	HAL_Delay(1);
	HAL_GPIO_WritePin(FSS_ISA_GPIO_Port,FSS_ISA_Pin,GPIO_PIN_SET);

	if (Rx != 0x06){
		  Rx = 0;
			return -1;
	}

	sostOuts[num] = val;
	return 0;
}

/*int8_t setDigitalOutSPI(uint8_t num, uint8_t val){
    uint8_t Rx=0, dummy=0xFF;
    uint8_t Cmd[2]={0xF0, DIGITAL_OUT_ID};

    HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_RESET); // CS↓

    HAL_SPI_Transmit(&hspi3, &Cmd[0], 1, 1000);
    HAL_SPI_Transmit(&hspi3, &Cmd[1], 1, 1000);
    HAL_SPI_Transmit(&hspi3, &num,    1, 1000);


    HAL_Delay(1);

    // отправляем val, что пришло здесь — часто мусор/статус
    HAL_SPI_TransmitReceive(&hspi3, &val,   &Rx, 1, 1000);
    // читаем ACK на следующем байте
    HAL_SPI_TransmitReceive(&hspi3, &dummy, &Rx, 1, 1000);

    HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_SET);   // CS↑

    return (Rx == 0x06) ? 0 : -1;
}*/





/*int8_t setDigitalOutSPI(uint8_t num, uint8_t val){
    uint8_t rx = 0, dummy = 0xFF;
    uint8_t Cmd[2] = {0xF0, DIGITAL_OUT_ID};

    HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_RESET);  // CS ↓

    HAL_SPI_Transmit(&hspi3, Cmd, 1, 1000);      // 0xF0
    HAL_SPI_Transmit(&hspi3, &Cmd[1], 1, 1000);  // DIGITAL_OUT_ID
    HAL_SPI_Transmit(&hspi3, &num, 1, 1000);     // номер канала

    // ВАЖНО: пауза, как в чтении
    //delay_us(delaySPIBetweenByte);               // например 50 us


    // 1) Отправляем значение, ответ игнорируем
    HAL_SPI_TransmitReceive(&hspi3, &val, &rx, 1, 1000);

    // 2) Отдельным байтом читаем ACK
    HAL_SPI_TransmitReceive(&hspi3, &dummy, &rx, 1, 1000);

    HAL_GPIO_WritePin(FSS_ISA_GPIO_Port, FSS_ISA_Pin, GPIO_PIN_SET);    // CS ↑

    if (rx != 0x06) {
        // временный лог, чтобы понять что реально пришло
        // printf("ACK=0x%02X (ожидали 0x06)\n", rx);
        return -1;
    }
    sostOuts[num] = val;
    return 0;
}*/

