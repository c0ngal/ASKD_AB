#include "ADC.h"
#include "UNIO.h"
#include "UART.h"

extern uint8_t F_DEBUG;
													
//========================================================
// Аналого-Цифровое Преобразование с входа №0 для входов АК с 0-го по 31-й
//  диапазон "0-10" В
float ADC0(){   //TODO
	float res = 0.0f;	//уровень сигнала в вольтах
	int i = 0;	//счетчик кол-ва повторных запросов в случае ошибок
	for (i=0;i<3;i++){
		//res = getADC(0);	//запрос данных с 1-ого АЦП
		res = getADC_SPI(0);
		if (res == -1){
			if (F_DEBUG){
				SendByte(1,'E');SendByte(1,'R');SendByte(1,'R');SendByte(1,'O');SendByte(1,'R');
				SendByte(1,'A');SendByte(1,'D');SendByte(1,'C');SendByte(1,'0');
				SendByte(1,'i');SendByte(1,'=');SendByte(1,(uint8_t)i+'0');
			}
			//ошибка
		}
		else break;	//чтение прошло успешно		
	}
	if (res == -1)
		return -1;
	res -= 2045;//1857;//1820;	//смещение ноля
	res = res * 0.00522;//0.00483;//TODO на макете убрана подтяжка к питанию 0ого канала// res * .00875;//CMRU;/*CMRU = 0.00244 V*/
 	return res;
	//  a:byte;
//     l,h:word;
//   port[AdcY] =  0x0B;/*установка для АЦП ($0B) => вход №0 АЦП,
    //                  усреднение по 16 выборкам, диапазон "0-10"*/
//   delay_ms(2);

//   do{
//   a= port[AdcStat] AND 0x80;/*ждем окончания преобразования*/
//   until(a > 0);

//   delay(2);
//   l= (portw[AdcL] AND 0x00FF);
//   h= (portw[AdcH] AND 0x000F);
//   h= ((h SHL 8) AND 0x0F00);
//   rez= ((l + h) AND 0x0FFF)* CMRU;/*CMRU = 0.00244 V*/
}
//========================================================
 /* Аналого-Цифровое Преобразование для ДТОК с входа №1 АЦП CPU188R */
 /*  диапазон "минус 5 - 0 - +5" В*/
float Read_ADC1() { //TODO
	float res = 0;	//уровень сигнала в вольтах
	int i = 0;	//счетчик кол-ва повторных запросов в случае ошибок

	for (i=0;i<3;i++){
//		res = getADC(1);	//запрос данных с 2-ого АЦП
		res = getADC_SPI(1);
		if (res == -1){//ошибка
			if (F_DEBUG){
				SendByte(1,'E');SendByte(1,'R');SendByte(1,'R');SendByte(1,'O');SendByte(1,'R');
				SendByte(1,'A');SendByte(1,'D');SendByte(1,'C');SendByte(1,'1');
				SendByte(1,'i');SendByte(1,'=');SendByte(1,(uint8_t)i+'0');
			}
		}
		else break;	//чтение прошло успешно		
	}
	if (res == -1)
		return -1;
	res -= 2045;//res -= 1610;	//смещение ноля
	res = res * 0.0048828125;//res = res * .00367;//CMRU;/*CMRU = 0.00244 V*/
	return res;
//       a:byte;
//   h,l,s,z:word;

//   port[AdcY] =  0x17;/*($17) => вход №1 АЦП,усреднение по 16 выборкам,  диапазон "+_5"*/
//   delay(5);
//   do{
//   a= port[AdcStat] AND 0x80;
//   until(a > 0);
//   delay(5);
//   l= (portw[AdcL] AND 0x00FF);
//   h= (portw[AdcH] AND 0x00FF);
//   h= ((h SHL 8) AND 0xFF00);
//   res= ((l + h) AND 0x07FF)* CMRU;

//   if (((l + h) AND 0xE000) <> 0) then res =  res - 5;
}

float ADCR(){   //TODO
	float res = 0;	//уровень сигнала в вольтах
	int i = 0;	//счетчик кол-ва повторных запросов в случае ошибок
	for (i=0;i<3;i++){
		//res = getADC(0);	//запрос данных с 1-ого АЦП
		res = getADC_SPI(0);
		if (res == -1){
			if (F_DEBUG){
				SendByte(1,'E');SendByte(1,'R');SendByte(1,'R');SendByte(1,'O');SendByte(1,'R');
				SendByte(1,'A');SendByte(1,'D');SendByte(1,'C');SendByte(1,'R');
				SendByte(1,'i');SendByte(1,'=');SendByte(1,(uint8_t)i+'0');
			}
			//ошибка
		}
		else break;	//чтение прошло успешно		
	}
	if (res == -1)
		return -1;
	res -= 2045;//res -= 1820;	//смещение ноля
	res = res * 0.0048828125;//res = res * .00875;//CMRU;/*CMRU = 0.00244 V*/
	return res;
	//  a:byte;
//     l,h:word;
//   port[AdcY] =  0x0B;/*установка для АЦП ($0B) => вход №0 АЦП,
    //                  усреднение по 16 выборкам, диапазон "0-10"*/
//   delay_ms(2);

//   do{
//   a= port[AdcStat] AND 0x80;/*ждем окончания преобразования*/
//   until(a > 0);

//   delay(2);
//   l= (portw[AdcL] AND 0x00FF);
//   h= (portw[AdcH] AND 0x000F);
//   h= ((h SHL 8) AND 0x0F00);
//   rez= ((l + h) AND 0x0FFF)* CMRU;/*CMRU = 0.00244 V*/
}
