#ifndef __SERVICE_FUNCTIONS_H
#define __SERVICE_FUNCTIONS_H
#include "global.h"

//extern uint8_t ssVixA3;
//extern uint8_t ssVixA4;
extern uint8_t a,b,c,d;
extern int w/*,wr*/;/*текущий номер съема информации в архив испытаний/рабочий архив*/
extern float x,y,z,t;

extern float U1;/*напряжение, замеряемое для Riz*/
extern uint8_t dd,dm,dg;/*переменные для установки даты*/
extern uint8_t tch,tm,ts;/*переменные для установки времени*/
extern float tVs;/*текущее время в сек.*/
extern float tVs1;/*целая часть текущего времени в мин.*/
extern float tVs24;/*целая часть текущего времени в сек./10*/
extern float Tok;/*Ток заряда/разряда*/
extern float ZnU[33];/*массив полных измеренных напряжений АБ*///формат Pascal =>32+1=33
extern float ZnUakb[33];/*массив напряжений отдельно взятых аккумуляторов (24 шт.)и полного Uab *///формат Pascal =>32+1=33
extern float ZnUU[21]; /*массив для усредненного замера НАПРЯЖЕНИЯ*///формат Pascal =>20+1=21
extern uint8_t PrSab;/*признак состояния АБ*/
extern uint8_t PrIo;/*признак знака dIo:$31-знак "минус";$30-знак "+"*/
extern uint8_t ZnArxI[256];/*массив данных для ввода/вывода в byte const*///формат Pascal =>255+1=256
extern char stroka [6];//    :   string;	//формат СИ
extern float dtD;/*"дельтa Т" режима "Д" в сек. */
extern float dtDm;/*"дельтa Т" цикла в мин. */
extern float dtDch;/*"дельтa Т" режима "Д" в час. */
extern float tD1;/*время 1-го съема для подсчета "дельты Т" в сек.*/
extern float tD2;/*время 2-го съема для подсчета "дельты Т" в сек.*/
extern unsigned short Pr0 ;/*признак 1-го прохода после включения*/
extern float izmZn;

extern uint8_t Uvy,Uzy;/*признаки отключения АБ от нагрузки или от ЗУ*/
extern uint8_t Uay;/*признак превышения напряжений*/
extern uint8_t DD1;/*признак замкнутости хотя-бы 1-го датчика
                               давления*/
extern uint8_t DD6;//признак замкнутости 6-ти и более датчиков
extern uint8_t PrDTE;/*признак ДТЭ*/
extern uint8_t PrPrdh;/*признак состояния предохранителя*/
extern uint8_t PrKZ;/*признак КЗ (500 А)*/

extern float CRS;/*суммарная (полная) емкость разряда*/
extern float CZS;/*суммарная (полная) емкость ЗАРЯДА*/
extern uint8_t U37,U27;/*признаки граничных значений напряжения АБ*/
extern uint8_t U05[25],U16[25] ;/*признаки граничных значений напряжения i-го аккумулятора*///формат Pascal =>24+1=25
extern unsigned short w159[25];//формат Pascal =>24+1=25

extern uint8_t PrRiz;/*признак Riz*/
extern uint8_t PrPC;

 extern uint8_t ZnDD[25] ;/*массив состояний ДД байтовый*///pascal формат 24+1=25
 extern uint16_t ZnDDl[25] ;/*массив состояний ДД словный*///pascal формат 24+1=25
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
     //формирование поправочных коэффициентов по U и I
 extern uint8_t kU[129];
// extern uint8_t kUc[129];
 extern uint8_t kI[13];
 //extern uint8_t kIc[13];

 extern char kS[6];
 extern float kUr[25];
/* extern char stx[25][6];//[24] of string[5];
 extern char ka[6];  //: string[5];
 extern char kb[129];
*/
 extern float kZr,kRr;
 extern char kSz[6],kSr[6]; // : string[5];
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


//void RdTok(void);	//TODO
void DTOK(float x);

//Дата и время
void YstDate(void);
void YstTime(void);
void RDate(void);
void RDate2(void);
void RDate1(void);

/*Подсчет периода опроса ["дельты Т"]*/
void VdtD(void);
/*подсчет "С разр."*/
void SCraz(void);
 /*подсчет "С зар."*/
void SCzar(void);
/*Измерение полных напряжений АБ и Ак[i]*/
void izmU(void);
/*Вычисление истинных значений напряжений Ак[i] + полное Uab*/
void Uakb(void);
/*Преобразование напряжений АБ и Ак[i] из real в строку в byte*/
void Ubyte(void);
/*Анализ граничных напряжений АБ и Ак[i] и выработка признаков отключения АБ*/
void GrU(void);
/*Обработка показаний датчиков давления и формирование признаков*/
void DtDv(void);
/*Обработка показаний ДТЭ, формирование признака и запись в ZnTekIc(ZnTekI)*/
void DTE(void);
/*Анализ состояния предохранителя, тока КЗ и формирование признаков*/
void Prdh(void);
/*Обработка признака работы с ПЭВМ и запись его в ZnTekIc(ZnTekI)*/
void PC(void);
void Sbros(void);

/*Снятие сигнала с выхода А4*/
void SVixA4(uint8_t a);
/*Установка сигнала на выходе А4*/
void YVixA4(uint8_t a);
/*Снятие сигнала с выхода А3*/
void SVixA3(uint8_t a);
/*Установка сигнала на выходе А3*/
void YVixA3(uint8_t a);
 /* Определение № записи архивa испытаний*/
void NomZapArx(void);
/*Анализ состояния "R" изоляции и формирование признаков*/
void Riz(void);
//void NomRArx(void); /* Определение № записи рабочего архива*/
void NRA(void); /* Подготовка и вывод № записи рабочего архива*/
/*Установка команды АСКД "ИСПРАВНО"*/
 void YstISP(void);
/*Сброс команды АСКД "ИСПРАВНО" */
 void SbrISP(void);
//============================================
/*Установка команды с АСКД на САЭ*/
 void Yst(uint8_t m);
//============================================
/*Сброс команды с АСКД на САЭ*/
 void Sbr(uint8_t m);
//============================================
/*Установка команды Riz НЕ НОРМА*/
 void YRNN(void);
//============================================
 /*Сброс команды Riz НЕ НОРМА*/
 void SRNN(void);
//============================================
/*Установка команды Riz НОРМА*/
 void YRN(void);
//============================================
  /*Сброс команды Riz НОРМА*/
 void SRN(void);
//============================================
/*Установка "НЕ НОРМА"*/
 void YNN(uint8_t m);
//============================================
/*Сброс "НЕ НОРМА"*/
 void SNN(uint8_t m);
/*============================================*/
void Sbros(void);
/*kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk*/
/*Прием коэффициентов по команде и запись их в "TekInf.txt" */
void KFUzap(void);
/*========================================================*/
/*Считывание коэффициентов из "TekInf.txt" и запись их в массив "kUr[1..24]" */
void KFUcht(void);
/*Прием коэффициентов по ТОКУ по команде и запись их в "TekInf.txt"  */
void KFIzap(void);
/*========================================================*/
 void KFIIcht(void);
  
 #endif //__SERVICE_FUNCTIONS_H
