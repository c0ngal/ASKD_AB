#ifndef __MAIN_PROGRAM_H
#define __MAIN_PROGRAM_H

//#include "../inc/opora.h"
#include <stdint.h>                           /* Include standard types */

//#define  KT 0x03; //конец передачи текста

 //==== Глобальные переменные ==========
 //uint8_t Pbyte;
 extern uint8_t Rbyte;
 //extern uint8_t ssVixA3;
 //extern uint8_t ssVixA4;
 extern uint8_t a,b,c,d;
 extern unsigned short i,j,m,n;
 extern unsigned short md,nd;
 extern int q;
 extern int w/*,wr*/;/*текущий номер съема информации в архив испытаний/рабочий архив*/
 extern float x,y,z,t;

 extern float U1;/*напряжение, замеряемое для Riz*/
 extern uint8_t dd,dm,dg;/*переменные для установки даты*/
 extern uint8_t tch,tm,ts;/*переменные для установки времени*/
 extern uint8_t znak; /*знак разряда/заряда*/
 extern float Tok;/*Ток заряда/разряда*/
 extern float ZnU[33];//массив полных измеренных напряжений АБ //формат Pascal =>32+1=33
 extern float ZnUakb[33];/*массив напряжений отдельно взятых аккумуляторов (24 шт.)и полного Uab *///формат Pascal =>32+1=33
 extern float ZnUU[21]; /*массив для усредненного замера НАПРЯЖЕНИЯ*///формат Pascal =>20+1=21
 extern float ZnPU[11]; /*массив для усредненного замера ТОКА*///формат Pascal =>10+1=11
 /* ++++ */
 extern uint8_t ZnTekI[766];/*массив данных для ввода/вывода в byte-текущая инф-я*///pascal формат 765+1=766
 
 //TODO нужен ли дополнительный массив char?
 //char ZnTekIc[765];/*массив данных для ввода/вывода в char-архивная инф-я*/
 //char ZnArxIc[255];/*массив данных для ввода/вывода в char const*/
 extern float izmZn;

 //ArxC ArxM;
 extern uint8_t ZnArxI[256];/*массив данных для ввода/вывода в byte const*///формат Pascal =>255+1=256
 //char* stu;	//TODO нигде не употребляется?

 extern char str1TekI[256];//формат СИ

 //uint8_t ZnArxInf[257];/*массив данных,сформированных в команду, для вывода их из "Archiv." на ПЭВМ*///pascal формат 256+1=257
 extern uint8_t ZnUTekI[766];/*массив данных,сформированных в команду,  для вывода их из "TekInf.txt" на ПЭВМ*///pascal формат 765+1=766
 extern float izmZn;

 extern char stroka [6];//    :   string;	//формат СИ!!!

//признаки
 extern uint8_t Uvy,Uzy;/*признаки отключения АБ от нагрузки или от ЗУ*/
 extern uint8_t Uay;/*признак превышения напряжений*/
 extern uint8_t DD1;/*признак замкнутости хотя-бы 1-го датчика
                               давления*/
 extern uint8_t DD6;/*признак замкнутости 6-ти и более датчиков
                              давления*/
 extern uint8_t PrDTE;/*признак ДТЭ*/
 extern uint8_t PrPrdh;/*признак состояния предохранителя*/
 extern uint8_t PrKZ;/*признак КЗ (500 А)*/
 extern uint8_t U37,U27;/*признаки граничных значений напряжения АБ*/
 extern uint8_t U05[25],U16[25] ;/*признаки граничных значений напряжения i-го аккумулятора*///pascal формат 24+1=25
 extern unsigned short w159[25];	//pascal формат 24+1=25

extern  uint8_t PrRiz;/*признак Riz*/

 extern uint8_t PrSab;/*признак состояния АБ*/
 extern unsigned short Pr0 ;/*признак 1-го прохода после включения*/
 extern unsigned short PrR ;/*признак наличия РАЗРЯДА*/
 extern unsigned short PrZ ;/*признак наличия ЗАРЯДА*/
 extern uint8_t PrPZ;/*признак ПОДЗАРЯДА*/
 extern float tVs;/*текущее время в сек.*/
 extern uint8_t PrIo;/*признак знака dIo:$31-знак "минус";$30-знак "+"*/
 extern unsigned short PrA ;/*признак режима "А"*/
 extern uint8_t PrPC;

  /*я┐╜я┐╜я┐╜я┐╜я┐╜ "я┐╜"*/

 extern unsigned short PrD ;/*признак режима "Д": 1-режим "Д",
                                          0-другие режимы */
 extern float Tr0;/*время начала разряда*/
 extern float Tz0;/*время начала заряда*/
 extern float TrK;/*время окончания разряда*/
 extern float TzK;/*время окончания заряда*/
 extern float Trt;/*текущее время разряда(при очередном съеме)*/
 extern float Tzt;/*текущее время заряда(при очередном съеме)*/
 extern float Tr1;/*измеренное время разряда
                                 (для определения <>1 мин.?)*/
