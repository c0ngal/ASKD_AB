#include "global.h"

const uint16_t AdrRY = 1;//номер com-порта (1 или 2) //0xFF10;/*Адрес Rg управления COM2*/
const uint8_t KT = 0x33; //конец передачи текста
uint8_t F_DEBUG = 0;

/*Константы для DIC122 (DI)*/
const uint16_t  AdrI   = 0x0100;//Базовый адрес для DIC122}
const uint16_t  AdrI1  = 0x0101;//Адрес входов с "0" по "15" для DIC122}
const uint16_t  AdrI2  = 0x0103;//Адрес входов с "16" по "31" для DIC122}

   /*Константы для DIC123 (DO)*/
const uint16_t  AdrO   = 0x0110;//Базовый адрес для DIC123}
const uint16_t  AdrO1  = 0x0110;//Адрес выходов с "0" по "7" для DIC123}
const uint16_t  AdrO2  = 0x0111;//Адрес выходов с "8" по "15" для DIC123}
const uint16_t  AdrO3  = 0x0112;//Адрес выходов с "16" по "23" для DIC123}
const uint16_t  AdrO4  = 0x0113;//Адрес выходов с "24" по "31" для DIC123}

    /*Константы установки команд с АСКД на САЭ*/

const uint8_t  rD     = 0x02;//режим "Д" включен
const uint8_t  rTO    = 0x04;//режим "ТО" включен
const uint8_t  ORaz   = 0x08;//Отключить РАЗРЯД
const uint8_t  VZar   = 0x10;//Включить ЗАРЯД
const uint8_t  VRaz   = 0x20;//Включить РАЗРЯД
const uint8_t  VPZ    = 0x40;//Включить ПОДЗАРЯД
const uint8_t  VDR    = 0x80;//Включить ДОРАЗРЯД
const uint8_t  DT     = 0x88;//НЕ НОРМА ДТЭ
const uint8_t  PR     = 0x90;//НЕ НОРМА Пр.
const uint8_t  DDv    = 0xA0;//НЕ НОРМА ДД
const uint8_t  Cr     = 0xC0;//НЕ НОРМА С разр.


void delay_5us(uint32_t mcsec){
	mcsec *= 24;
	while (mcsec > 0) mcsec--;
}

void delay_ms(uint32_t msec){
	delay_5us(msec*200);
}
