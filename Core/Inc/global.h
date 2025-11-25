#ifndef __GLOBAL_H
#define __GLOBAL_H

//#include "../inc/opora.h"
#include <stdint.h>                           // Include standard types 
#include <stdio.h>                            // для sprintf()
#include <stdlib.h>                           // для atoi()

//#define F_DEBUG 0
extern uint8_t F_DEBUG;

extern const uint8_t KT; //конец передачи текста

extern const uint16_t AdrRY;//номер com-порта (1 или 2) //0xFF10;/*Адрес Rg управления COM2*/
#define  AdAK    0x00A0;/*Базовый адрес [для J8] Rg управления аналогового коммутатора*/
#define  AdrBA   0x00C0;/*Базовый адрес [для J9] управление выходами при замере Rиз.*/
extern uint8_t ZnX[129];	//формат pascal 128+1
extern uint8_t ZnS[65];   //массив для посылки данных из памяти на ПК по UART, используются  с 1 по  24 байты (первые 25 в си, кроме 1-ого(с индексом 0))//формат pascal 64+1
extern uint8_t ZnTekI[766];/*массив данных для ввода/вывода в byte-текущая инф-я*///формат pascal 765+1

    /*Константы для DIC122 (DI)*/
extern const uint16_t  AdrI;//Базовый адрес для DIC122}
extern const uint16_t  AdrI1;//Адрес входов с "0" по "15" для DIC122}
extern const uint16_t  AdrI2;//Адрес входов с "16" по "31" для DIC122}

   /*Константы для DIC123 (DO)*/
extern const uint16_t  AdrO;//Базовый адрес для DIC123}
extern const uint16_t  AdrO1;//Адрес выходов с "0" по "7" для DIC123}
extern const uint16_t  AdrO2;//Адрес выходов с "8" по "15" для DIC123}
extern const uint16_t  AdrO3;//Адрес выходов с "16" по "23" для DIC123}
extern const uint16_t  AdrO4;//Адрес выходов с "24" по "31" для DIC123}

    /*Константы установки команд с АСКД на САЭ*/

extern const uint8_t  rD;//режим "Д" включен
extern const uint8_t  rTO;//режим "ТО" включен
extern const uint8_t  ORaz;//Отключить РАЗРЯД
extern const uint8_t  VZar;//Включить ЗАРЯД
extern const uint8_t  VRaz;//Включить РАЗРЯД
extern const uint8_t  VPZ;//Включить ПОДЗАРЯД
extern const uint8_t  VDR;//Включить ДОРАЗРЯД
extern const uint8_t  DT;//НЕ НОРМА ДТЭ
extern const uint8_t  PR;//НЕ НОРМА Пр.
extern const uint8_t  DDv;//НЕ НОРМА ДД
extern const uint8_t  Cr;//НЕ НОРМА С разр.


void delay_5us(uint32_t mcsec); //задержка в mcsec * 5 мкс
void delay_ms(uint32_t msec);		//задержка msc мс

extern uint8_t fMemoryArxIOverWrite;	//признак того, что память для архива испытаний закончилась и мы начинаем перезаписывать сначала (со 2ого сектора)
extern uint8_t fMemoryArxROverWrite;	//признак того, что память для рабочего архива закончилась и мы начинаем перезаписывать сначала (со 5ого сектора)
extern uint16_t adrArxI;//адрес - номер записи, где храниться последняя запись с архивной информацией из архива испытаний (со 2ого сектора по 4ый)
extern uint16_t adrArxR;//адрес - номер записи, где храниться последняя запись с архивной информацией из рабочего архива (с 5ого сектора по 8й)


#endif // __GLOBAL_H