extern  float Tz1;/*измеренное время заряда
                                 (для определения <>3-х часов?)*/
 extern float CRS;/*суммарная (полная) емкость разряда*/
 extern float CZS;/*суммарная (полная) емкость ЗАРЯДА*/
 extern float OtbrC;/*Отобранная (полная) емкость разряда*/
 extern float SoobC;/*Сообщенная (полная) емкость ЗАРЯДА*/
 extern float PrgR;/*продолжительность РАЗРЯДА в сек.*/
 extern float PrgZ;/*продолжительность ЗАРЯДА в сек.*/
 extern float dtD;/*"дельтa Т" режима "Д" в сек. */
 extern float dtDm;/*"дельтa Т" цикла в мин. */
 extern float dtDch;/*"дельтa Т" режима "Д" в час. */
 extern float tD1;/*время 1-го съема для подсчета "дельты Т" в сек.*/
 extern float tD2;/*время 2-го съема для подсчета "дельты Т" в сек.*/
 extern uint8_t ZnDD[25] ;/*массив состояний ДД байтовый*///pascal формат 24+1=25
 extern uint16_t ZnDDl[25] ;/*массив состояний ДД словный*///pascal формат 24+1=25
 extern float t24DD[25];/*массив времен замкнутости каждого из ДД*///pascal формат 24+1=25
 extern float tRDD[25];/*массив предельных времен замкнутости  каждого из ДД после РАЗРЯДА*///pascal формат 24+1=25
 extern float tZmkDD[25];/*массив времен замкнутости//pascal формат 24+1=25
                               каждого из ДД в мин.*/
 extern unsigned short PrRP1 ;/*признак после разряда пошел подзаряд
                                 и при этом DDi =1*/
 extern unsigned short PrZP1 ;/*признак установки "Вкл.ПОДЗАРЯД"
                                    в зависимости от значения DD1*/
 extern unsigned short PrZR ;/*признак начала разряда после заряда
                                    (В ЗАРЯДЕ имел место РАЗРЯД)*/
 extern unsigned short PrPR ;/*признак начала разряда после подзаряда
                                (в ПОДЗАРЯДЕ имел место РАЗРЯД)*/
 extern unsigned short PrIZ ;/*признак измерения сопротивления изоляции  "1"- замер в данном цикле производим,
                      "0"-замер не делаем в данном цикле*/

  /* Р е ж и м   "ТО"  */

 extern unsigned short PrTO ;/*признак режима ТО: 1-режим ТО, 0-другие режимы */
 extern unsigned short PrDR ;/*признак ДОРАЗРЯДА в режиме ТО:
                                    1 - ДОРАЗРЯД, 0 - нет*/
 extern float Tdz0;/*время начала доразряда в сек.*/
 extern float Tdzt;/*текущее время доразряда в сек.
                         (при очередном съеме)*/
 extern float Tdz1;/*измеренное время доразряда
                           (для определения <> 12-ти часов?)*/
 extern unsigned short nTO ;/*признак очередного цикла
                             (заряд/разряд/доразряд) в режиме ТО*/

 extern unsigned short PrRRiz ;/*признак режима "Riz": 1-режим "Riz",
                                0-другие режимы */

  /*повременная запись информации в рабочем режиме*/

 extern float tVs1;/*целая часть текущего времени в мин.*/
 extern float tVs24;/*целая часть текущего времени в сек./10*/
 extern float t1M;/*конечное время на запись информации в архив
            при ЗАРЯДЕ/РАЗРЯДЕ после достижения U>=36,5B/U<=27,5B*/
 extern float t5M;/*конечное время на запись информации в архив первые 5мин. при ЗАРЯДЕ/РАЗРЯДЕ */
 extern float t10M;/*время момента вывода на запись информации в архив при  ЗАРЯДЕ/РАЗРЯДЕ каждые 10 мин.*/
 extern float t24ch;/*время момента вывода на запись информации в архив при  ПОДЗАРЯДЕ один раз в сутки.*/

 extern unsigned short Pr1M,Pr5M,Pr24 ;/*признаки вывода информации на запись в архив*/
 //========================================================
                 //только для технологической части

 extern unsigned short PrAA ;/*признак вывода архивного слова по
                                команде обновления*/
 extern unsigned short Pr1Min ;/*признак условия:
                                Время разряда > 1-ой минуты*/
 extern unsigned short Pr00 ;/*признак 1-го прохода после включения
                              для обнуления t замкн.ДД*/
 extern unsigned short t90 ;/*признак замкнутости ДД более 90 минут*/

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
     //формирование поправочных коэффициентов по U и I
 extern uint8_t kU[129];
 extern uint8_t kUc[129];
 extern uint8_t kI[13];
 extern uint8_t kIc[13];

 extern char kS[6];
 extern float kUr[25];
 extern char stx[25][6];//[24] of string[5];
 extern char ka[6];  //: string[5];
 extern char kb[129];

 extern float kZr,kRr;
 extern char kSz[6],kSr[6]; // : string[5];
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


void mainProgram(void);

#endif // __MAIN_PROGRAM_H
