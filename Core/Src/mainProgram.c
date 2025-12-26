#include "mainProgram.h"
#include "global.h"
#include "MemoryFunctions.h"
#include "ServiceFunctions.h"
#include "UART.h"
#include "UNIO.h"
#include "ADC.h"
#include <ctype.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "flash_ext.h"
#include "Flash_SSt25.h"

 uint8_t Rbyte;
 uint8_t ssVixA3;
 uint8_t ssVixA4;
 uint8_t a,b,c,d;
 unsigned short i,j,m,n;
 unsigned short md,nd;
 int q;
 int w/*,wr*/;/*текущий номер съема информации в архив испытаний/рабочий архив*/
 float x,y,z,t;

 float U1;/*напряжение, замеряемое для Riz*/
 uint8_t dd,dm,dg;/*переменные для установки даты*/
 uint8_t tch,tm,ts;/*переменные для установки времени*/
 uint8_t znak; /*знак разряда/заряда*/
 float Tok;/*Ток заряда/разряда*/
 float ZnU[33];//массив полных измеренных напряжений АБ //формат Pascal =>32+1=33
 float ZnUakb[33];/*массив напряжений отдельно взятых аккумуляторов (24 шт.)и полного Uab *///формат Pascal =>32+1=33
 float ZnUU[21]; /*массив для усредненного замера НАПРЯЖЕНИЯ*///формат Pascal =>20+1=21
 float ZnPU[11]; /*массив для усредненного замера ТОКА*///формат Pascal =>10+1=11
 /* ++++ */
 //uint8_t ZnTekI[766];/*массив данных для ввода/вывода в byte-текущая инф-я*///pascal формат 765+1=766
 
 //TODO1 нужен ли дополнительный массив char?
 //char ZnTekIc[765];/*массив данных для ввода/вывода в char-архивная инф-я*/
 //char ZnArxIc[255];/*массив данных для ввода/вывода в char const*/
 float izmZn;

 uint8_t ZnArxI[256];/*массив данных для ввода/вывода в byte const*///формат Pascal =>255+1=256
 
 char str1TekI[256];//формат СИ

 //uint8_t ZnArxInf[257];/*массив данных,сформированных в команду, для вывода их из "Archiv." на ПЭВМ*///pascal формат 256+1=257
 uint8_t ZnUTekI[766];/*массив данных,сформированных в команду,  для вывода их из "TekInf.txt" на ПЭВМ*///pascal формат 765+1=766
 float izmZn;

 char stroka [6];//    :   string;	//формат СИ!!!

//признаки
 uint8_t Uvy,Uzy;/*признаки отключения АБ от нагрузки или от ЗУ*/
 uint8_t Uay;/*признак превышения напряжений*/
 uint8_t DD1;/*признак замкнутости хотя-бы 1-го датчика
                               давления*/
 uint8_t DD6;/*признак замкнутости 6-ти и более датчиков
                              давления*/
 uint8_t DD24[25];/*признаки замкнутости датчиков давления более 24 часов*///pascal формат 24+1=25
 uint8_t PrDTE;/*признак ДТЭ*/
 uint8_t PrPrdh;/*признак состояния предохранителя*/
 uint8_t PrKZ;/*признак КЗ (500 А)*/
 uint8_t U37,U27;/*признаки граничных значений напряжения АБ*/
 uint8_t U05[25],U16[25] ;/*признаки граничных значений напряжения i-го аккумулятора*///pascal формат 24+1=25
 unsigned short w159[25];	//pascal формат 24+1=25

 uint8_t PrRiz;/*признак Riz*/

 uint8_t PrSab;/*признак состояния АБ*/
 unsigned short Pr0 ;/*признак 1-го прохода после включения*/
 unsigned short PrR ;/*признак наличия РАЗРЯДА*/
 unsigned short PrZ ;/*признак наличия ЗАРЯДА*/
 uint8_t PrPZ;/*признак ПОДЗАРЯДА*/
 float tVs;/*текущее время в сек.*/
 uint8_t PrIo;/*признак знака dIo:$31-знак "минус";$30-знак "+"*/
 unsigned short PrA ;/*признак режима "А"*/
 uint8_t PrPC;

  /*я┐╜я┐╜я┐╜я┐╜я┐╜ "я┐╜"*/

 unsigned short PrD ;/*признак режима "Д": 1-режим "Д",
                                          0-другие режимы */
 float Tr0;/*время начала разряда*/
 float Tz0;/*время начала заряда*/
 float TrK;/*время окончания разряда*/
 float TzK;/*время окончания заряда*/
 float Trt;/*текущее время разряда(при очередном съеме)*/
 float Tzt;/*текущее время заряда(при очередном съеме)*/
 float Tr1;/*измеренное время разряда
                                 (для определения <>1 мин.?)*/
 float Tz1;/*измеренное время заряда
                                 (для определения <>3-х часов?)*/
 float CRS;/*суммарная (полная) емкость разряда*/
 float CZS;/*суммарная (полная) емкость ЗАРЯДА*/
 float OtbrC;/*Отобранная (полная) емкость разряда*/
 float SoobC;/*Сообщенная (полная) емкость ЗАРЯДА*/
 float PrgR;/*продолжительность РАЗРЯДА в сек.*/
 float PrgZ;/*продолжительность ЗАРЯДА в сек.*/
 float dtD;/*"дельтa Т" режима "Д" в сек. */
 float dtDm;/*"дельтa Т" цикла в мин. */
 float dtDch;/*"дельтa Т" режима "Д" в час. */
 float tD1;/*время 1-го съема для подсчета "дельты Т" в сек.*/
 float tD2;/*время 2-го съема для подсчета "дельты Т" в сек.*/
 uint8_t ZnDD[25] ;/*массив состояний ДД байтовый*///pascal формат 24+1=25
 uint16_t ZnDDl[25] ;/*массив состояний ДД словный*///pascal формат 24+1=25
 float t24DD[25];/*массив времен замкнутости каждого из ДД*///pascal формат 24+1=25
 float tRDD[25];/*массив предельных времен замкнутости  каждого из ДД после РАЗРЯДА*///pascal формат 24+1=25
 float tZmkDD[25];/*массив времен замкнутости//pascal формат 24+1=25
                               каждого из ДД в мин.*/
 unsigned short PrRP1 ;/*признак после разряда пошел подзаряд
                                 и при этом DDi =1*/
 unsigned short PrZP1 ;/*признак установки "Вкл.ПОДЗАРЯД"
                                    в зависимости от значения DD1*/
 unsigned short PrZR ;/*признак начала разряда после заряда
                                    (В ЗАРЯДЕ имел место РАЗРЯД)*/
 unsigned short PrPR ;/*признак начала разряда после подзаряда
                                (в ПОДЗАРЯДЕ имел место РАЗРЯД)*/
 unsigned short PrIZ ;/*признак измерения сопротивления изоляции  "1"- замер в данном цикле производим,
                      "0"-замер не делаем в данном цикле*/

  /* Р е ж и м   "ТО"  */

 unsigned short PrTO ;/*признак режима ТО: 1-режим ТО, 0-другие режимы */
 unsigned short PrDR ;/*признак ДОРАЗРЯДА в режиме ТО:
                                    1 - ДОРАЗРЯД, 0 - нет*/
 float Tdz0;/*время начала доразряда в сек.*/
 float Tdzt;/*текущее время доразряда в сек.
                         (при очередном съеме)*/
 float Tdz1;/*измеренное время доразряда
                           (для определения <> 12-ти часов?)*/
 unsigned short nTO ;/*признак очередного цикла
                             (заряд/разряд/доразряд) в режиме ТО*/

 unsigned short PrRRiz ;/*признак режима "Riz": 1-режим "Riz",
                                0-другие режимы */

  /*повременная запись информации в рабочем режиме*/

 float tVs1;/*целая часть текущего времени в мин.*/
 float tVs24;/*целая часть текущего времени в сек./10*/
 float t1M;/*конечное время на запись информации в архив
            при ЗАРЯДЕ/РАЗРЯДЕ после достижения U>=36,5B/U<=27,5B*/
 float t5M;/*конечное время на запись информации в архив первые 5мин. при ЗАРЯДЕ/РАЗРЯДЕ */
 float t10M;/*время момента вывода на запись информации в архив при  ЗАРЯДЕ/РАЗРЯДЕ каждые 10 мин.*/
 float t24ch;/*время момента вывода на запись информации в архив при  ПОДЗАРЯДЕ один раз в сутки.*/

 unsigned short Pr1M,Pr5M,Pr24 ;/*признаки вывода информации на запись в архив*/
 //========================================================
                 //только для технологической части

 unsigned short PrAA ;/*признак вывода архивного слова по
                                команде обновления*/
 unsigned short Pr1Min ;/*признак условия:
                                Время разряда > 1-ой минуты*/
 unsigned short Pr00 ;/*признак 1-го прохода после включения
                              для обнуления t замкн.ДД*/
 unsigned short t90 ;/*признак замкнутости ДД более 90 минут*/
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
     //формирование поправочных коэффициентов по U и I
 uint8_t kU[129];
 uint8_t kUc[129];
 uint8_t kI[13];
 uint8_t kIc[13];

 char kS[];
 float kUr[25];
 char stx[25][6];//[24] of string[5];
 char ka[6];  //: string[5];
 char kb[129];

 float kZr,kRr;
 char kSz[6],kSr[6]; // : string[5];
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	
void mainProgram(){
	int i = 0;	//переменная для циклов
	int f = 0;	//переменная для флагов
	int width = 0;	//для преобразования из числа в строку

 //SetCom2;/*СОМ2 => в рабочее состояние*/
 //portConfig();
// portw[AdrRY]=  0;
// portw[AdrRY]=  0x007B;   /*Установка Rg управления СОМ2*/

 //TODO1 по умолчанию - на передачу, нужно ли еще раз?
 //port[AdAK+3] = 0x00;/*установка разъема J8 (каналы 0...23) CPU188R  на "передачу" для управления аналоговым коммутатором*/
 //port[AdAK] =  0x00; /*установка регистра "А" выходов разъема J8 в "0"*/
//	for (i=24;i<30;i++)
	//	setUNIO(i,0);

	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
      KFUcht();
      KFIIcht();
 //kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk

 //Исходная установка модулей выходов 
	//TODO
    //port[AdrO1]:= $00;
    //port[AdrO2]:= $00;
    //port[AdrO3]:= $00;
    //port[AdrO4]:= $00;

	
/*Чтение информации из внешнего файла TekInf.txt и запись её */
/* в символьный и байтовый массивы ZnTekI(c) */
	RTekI();
	//подготовка памяти - если нет данных о том , что то-то записано - стираем память
	//InitMemory();
	//ssVixA3 = 0;/*начальная установка для выходов на САЭ*/
	//ssVixA4 = 0;

 /*Исходная установка признаков*/
     PrR   = 0;
     PrZ   = 0;
     Pr0   = 0;
     PrSab = 0;
     PrRiz = 0;
     PrD   = 0;
     PrTO  = 0;
     PrPZ  = 0;

     PrZP1 = 0;
     PrRP1 = 0;
     PrZR  = 0;
     PrPR  = 0;
		 
		 PrIZ  = 1;
 /*++++++++++++++++++*/
  /*Обнуление архивной строки кроме № съема инфр.*/
/*  i = 1;		//TODO1 нужен ли дополнительный массив char?
  do{
  ZnArxIc[i] = '0';
  i++;
  }while(i < 255);*/

  i= 1;
  do{
  w159[i] = 0;
  i++;
  }while(i < 25);

  i=  1;
  do{
  ZnArxI[i] = 0x30;
  i++;
  }while(i < 255);

  ZnU[27] = 0;
  //TODO1
  //port[AdAK] = 0x3A;/*26 вход АК:"+5 В Контр."  */
		 setUNIO(27,1);setUNIO(26,0);setUNIO(25,1);setUNIO(24,0);setUNIO(29,0);setUNIO(28,1);
  delay_ms(50);

  izmZn = ADC0();	//TODO1
  ZnU[27] = izmZn;
  if ((4.6 < ZnU[27]) && (ZnU[27] < 5.1)){										                         
																YstISP();		//ИСПРАВНО//TODO
		                                         //port[AdAK] = 0x00;
                                              //delay_ms(10);
                                              //izmZn = ADC0();
                                            }
  else{
																						SbrISP();		//НЕИСПРАВНО
                                             //old version a = 0x10;
                                           // SVixA3(a);//НЕИСПРАВНО
                                             //TODO1
                                             //HALT(0);
																							return;
                                            }

   //Анализ признака РАБОТЫ с ПЭВМ
  PC();
  delay_ms(10);
  if  (ZnTekI[399] == 0x31) goto L71;/*работаем с ПЭВМ*/

/*Анализ наличия работы в выбранных режимах*/
L1:   																						
     //md = portw[AdrI2];
     //delay_ms(10);
		nd = (getDigitalInSPI(24) << 8) | (getDigitalInSPI(25) << 9) | (getDigitalInSPI(26) << 10);		
//+++++++++++TODO! !!!!!!!!!!!!! Для макета, потому что не исправен канал на DigIn !!!!!!!!!!
//		nd = (getDigitalInSPI(31) << 8) | (getDigitalInSPI(25) << 9) | (getDigitalInSPI(26) << 10);		
//--------------TODO! !!!!!!!!!!!!! Для макета, потому что не исправен канал на DigIn !!!!!!!!!!
     //nd = md & 0x0700;
     switch (nd){
//DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
         /*Р е ж и м   "D" */
         /*================*/

     case 0x0200:
     case 0x0300:

         if (PrD == 0){

        /*сброс всех сообщений режимов, кроме D*/
					 Sbr(rTO);
//         a = 0xA0;
  //       SVixA3(a);/*A,TO*/
					 //TODO1 по-умолчанию и так выход
         //port[AdrBA +3] =  0;
         //delay_ms(5);
         //port[AdrBA]= 0x00;/*Riz*/
					 for (i=0;i<8;i++){
						 setUNIO(24+i,0);}
					 if (nd == 0x0300) PrIZ = 1; else PrIZ = 0;

         PrR   = 0;
         PrZ   = 0;
         PrPZ  = 0;
         PrD   = 1;
         PrTO  = 0;

         PrZP1 = 0;
         PrRP1 = 0;
         PrZR  = 0;
         PrPR  = 0;

         Yst(rD);/*Установка сообщения "Режим "Д" включен"*/
         Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"
         Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
         Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ"

         /*Анализ состояния датчиков ДД1 ...ДД24*/
         DtDv();/*DD1,DD6*/
         if  (ZnTekI[282] == 0x31) goto L21; /*DD1*/

         delay_ms(1000);

         Yst(VPZ);/*Установить ком-ду "Включить ПОДЗАРЯД АБ"*/
     }
             
L21:      if (nd == 0x0200){ //port[AdrBA] =  0x00; /*установка регистра "А" выходов разъема J9 в "0"*/
                                            /*сброс установок измерения Riz.*/
/*						for(i=0;i<8;i++){
							setUNIO(24+i,0);}*/
							SRNN();
							SRN();
						}

L2:                     /* опрос состояния АБ */

     ZnU[27] =  0;
		 //TODO1
     //port[AdAK] = 0x3A;
		 setUNIO(27,0);setUNIO(26,1);setUNIO(25,0);setUNIO(24,1);setUNIO(29,0);setUNIO(28,1);
     delay_ms(40);
     izmZn = ADC0();
     ZnU[27] = izmZn;	//TODO!!! не работает 2ая МК АК => не мерим 5В контр
     if ((4.6 < ZnU[27]) && (ZnU[27] < 5.1)){
         YstISP();//ИСПРАВНО
     }
     else{
			 delay_ms(40);
			 izmZn = ADC0();
			 ZnU[27] = izmZn;	//TODO!!! не работает 2ая МК АК => не мерим 5В контр
			 if ((4.6 < ZnU[27]) && (ZnU[27] < 5.1)){
         YstISP();//ИСПРАВНО
				 }
			 else{
         SbrISP();//НЕИСПРАВНО
         //HALT(0);	//TODO1
				 return;
			 }
     }

     //формирование текущего числа съемов информации
/*     for(i=0;i<5;i++)	//TODO1 нужен ли масси символов
         str1TekI[i] = ZnTekI[755+i];// ZnTekIc[755+i];//str1TekI = ZnTekIc[755]+ZnTekIc[756]+ZnTekIc[757]+ZnTekIc[758]+ZnTekIc[759];
     str1TekI[5] = 0x00;
     //Val(str1TekI,wr,q);//wr - текущий № измерения в рабочем режиме
     wr = atoi(str1TekI);
     if (wr == 0){//я┐╜шибя┐╜я┐╜!

     }*/

     /*+++++++++++*/
    RDate2();/*запись текущего времени и даты в ZnTekI(c)*/

//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
  KFUcht();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk	
    /*//измерение показаний датчиков давления
    //  и подготовка их значений для записи в файл "TekInf.txt"
    DtDv();//DD1,DD6

 //проверка состояния предохранителя и наличия тока КЗ и подготовка их значений для записи в файл "TekInf.txt"
    Prdh();

 //измерение показаний датчиков течи электролита и подготовка их значений для записи в файл "TekInf.txt"
    DTE();*/


 /*измерение напряжений и подготовка их значений для записи в файл "TekInf.txt"*/
 sendDebug("izmU"); 
    izmU();
 sendDebug("End"); 
    Uakb();
    Ubyte();
    GrU();
 //kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
  KFUcht();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk	

/*измерение тока заряда/разряда и подготовка их значений для записи в файл "TekInf.txt"*/
delay_ms(30);	//задержка, перед опросом тока
    i = 1;
    do{
    izmZn = Read_ADC1();
    ZnPU[i] =  izmZn;
    i++;
    }while(i < 11);

    z = 0;
    i = 1;
    do{
    z =  z + ZnPU[i];
    i++;
    }while(i < 11);

    izmZn =  z/10.;
			if (F_DEBUG){
				sprintf(kS,"%6.3f\0",izmZn);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
				sendDebug("T");sendDebug(kS);}
    DTOK(izmZn);

     //измерение показаний датчиков давления

    //  и подготовка их значений для записи в файл "TekInf.txt"
    DtDv();//DD1,DD6

 //проверка состояния предохранителя и наличия тока КЗ и подготовка их значений для записи в файл "TekInf.txt"
    Prdh();

 //измерение показаний датчиков течи электролита и подготовка их значений для записи в файл "TekInf.txt"
    DTE();
 //запись в файл "TekInf.txt"
   WTekI();
    delay_ms(5);
 /*чтение из файла "TekInf.txt"*/
   RTekI();
//ZapRArx();
 /*Считаем "дельту" t (dtD)*/
    RDate1();/*Время в сек. с начала суток "tVs"*/
    VdtD(); /*подсчет периода опроса*/
 /*====================================*/
    Pr0 =  1;
    /*Анализ состояния ПР. i KZ*/
    if  ((ZnTekI[356] == 0x30) && (ZnTekI[357] == 0x30)){
        SNN(PR); //Сброс НЕ НОРМА ПР.
    }
    else{
        YNN(PR);//Установка НЕ НОРМА ПР.
        Sbros();
        goto L1;
    }

    /*Анализ состояния ДТЭ*/
    if  (ZnTekI[294] == 0x30){
        SNN(DT);//Сброс НЕ НОРМА ДТЭ
    }
    else{
        YNN(DT);//Установка НЕ НОРМА ДТЭ
        Sbros();
        goto L1;
    }

    /*Анализ состояния датчиков ДД1 ...ДД24*/
    if  (ZnTekI[282] == 0x30){ /*DD1*/
        i = 1;
        do{
            t24DD[i] =  0;
            i++;
        }while(i < 25);
				SNN(DDv);//сброс "DD" не норма
			}
		else{  /*отсчитываем очередную порцию времени замкнутости ДД*/
			i = 1;
			j = 258;
			do{
        if (ZnTekI[j] == 0x31){
					t24DD[i] =  t24DD[i] + dtDm;
        }
        j++;
        i++;
			}while(j < 282);
		}
		/*Вывод текущего значения времени замкнутости ДД в мин.*/
        i = 1;
        j = 0;
        m = 0;
				f = 0;
        do{
						int width = 0;
            tZmkDD[i] =  t24DD[i];
						if (t24DD[i] >= 1440){ 
							DD24[i] = 1; f = 1;}
						else DD24[i] = 0;
					

            //Str(tZmkDD[i]:6:1,stroka);
            width = sprintf(stroka,"%6.1f\0",tZmkDD[i]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 1 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
            if (width != 6){//я┐╜шибя┐╜я┐╜!!!
            }

            ZnTekI[411 + j] = stroka[0];
            ZnTekI[412 + j] = stroka[1];
            ZnTekI[413 + j] = stroka[2];
            ZnTekI[414 + j] = stroka[3];
            ZnTekI[415 + j] = stroka[4];
            ZnTekI[416 + j] = stroka[5];

            ZnArxI[116 + m] = stroka[0];
            ZnArxI[117 + m] = stroka[1];
            ZnArxI[118 + m] = stroka[2];
            ZnArxI[119 + m] = stroka[3];

            m = m + 4;
            j = j + 6;
            i++;
        }while(i < 25);
				
				/*Анализ состояния ДД6*/
				if  (ZnTekI[283] == 0x30 && !f){
					SNN(DDv);//Сброс НЕ НОРМА ДД
					}
				else{
					YNN(DDv);//Установка НЕ НОРМА ДД
					Sbros();
					goto L1;
				}

				
/*L8:*/              switch (PrSab){
                     case 0x31: goto L10; //break; /*ЗАРЯД*/
                     case 0x32: goto L12; //break; /*РАЗРЯД*/
                     case 0x33: goto L17; //break; /*ПОДЗАРЯД*/
                     }

L12:           /*Р А З Р Я Д*/
							 
               Pr24 = 0;
               if (PrR == 0){

                   if (PrZ == 0) PrZR = 0;
                   else PrZR = 1;/*РАЗРЯД после ЗАРЯДА*/
                   if (PrPZ == 0) PrPR = 0;
                   else PrPR = 1;/*запоминаем время и дату начала РАЗРЯДА*/

                   PrR   =  1;
                   PrZ   =  0;
                   PrPZ  =  0;

                   PrZP1 =  0;
                   PrRP1 =  0;

                   RDate1();
                   Tr0 =  tVs;/*запоминаем время и дату начала РАЗРЯДА*/
                   CRS =  0;/*"С разр." := 0*/

                   t5M  =  tVs1 + 5;
                   Pr5M =  1;
                   t10M =  tVs1 + 15;
                   Pr1M =  0;
                   t1M  =  tVs1 + 1;

                   Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ"
               }

               SCraz(); /*считаем "С разр."*/
               /* Запись текущего значения разрядной емкости*/
               //Str(CRS:5:2,stroka);
               width = sprintf(stroka,"%5.2f\0",CRS);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 5, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 2 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
               if (width != 5){//я┐╜шибя┐╜я┐╜!!!
               }
               ZnTekI[384]= stroka[0];
               ZnTekI[385]= stroka[1];
               ZnTekI[386]= stroka[2];
               ZnTekI[387]= stroka[3];
               ZnTekI[388]= stroka[4];

               ZnArxI[212]= stroka[0];
               ZnArxI[213]= stroka[1];
               if (stroka[2] == '.') ZnArxI[214] = stroka[3];
               else ZnArxI[214] = stroka[2];

   /*i = 384;
   do{
       ZnTekIc[i] = ZnTekI[i];//TODO1 ???
       i++;
   }while(i < 389);

   i = 212;
   do{
       ZnArxIc[i]= ZnArxI[i];//TODO1 ???
       i++;
   }while(i < 215);*/

               RDate1();
               if (tVs1 > t5M) Pr5M = 0;

               if (ZnTekI[354] == 0x30){ /*Uvy => '<27V,<0.5V'*/
                   SNN(Cr);  //Сброс "С разр." не норма
                   RDate1();
                   Trt =  tVs;
                   Tr1 =  Trt - Tr0;
                   if (Tr1 < 0){ /*вычисление t разряда*/
                       Tr1= (86400 -Tr0) + Trt;
                   }
                   Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
                   goto L9;
               }
               else{
                   Yst(ORaz);//Установить ком-ду "Отключить РАЗРЯД АБ"

                   delay_ms(5000);

                   Yst(VZar);//Установка ком-ды "Включить ЗАРЯД АБ"
                   goto L9;
               }
              /*================================*/
L9:  if (Pr5M == 1){
             if (tVs1 >= (int)(t1M)){
                  //NomRArx();
                  WTekI();
                  ZapRArx();
                  t1M =  tVs1 + 1;
             }
             goto L7;
     }

    if (PrZ == 1){
           if (ZnUakb[1] >= 36.5){
L5:             if (Pr1M == 1){
L4:                 if (tVs1 >= (int)(t1M)){
                       //NomRArx();
                       WTekI();
                       ZapRArx();
                       t1M =  tVs1 + 1;
                   }

                   if (ZnTekI[354] == 1) Pr1M = 0;
                   if (ZnTekI[359] == 1) Pr1M = 0;
                   goto L7;
               }
               else{
                   t1M =  tVs1 + 1;
                   Pr1M =  1;
                   goto L4;
               }
           }
           else{
L6:                   if (tVs1 >= (int)(t10M)){
                      //NomRArx();
                      WTekI();
                      ZapRArx();
                      t10M =  tVs1 + 10;
               }
               goto L7;
           }
    }
    else{
        if (PrR == 1){
            if (ZnUakb[1] <= 27.5) goto L5;
            else goto L6;
        }
        else goto L7;
    }
L7:  if (PrSab == 0x33){
        RDate1();
        if (Pr24 == 0){
            //NomRArx();
            WTekI();
            ZapRArx();
            Pr24 =  1;
            t24ch =  tVs24;/*время */
        }
        else{
            if (t24ch == tVs24){
                //NomRArx();
                WTekI();
                ZapRArx();
            }
            goto L20;
        }
        goto L20;
    }
    goto L20;

L10:           /*З А Р Я Д*/

              Pr24 = 0;
              if (PrZ == 0){
                  PrZ =  1;
                  PrR =  0;
                  PrPZ =  0;
                  PrZP1 =  0;
                  PrRP1 =  0;
                  PrZR  =  0;
                  PrPR  =  0;

                  Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"

                  RDate1();
                  Tz0 =  tVs;/*запоминаем время начала ЗAРЯДА*/

                  CRS =  0;/*обнуление "С" разряда*/

                  i = 384;
                  do{
                      ZnTekI[i] =  0x30;
                      i++;
                  }while(i < 389);

                  ZnArxI[212] = 0x30;
                  ZnArxI[213] = 0x30;

                  i = 384;/*
                  do{
                      ZnTekIc[i]=  ZnTekI[i]; //TODO1 ???
                      i++;
                  }while(i < 389);

                  i = 212;
                  do{
                      ZnArxIc[i] = ZnArxI[i];//TODO1 ???
                      i++;
                  }while(i < 215);*/

                  t5M  =  tVs1 + 5;
                  Pr5M =  1;
                  t10M =  tVs1 +15;
                  Pr1M =  0;
                  t1M  =  tVs1 + 1;
              }

              RDate1();
              if (tVs1 > t5M) Pr5M = 0;

              if (ZnTekI[355] == 0x31){  /*Uzy => ">37V,>1.59V,DD1=1"*/
L18:              Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"

                  delay_ms(5000);

                  if (ZnTekI[282] == 0x31){ /*DD1*/
                      PrZP1 = 1;
                      goto L9;
                  }
                  else{
                      PrZP1 = 0;
                      Yst(VPZ);//Установить ком-ду "Включить ПОДЗАРЯД АБ"
                      goto L9;
                  }
              }
              else{
                  RDate1();
                  Tzt = tVs;
                  Tz1= Tzt - Tz0;
                  if (Tz1 < 0){
                      Tz1 = (86400 -Tz0) + Tzt;
                  }
                  if (Tz1 > 10800) goto L18;
                  else goto L9;
              }

L17:    /* П О Д З А Р Я Д */

       if (PrZR == 1){
           if (ZnTekI[282] == 0x30){ /*DD1*/
               PrPR =  0;
               Yst(VZar);//Установить ком-ду "Включить ЗАРЯД АБ"
               goto L9;
           }
           else goto L22;
       }
       if (PrPR == 1){
           PrR =  0;
           PrZR =  0;

           if (Tr1 > 60){
               if (ZnTekI[282] == 0x30){ /*DD1*/
                   Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ"

                   delay_ms(5000);

                   Yst(VZar);//Установить ком-ду "Включить ЗАРЯД АБ"
                   goto L9;
               }
               else goto L22;
           }
           else{
               if (ZnTekI[282] == 0x30){ /*DD1*/
                   Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"

                   delay_ms(5000);

                  Yst(VPZ);//Установить ком-ду "Включить ПОДЗАРЯД АБ"
               }
               else goto L22;
           }
       }

       if (PrZP1 == 1){
           if (ZnTekI[282] == 0x31){ /*DD1*/
               Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ"
               PrZP1 = 0;
               goto L9;
           }
           else{
               PrZP1 =  0;
               Yst(VPZ);//Установить ком-ду "Включить ПОДЗАРЯД АБ"
               delay_ms(5000);
           }
       }

L22:     if (PrPZ == 0){
           Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"
           Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"

           PrZ =  0;
           PrPZ =  1;
       }

       if (ZnTekI[359] == 0x31){ /*Uay [>37V; >1.59 V]*/
           Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ"
           goto L9;
       }

       if (ZnTekI[282] == 0x31){ /*DD1*/
           Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ"
 /*  сравнение предельных значений времен замкнутости ДД с текущими
                             значениями замкнутости ДД*/
           i =  1;
           do{
               if (ZnDD[i] == 0x31){
                     if (tZmkDD[i] > 1440){ 	//24 часа
                   //if (tZmkDD[i] > 5){
                       YNN(DDv);//Установка "DD" не норма
											 Sbros();

                       goto L9;
                   }
               }
               i++;
           }while(i < 25);

           goto L20;
       }
       else{
           Yst(VPZ);//Установить ком-ду "Включить ПОДЗАРЯД АБ"

           i = 1;
           do{
               tRDD[i] =  0;
               i++;
           }while(i < 25);
           SNN(DDv);//сброс "DD" не норма
           if (Pr24 == 0) goto L9;
       }
/*======+++++++++++++++*/


L20:  
       //md = port[0x0104];
//     delay_ms(10);
     //nd=  md & 0x0100;
		nd = (getDigitalInSPI(24) << 8);		
//+++++++++++TODO! !!!!!!!!!!!!! Для макета, потому что не исправен канал на DigIn !!!!!!!!!!
//		nd = (getDigitalInSPI(31) << 8);		
//--------------TODO! !!!!!!!!!!!!! Для макета, потому что не исправен канал на DigIn !!!!!!!!!!
     if (nd > 0){
         if ((PrSab == 0x31) || (PrSab == 0x32)) goto L1;
					if (PrIZ == 1){

         /*я┐╜я┐╜я┐╜я┐╜реня┐╜я┐╜"R я┐╜я┐╜." */
         Riz();
         //TODO1 по-молчанию все на выход, нужно ли менять? (есть ли ввод???)
         //port[AdrBA+3] = 0x0A;/*установка разъема J9 (каналы 23...47:A-вывод,В-ввод,
//                       С[0-3]-вывод,С[4-7]-ввод) CPU188R для режима Rиз.*/
         //port[AdrBA] =  0x00; /*установка регистра "А" выходов разъема J9 в "0"*/
                               /*Riz. норма*/
	  		 for (i=0;i<8;i++){
						setUNIO(24+i,0);}//сброс UNIO

         //TODO1
         if (ZnTekI[358] == 0x30){ 				
									SRNN();
                  YRN(); //Riz. норма с одновременоной выдачей сигнала "Режим Rиз. ВКЛЮЧЕН"
				 }
         else{
   							SRN();
                YRNN();//Riz. НЕ норма с одновременоной выдачей сигнала "Режим Rиз. ВКЛЮЧЕН"
                Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
                Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ"
         }
				 ZapRArx();
				 PrIZ = 0;
         goto L1;
			 }
					else goto L1;
     }
		 else PrIZ = 1;
		 
     goto L1;
     //break;

/*DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD*/
/*TOTOTOTOTOTOTOTOTOTOTOTOTOTOTOTOT*/
         /*Р е ж и м   "ТО"*/
         /*=================*/


     case 0x0400:   /*TO*/

          if (PrTO == 0){
              Sbr(VPZ);//Сброс ком-ды "Включить ПОДЗАРЯД АБ" 
              nTO =  0;
              /*сброс донесениЯ режима "Д"*/
              Sbr(rD);//сброс донесениЯ режима "Д"
              PrTO =  1;
              PrD  =  0;

              Yst(rTO);//установка донесения "TO включен"

              delay_ms(5000);

              Yst(VZar);//Установка ком-ды "Включить ЗАРЯД АБ" 
          }

L14:  /*Проверка ИСПРАВНОСТИ */
      nTO =  1;
      goto L3;
             /*Uzy*/
L54:  if (ZnTekI[355] == 0x31) goto L11; /*U37;U1.59;DD1*/

 if (ZnTekI[287] == 0x2D){
     /*З А Р Я Д*/

     PrR =  0;
     if (PrZ == 0){
            PrZ =  1;
            RDate1();
            Tz0 =  tVs;/*запоминаем время начала ЗAРЯДА*/

            t5M  =  tVs1 + 5;
            Pr5M =  1;
            t10M =  tVs1 +15;
            Pr1M =  0;
            t1M  =  tVs1 + 1;
     }

     /*Вычисляем время заряда*/
            RDate1();

            if (tVs1 > t5M) Pr5M =  0;

            Tzt = tVs;
            Tz1 = Tzt - Tz0;
            if (Tz1 < 0){
              Tz1 = (86400 -Tz0) + Tzt;
            }
   if (Tz1 > 10800) goto L11;
   else goto L50;

 }else goto L50;



L11:   Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ" 
      PrZ = 0;

      delay_ms(5000);
      Yst(VRaz);//Установка ком-ды "Включить РАЗРЯД АБ"

L13:  nTO =  2;
     goto L3;

             /*Uvy*/
L19:  if (ZnTekI[354] == 0x31) goto L40; /*U27;U0.5*/

 //old versino if ((ZnTekI[287] != 0x2D) && ((ZnTekI[287] != 0x30) && (ZnTekI[288] != 0x30))){
			if (PrSab == 0x32){

              /*Р А З Р Я Д*/
          if (PrR == 0){
               PrR =  1;
               PrZ =  0;
               RDate1();
               Tr0 =  tVs;/*запоминаем время начала РАЗРЯДА*/
               CRS =  0;/*"С разр." := 0*/

               t5M  =  tVs1 + 5;
               Pr5M =  1;
               t10M =  tVs1 +15;
               Pr1M =  0;
               t1M  =  tVs1 + 1;

          }

               SCraz(); /*считаем "С разр."*/

   /* Запись текущего значения разрядной емкости*/
//  Str(CRS:5:2,stroka);
  width = sprintf(stroka,"%5.2f\0",CRS);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
  if (width != 5){//я┐╜шибя┐╜я┐╜!!!
  }

  ZnTekI[384]= stroka[0];
  ZnTekI[385]= stroka[1];
  ZnTekI[386]= stroka[2];
  ZnTekI[387]= stroka[3];
  ZnTekI[388]= stroka[4];

  ZnArxI[212]= stroka[0];
  ZnArxI[213]= stroka[1];
  if (stroka[2] == '.') ZnArxI[214]= stroka[3];
  else ZnArxI[214]= stroka[2];


   /*i = 384;
   do{
   ZnTekIc[i] = ZnTekI[i];//TODO1 ???
   i++;
   }while(i < 389);

   i = 212;
   do{
   ZnArxIc[i] = ZnArxI[i];//TODO1 ???
   i++;
   }while(i < 215);*/

   RDate1();
   if (tVs1 > t5M) Pr5M =  0;

/*old version   if (CRS < 70) goto L50;
   else goto L15;*/
	 goto L50;
      }
      else goto L50;

L15:   Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ" 
      PrR = 0;

      delay_ms(1000);

      Yst(ORaz);//Установка ком-ды "Отключить РАЗРЯД АБ"
      delay_ms(50);
      Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"

      delay_ms(5000);

      Yst(VZar);//Установка ком-ды "Включить ЗАРЯД АБ"

L41:  nTO = 3;
     goto L3;

             /*Uzy*/
L42:  if (ZnTekI[355] == 0x31) goto L43; /*U37;U1.59;DD1*/

 if (ZnTekI[287] == 0x2D){

          /*З А Р Я Д*/

       PrR =  0;
       if (PrZ == 0){
            PrZ =  1;
            RDate1();
            Tz0 =  tVs;/*запоминаем время начала ЗAРЯДА*/

            t5M  =  tVs1 + 5;
            Pr5M =  1;
            t10M =  tVs1 +15;
            Pr1M =  0;
            t1M  =  tVs1 + 1;
       }

     /*Вычисляем время заряда*/
            RDate1();

            if (tVs1 > t5M) Pr5M =  0;

            Tzt = tVs;
            Tz1= Tzt - Tz0;
            if (Tz1 < 0){
              Tz1= (86400 -Tz0) + Tzt;
            }


   if (Tz1 > 10800) goto L43;
   else goto L50;

 }
 else goto L50;

L43: Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
     PrZ = 0;
     Sbr(rTO);//сброс донесения: ТО - ВКЛЮЧЕН
  /*Проверка снятия команды " ТО - ВКЛЮЧЕНО"*/
L46:  //TODO
     //m=port[AdrI2];
//     delay_ms(10);
     //n = m & 0x0400;
		n = (getDigitalInSPI(26) << 10);		
		if (n == 0x0400){
         delay_ms(10);
         goto L46;
     }
     goto L1;/*ВЫХОД из режима "ТО"*/

L40: /* if CRS < 1.0 then  */
     //old version if (CRS < 70){
		 RDate1();
     if ((tVs - Tr0) < 8400){
         PrDR =  0;
         goto L44;/*{при Сраз.< 70*/
     }
     else{
         PrDR =  0;
         i = 1;
         do{
             if (t24DD[i] >= 3600) goto L44;/*при Сраз.>70, но DDi >1 час*/
             i++;
         }while(i < 25);
         goto L15;
     }

L50:  if (Pr5M == 1){
         if (tVs1 >= (int)(t1M)){
                 // NomRArx();
                  WTekI();
                  ZapRArx();
                  t1M =  tVs1 + 1;
         }
         goto L51;
     }

     if (PrZ == 1){
         if (ZnUakb[1] >= 36.5){
L48:           if (Pr1M == 1){
L47:              if (tVs1 >= (int)(t1M)){
                     //NomRArx();
                     WTekI();
                     ZapRArx();
                     t1M =  tVs1 + 1;
                 }

                 if (ZnTekI[354] == 1) Pr1M = 0;
                 if (ZnTekI[359] == 1) Pr1M = 0;
                 goto L51;
             }
             else{
                 t1M =  tVs1 + 1;
                 Pr1M =  1;
                 goto L47;
             }
         }
         else{
L49:            if (tVs1 >= (int)(t10M)){
                      //NomRArx();
                      WTekI();
                      ZapRArx();
                      t10M = tVs1 + 10;

             }
             goto L51;
         }
     }
     else{
         if (PrR == 1){
             if (ZnUakb[1] <= 27.5) goto L48;
             else goto L49;
         }
         else goto L51;
     }
L51:  if (PrSab == 0x33){
         RDate1();
         if (Pr24 == 0){
            // NomRArx();
             WTekI();
             ZapRArx();
             Pr24 = 1;
             t24ch = tVs24;
         }
         else{
             if (t24ch == tVs24){
                 //NomRArx();
                 WTekI();
                 ZapRArx();
             }
         }
     }
                      switch (nTO){
                      case 1: goto L14; //break;
                      case 2: goto L13; //break;
                      case 3: goto L41; //break;
                      case 4: goto L58; //break;
                      case 5: goto L59; //break;
                      case 6: goto L65; //break;
                      }


L3:/*Проверка ИСПРАВНОСТИ */
     ZnU[27] =  0;
     //TODO1
     //port[AdAK] = 0x3A;
		 setUNIO(27,0);setUNIO(26,1);setUNIO(25,0);setUNIO(24,1);setUNIO(29,0);setUNIO(28,1);
     delay_ms(10);
     izmZn = ADC0();
		 ZnU[27] = izmZn;	//TODO!!! не работает 2ая МК АК => не мерим 5В контр
  if ((4.6 < ZnU[27]) && (ZnU[27] < 5.1)){
      YstISP();//ИСПРАВНО
  }
  else{
      SbrISP();//НЕИСПРАВНО
      //TODO1
      //HALT(0);
		return;
  }

/*   //формирование текущего числа съемов информации
   for(i=0;i<5;i++) //TODO1 нужно ли ZnTekIc
       str1TekI[i] = ZnTekI[755+i];//ZnTekIc[755+i];//str1TekI = ZnTekIc[755]+ZnTekIc[756]+ZnTekIc[757]+ZnTekIc[758]+ZnTekIc[759];
   str1TekI[5] = 0x00;
   //Val(str1TekI,wr,q);//wr - текущий № измерения в рабочем режиме
   wr = atoi(str1TekI);
   if (wr == 0){//я┐╜шибя┐╜я┐╜!

   }*/


     /*+++++++++++*/
L76: RDate2();/*запись текущего времени и даты в ZnTekI(c)*/
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
  KFUcht();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
    /*//измерение показаний датчиков давления и подготовка их значений для записи в файл "TekInf.txt"
    DtDv();//DD1,DD6

 //проверка состояния предохранителя и наличия тока КЗ и подготовка их значений для записи в файл "TekInf.txt"
    Prdh();

 //измерение показаний датчиков течи электролита и подготовка их значений для записи в файл "TekInf.txt"
    DTE();*/

 /*измерение напряжений и подготовка их значений для записи в файл "TekInf.txt"*/
    izmU();
    Uakb();
    Ubyte();
    GrU();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
  KFIIcht();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
/*измерение тока заряда/разряда и подготовка их значений для записи в файл "TekInf.txt"*/
delay_ms(30);	//перед измерением тока, ?чтобы успел установться?
    i = 1;
    do{//TODO1 ???
        izmZn = Read_ADC1();
        ZnPU[i] = izmZn;
        i++;
    }while(i < 11);

    z = 0;
    i = 1;
    do{
        z =  z + ZnPU[i];
        i++;
    }while(i < 11);

    izmZn =  z/10.;
			if (F_DEBUG){
				sprintf(kS,"%6.3f\0",izmZn);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
				sendDebug("T");sendDebug(kS);}
    DTOK(izmZn);
		
   //измерение показаний датчиков давления и подготовка их значений для записи в файл "TekInf.txt"
    DtDv();//DD1,DD6

 //проверка состояния предохранителя и наличия тока КЗ и подготовка их значений для записи в файл "TekInf.txt"
    Prdh();

 //измерение показаний датчиков течи электролита и подготовка их значений для записи в файл "TekInf.txt"
    DTE();

 /*запись в файл "TekInf.txt"*/
    WTekI();
    delay_ms(5);

 /*чтение из файла "TekInf.txt"*/
   //RTekI();

 /*Считаем "дельту" t (dtD)*/
    RDate1();/*Время в сек. с начала суток "tVs"*/
    VdtD(); /*подсчет периода опроса*/

 /*====================================*/

/*Анализ состояния ПР. i KZ*/
if  ((ZnTekI[356] == 0x30) && (ZnTekI[357] == 0x30)){
    SNN(PR); //Сброс НЕ НОРМА ПР.
}
else{
    YNN(PR);//Установка НЕ НОРМА ПР.

    Sbros();

    goto L1;
}
/*Анализ состояния ДТЭ*/
if (ZnTekI[294] == 0x30){
    SNN(DT); //Сброс НЕ НОРМА ДТЭ
}
else{
    YNN(DT);//Установка НЕ НОРМА ДТЭ

    Sbros();

    goto L1;
}


/*Анализ состояния датчиков ДД1 ...ДД24*/
if (ZnTekI[282] == 0x31){ /*DD1*/

        /*отсчитываем очередную порцию времени замкнутости ДД*/
                      i = 1;
                      j = 258;
                      do{
                          if (ZnTekI[j] == 0x31){
                              t24DD[i] =  t24DD[i] + dtDm;
                          }
                          j++;
                          i++;
                       }while(j < 282);

             /*Вывод текущего значения времени замкнутости ДД в мин. */

                       i = 1;
                       j = 0;
                       m = 0;
											 f = 0;
                       do{
                           tZmkDD[i] =  t24DD[i];
													 if (t24DD[i] >= 1440){ DD24[i] = 1; f = 1;}													 
													 else DD24[i] = 0;


                           //Str(tZmkDD[i]:6:1,stroka);
                           width = sprintf(stroka,"%6.1f\0",tZmkDD[i]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
                           if (width != 6){//я┐╜шибя┐╜я┐╜!!!
                           }

                           ZnTekI[411 + j] = stroka[0];
                           ZnTekI[412 + j] = stroka[1];
                           ZnTekI[413 + j] = stroka[2];
                           ZnTekI[414 + j] = stroka[3];
                           ZnTekI[415 + j] = stroka[4];
                           ZnTekI[416 + j] = stroka[5];

                           ZnArxI[116 + m] = stroka[0];
                           ZnArxI[117 + m] = stroka[1];
                           ZnArxI[118 + m] = stroka[2];
                           ZnArxI[119 + m] = stroka[3];

                           m = m + 4;
                           j = j + 6;
                           i++;
                       }while(i < 25);
										 }
 /*Анализ состояния ДД6*/
if (ZnTekI[283] == 0x30 && !f){
    SNN(DDv); //Сброс НЕ НОРМА ДД
}
else{
    YNN(DDv);//Установка НЕ НОРМА ДД
    Sbros();
    goto L1;
}

L55:    
		n = (getDigitalInSPI(24) << 8) | (getDigitalInSPI(25) << 9) | (getDigitalInSPI(26) << 10);		
                    //if ( n== 0x40) goto L77;
                      if (n != 0x0400) goto L78;

     switch (nTO) {
                      case 1: goto L54; //break;
                      case 2: goto L19; //break;
                      case 3: goto L42; //break;
                      case 4: goto L56; //break;
                      case 5: goto L60; //break;
                      case 6: goto L66; //break;
                      case 7: goto L69; //break;
                      case 8: goto L16; //break;
                      case 9: goto L70; //break;
                      }


L44:  /* Д О Р А З Р Я Д */
      Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ"

      nTO = 7;
      delay_ms(1000);

      Yst(ORaz);//Установка ком-ды "Отключить РАЗРЯД АБ" 
      delay_ms(50);
      Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"

      /*Сброс ком-ды "Отключить РАЗРЯД АБ"*/

L69: //old version if ((ZnTekI[287] == 0x2D) || ((ZnTekI[287] == 0x30) && (ZnTekI[288] == 0x30))) goto L3;
		if (PrSab == 0x32) goto L3;

    PrR = 0;
    Yst(VDR);//Установить ком-ду "Включить ДОРАЗРЯД АБ"
											
   /*Проверка установки команды "Включить ДОРАЗРЯД АБ"*/
L45: 
 //   m = portw[AdrI2];
    //delay_ms(10);
    //n = m & 0x0800;
		n = (getDigitalInSPI(27) << 11);
    if (n != 0x0800) {
        delay_ms(10);
        goto L45;
    }
    if (PrDR == 0){
        RDate1();
        PrDR = 1;
        nTO = 8;
        Tdz0 = tVs;/*запоминаем время начала ДОРАЗРЯДА*/
        nTO =  8;
        goto L3;
L16:    // NomRArx(); /*однократная запись в архив*/
        WTekI();
        ZapRArx();
    }
L52:       delay_ms(1000);
          RDate1();
          Tdzt = tVs;
          Tdz1 = Tdzt - Tdz0;
          if (Tdz1 < 0){
               Tdz1 = (86400 -Tdz0) + Tdzt;
          }
          /*Проверка ДОРАЗРЯДА на 12 часов*/
          //if (Tdz1 < 1200) goto L52;	//для отладки 20 минут
           if (Tdz1 < 43200) goto L52;	//12 частов
           else{
               PrDR =  0;
               nTO =  9;
               Sbr(VDR);//Сброс ком-ды "Включить ДОРАЗРЯД АБ"
               goto L3;
L70:          //  NomRArx(); /*однократная запись в архив*/
               WTekI();
               ZapRArx();
         /*Проверка сброса команды "Включить ДОРАЗРЯД АБ"*/
L53: 
               //m = port[AdrI2];
     //          delay_ms(10);
       //        n = m & 0x0800;
         n = (getDigitalInSPI(27) << 11);		
						 if (n == 0x0800){
                   delay_ms(100);
                   goto L53;
               }
               else{
                   delay_ms(1000);
                   Yst(VZar);//Установка ком-ды "Включить ЗАРЯД АБ"
               }
           }

/*==============================================*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*==============================================*/

/*Проверка ИСПРАВНОСТИ */
L58:   nTO =  4;
      goto L3;
             /*Uzy*/
L56:  if (ZnTekI[355] == 0x31) goto L57; /*U37;U1.59;DD1*/

     if (ZnTekI[287] == 0x2D){

          /*З А Р Я Д*/
         PrR =  0;
         if (PrZ == 0){
             PrZ =  1;
             RDate1();
             Tz0 =  tVs;/*запоминаем время начала ЗAРЯДА*/

             t5M  =  tVs1 + 5;
             Pr5M =  1;
             t10M =  tVs1 +15;
             Pr1M =  0;
             t1M  =  tVs1 + 1;
         }

     /*Вычисляем время заряда*/
         RDate1();

         if (tVs1 > t5M) Pr5M =  0;

         Tzt = tVs;
         Tz1= Tzt - Tz0;
         if (Tz1 < 0){
             Tz1= (86400 -Tz0) + Tzt;
         }
         if (Tz1 > 10800) goto L57;
         else goto L50;
     }
     else goto L50;


L57:   Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
      PrZ = 0;

      delay_ms(5000);
      Yst(VRaz);//Установка ком-ды "Включить РАЗРЯД АБ" 

L59:  nTO =  5;
     goto L3;

             /*Uvy*/
L60:  if (ZnTekI[354] == 0x31) goto L61; /*U27;U0.5*/

     //old version if ((ZnTekI[287] != 0x2D) && ((ZnTekI[287] != 0x30) && (ZnTekI[288] != 0x30))){
		 if (PrSab == 0x32){

               /*Р А З Р Я Д*/

          if (PrR == 0){
              PrR = 1;
              PrZ = 0;
              RDate1();
              Tr0 = tVs;/*запоминаем время начала РАЗРЯДА*/
              CRS = 0;/*"С разр." := 0*/

              t5M  =  tVs1 + 5;
              Pr5M =  1;
              t10M =  tVs1 +15;
              Pr1M =  0;
              t1M  =  tVs1 + 1;

          }

          SCraz(); /*считаем "С разр."*/

   /* Запись текущего значения разрядной емкости*/
//          Str(CRS:5:2,stroka);
          width = sprintf(stroka,"%5.2f\0",CRS);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
          if (width != 5){//я┐╜шибя┐╜я┐╜!!!
          }

          ZnTekI[384] = stroka[0];
          ZnTekI[385] = stroka[1];
          ZnTekI[386] = stroka[2];
          ZnTekI[387] = stroka[3];
          ZnTekI[388] = stroka[4];

          ZnArxI[212]= stroka[0];
          ZnArxI[213]= stroka[1];
          if (stroka[3] == '.') ZnArxI[214] = stroka[3];
          else ZnArxI[214] = stroka[2];

/*          i = 384;
          do{
              ZnTekIc[i] = ZnTekI[i];//TODO1 ???
              i++;
          }while(i < 389);

          i = 212;
          do{
              ZnArxIc[i] = ZnArxI[i];//TODO1 ???
              i++;
          }while(i < 215);*/

          RDate1();
          if (tVs1 > t5M) Pr5M =  0;

          //old versino if (CRS < 70) goto L50; else goto L63;
					goto L50;
      }
      else goto L50;

L63:   Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ"
      PrR = 0;

      delay_ms(1000);

      Yst(ORaz);//Установка ком-ды "Отключить РАЗРЯД АБ"
      delay_ms(50);
      Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"

      delay_ms(5000);

      Yst(VZar);//Установка ком-ды "Включить ЗАРЯД АБ" 


L65:  nTO =  6;
     goto L3;

             /*Uzy*/
L66:  if (ZnTekI[355] == 0x31) goto L67; /*U37;U1.59;DD1*/

      if (ZnTekI[287] == 0x2D){

          /*З А Р Я Д*/
          PrR=  0;
          if (PrZ == 0){
              PrZ =  1;
              RDate1();
              Tz0 =  tVs;/*запоминаем время начала ЗAРЯДА*/

              t5M  =  tVs1 + 5;
              Pr5M =  1;
              t10M =  tVs1 +15;
              Pr1M =  0;
              t1M  =  tVs1 + 1;
          }

     /*Вычисляем время заряда*/
          RDate1();

          if (tVs1 > t5M) Pr5M =  0;

          Tzt = tVs;
          Tz1= Tzt - Tz0;
          if (Tz1 < 0){
              Tz1= (86400 -Tz0) + Tzt;
          }


          if (Tz1 > 10800) goto L67;
          else goto L50;

       }
      else goto L50;

L67:  Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
     PrZ = 0;
     /*сброс донесения: ТО - ВКЛЮЧЕН*/
     Sbr(rTO);//сброс донесения: ТО - ВКЛЮЧЕН
  /*Проверка снятия команды " ТО - ВКЛЮЧЕНО"*/
L68:  
 //    m = port[AdrI2];
//     delay_ms(10);
//     n = m & 0x0400;
			 n = (getDigitalInSPI(26) << 10);		
     if (n == 0x0400){
         delay_ms(10);
         goto L68;
     }
     else goto L1;/*ВЫХОД из режима "ТО"*/

L61: RDate1();
		 //old version if (CRS < 1.0){
		 if ((tVs - Tr0) < 8400){
    /* if CRS < 70 then */
         PrDR =  0;
         goto L62;/*при Сраз.< 70*/
     }
     else{
         PrDR =  0;
         i= 1;
         do{
             if (t24DD[i] >= 3600) goto L62;/*при Сраз.>70, но DDi >1 час*/
             i++;
         }while(i < 25);
         goto L63;
     }

L62:  Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
     PrR = 0;
     Sbr(rTO);//сброс донесения: ТО - ВКЛЮЧЕН
     YNN(Cr); //установка "НЕ НОРМА Сраз.","НЕ НОРМА АБ"
     Sbros();

  /*Проверка снятия команды " ТО - ВКЛЮЧЕНО"*/
L64:  
 //     m = port[AdrI2];
     //delay_ms(10);
     //n = m & 0x0400;
		 n = (getDigitalInSPI(26) << 10);		
     if (n == 0x0400){
         delay_ms(100);
         goto L64;
     }
     else goto L1;/*ВЫХОД из режима "ТО"*/

//break;

     /*   end;  */

	 
/*TOTOTOTOTOTOTOTOTOTOTOTOTOTOTOTOT*/


     case 0x0000:
     case 0x0100:
     case 0x0500:
     case 0x0600:
     case 0x0700:
     /*case 0x90:
     case 0xA0:
     case 0xB0:
     case 0xC0:
     case 0xD0:
     case 0xE0:
     case 0xF0:*/
L78:     Sbr(rD);
         Sbr(rTO);
         SRNN();
         SRN();         

         PrR   =  0;
         PrZ   =  0;
         PrPZ  =  0;
         PrD   =  0;
         PrTO  =  0;
         PrRiz =  0;
         PrA   =  0;

         Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"}
         Sbros();

           /*Анализ признака РАБОТЫ с ПЭВМ*/
  PC();
  delay_ms(10);
  if  (ZnTekI[399] == 0x31) goto L71;/*работаем с ПЭВМ*/

  goto L1;
//break;
}
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*Т Е Х Н О Л О Г И Ч Е С К А Я        Ч А С Т Ь*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

L71:  /*Работа с ПК*/
 //???   begin
  /*  RTekI;     */
    /*ssVixA3 = 0;
    ssVixA4 = 0;*/
  /*Исходная установка признаков */

    PrR   =  0;
    PrAA  =  0;
    PrZ   =  0;
    Pr1Min=  0;
    Pr0   =  0;
    Pr00  =  0;
    PrSab =  0;
    t90   =  0;
/*++++++++++++++++++*/
/*++++++++++++++++++*/

  /*Обнуление архивной строки кроме № съема инфр.*/
/*  i=  1;
  do{
  ZnArxIc[i] = '0';
  i=  i + 1;
  }while(i < 250);

  i=  1;
  do{
  ZnArxI[i] = 0x30;
  i=  i + 1;
  }while(i < 250);     */

L73:

     /*Прием команд от оператора*/
     /*Формирование ZnX[1...KT]*/
		//TODO1
    //ZnX[1]= Lo(portw[AdrRY+6]);
/* do{ //TODO1
        //c = (Lo(portw[AdrRY + 2] & RxD));
    }while(c > 0);*/
	memset(ZnX, '0', 129);
		ZnX[1] = GetByte(AdrRY);
		i = 2;
L74:  Rbyte = GetByte(AdrRY);
     ZnX[i] =  Rbyte;
     i++;
  if (Rbyte != KT) goto L74;
  /*while (1) {
	  Rbyte = GetByte(AdrRY);
	  if (Rbyte == KT) break;
	  ++i;
  }*/

  int j = i - 1;
  while (j > 0 && !isalpha(ZnX[j])) --j;
  d =  ZnX[j];
  for (int i = 0; i < 256; ++i) ZnArxI[i] = i;

  ZapRArx();
  switch (d){
  case 0x52 : goto L75; //break;/*R - вывод текущего времени */
  case 0x4E : goto L23; //break;/*N - заводской номер*/
  case 0x4D : goto L24; //break;/*M - запрос на чтение зав.№ и даты изготовл. при запуске*/
  case 0x44 : goto L25; //break;/*D - установка даты и времени в часах RTC*/
  case 0x57 : goto L26; //break;/*W - установка даты изготовления*/
  case 0x41 : goto L27; //break;/*A - запрос на обновление информации на экране из файла TekInf.txt */
  case 0x5A : goto L28; //break;/*Z - измерение Rиз.*/
  case 0x58 : goto L29; //break;/*X - вывод архива*/
	case 0x45 : goto L1; //break;/*E - выход из технологического режима*/
	case 0x46 : if (ZnX[2] == 0x31) /*F - включение/выключение режима отладки => посылка отладочной информации*/
								F_DEBUG = 1;
							else F_DEBUG =0;
							break; //break;
  case 0x47 : goto L38; //break;/*G - вывод рабочего архива*/
  case 0x4A : if (ZnX[2] == 0x31){ /*J - изменить номера архивов, '1' - номер рабочего архива, '2' - номер архива испытаний*/
								adrArxR = (ZnX[3] - 0x30) * 1000 + (ZnX[4] - 0x30) * 100 + (ZnX[5] - 0x30) * 10 + (ZnX[6] - 0x30);
								ZnTekI[755] = '0'; ZnTekI[756] = ZnX[3]; ZnTekI[757] = ZnX[4]; ZnTekI[758] = ZnX[5]; ZnTekI[759] = ZnX[6];
							}
							else if (ZnX[2] == 0x32){ /*J - изменить номера архивов, '1' - номер рабочего архива, '2' - номер архива испытаний*/
								w = adrArxI = (ZnX[3] - 0x30) * 1000 + (ZnX[4] - 0x30) * 100 + (ZnX[5] - 0x30) * 10 + (ZnX[6] - 0x30);
								ZnTekI[760] = '0'; ZnTekI[761] = ZnX[3]; ZnTekI[762] = ZnX[4]; ZnTekI[763] = ZnX[5]; ZnTekI[764] = ZnX[6];
							}
							break; //break;
  case 0x4B :  /*K - запросить номера архивов*/
							ZnS[2] = adrArxR / 1000 + 0x30;
							ZnS[3] = (adrArxR % 1000) / 100 + 0x30;
							ZnS[4] = (adrArxR % 100) / 10 + 0x30;
							ZnS[5] = (adrArxR % 10) + 0x30;
							ZnS[6] = 'I';
							ZnS[7] = '=';
							ZnS[8] = adrArxI / 1000 + 0x30;
							ZnS[9] = (adrArxI % 1000) / 100 + 0x30;
							ZnS[10] = (adrArxI % 100) / 10 + 0x30;
							ZnS[11] = (adrArxI % 10) + 0x30;
							ZnS[12] = KT;
							ZnS[13] = 0;
	ZnS[0] = 'R';ZnS[1] = '=';
							sendDebug(ZnS);
							break;
  case 0x54 : goto L30; //break;/*T - обнуление времени замкнутости ДД*/
  case 0x59 : goto L39; //break;/*Y - вывод последнего № съема инф-ции рабочего режима*/
  case 0x43 :

        PrAA = 1;
	sendDebug("Cmd C");
	goto L72;
      //  goto L72;/*C - запрос на однократное обновление информации на экране из файла TekInf.txt */
        //break;
  case 0x42 : /*B - стирание раб. архива и № := 0 */

			EraseRabArx();

      /*Обнуление № съема информации*/
      ZnTekI[755] = 0x21;
      ZnTekI[756] = 0x21;
      ZnTekI[757] = 0x21;
      ZnTekI[758] = 0x21;
      ZnTekI[759] = 0x30;

/*      ZnTekIc[755] = ' ';	//TODO1 ???
      ZnTekIc[756] = ' ';
      ZnTekIc[757] = ' ';
      ZnTekIc[758] = ' ';
      ZnTekIc[759] = '0';*/

      WTekI();

      ZnS[1]=  0x48;/*H*/
      ZnS[2]=  KT;
      for (i=1;i<=2;i++) SendByte(AdrRY,ZnS[i]);
      goto L37;
      //break;

   case 0x51 :   /*Q-стирание файла "Archive.dat"*/
      EraseArx();

      /*Обнуление № съема информации*/
			w = 0;
			adrArxI = 0;
      ZnTekI[760] = 0x20;
      ZnTekI[761] = 0x20;
      ZnTekI[762] = 0x20;
      ZnTekI[763] = 0x20;
      ZnTekI[764] = 0x30;

      /*ZnTekIc[760] = ' ';	//TODO1 ???
      ZnTekIc[761] = ' ';
      ZnTekIc[762] = ' ';
      ZnTekIc[763] = ' ';
      ZnTekIc[764] = '0';*/

      WTekI();

      ZnS[1] =  0x4F;/*O*/
      ZnS[2] =  KT;
      for (i=1;i<=2;i++) SendByte(AdrRY,ZnS[i]);
      goto L37;
      //break;

   case 0x56 : /*V - начать вывод архива испытаний*/
           PrAA = 1;
           goto L37;
           //break;

   case 0x53 : /*S - закончить вывод архива испытаний*/
           PrAA = 0;
           goto L37;
           //break;

   case 0x50 : /*P - сброс текущего номера архива испытаний*/

           i =  1;
           do{
               ZnTekI[i + 759] =  0x20;
               i++;
           }while(i < 5);
           ZnTekI[764] = 0x30;

           /*i =  1;	//TODO1 ???
           do{
               ZnTekIc[i + 759]=  ' ';
               i++;
           }while(i < 5);
           ZnTekIc[764] =  '0';*/

           w =  0;
					 adrArxI = 0;

           WTekI();

           goto L37;
           //break;

					 //kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
      case 0x4C: //L - ввод коэффициентов по НАПРЯЖЕНИЮ
          i = 2;
          m = 1;
          do{
						kU[m] = ZnX[i];
						//kUc[m] = chr(kU[m]);
						i++;
						m++;
					}while(m < 121);
						
           KFUzap();

           KFUcht();

           break;

     case 0x49: //I- ввод коэффициентов по ТОКУ}

          i = 2;
          m = 1;
          do {
						kI[m] = ZnX[i];
						//kIc[m]= chr(kI[m]);
						i++;
						m++;
          }while (m < 11);

          KFIzap();
          //RTekI();
          KFIIcht();
     break;
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
      }//switch (d)
     goto L37;

L27:  /*+++++++++++*/
  if (Pr0 == 0){
/*      i =  1;
      do{
          ZnArxIc[i + 250] =  '0';	//TODO1 ???
          i++;
      }while(i < 6);*/

      i =  1;
      do{
          ZnArxI[i + 250] =  0x30;
          i++;
      }while(i < 6);
  }

           /*формирование исходного числа съемов информации*/
L72:
  for(i=0;i<5;i++)	//TODO1 ???
      str1TekI[i] = ZnTekI[760+i];//ZnTekIc[755+i];//str1TekI = ZnTekIc[755]+ZnTekIc[756]+ZnTekIc[757]+ZnTekIc[758]+ZnTekIc[759];
  //str1TekI[5] = 0x00;	
  //  if (w == 0){//я┐╜шибя┐╜я┐╜!

//  Val(str1TekI,w,q);
  w = atoi(str1TekI);
  //}

     /*+++++++++++*/
    RDate2();/*запись текущего времени*/
    /*измерение показаний датчиков давления
      и подготовка их значений для записи в файл "TekInf.txt"*/
    //DtDv();/*DD1,DD4*/
 /*проверка состояния предохранителя и наличия тока КЗ
  и подготовка их значений для записи в файл "TekInf.txt"*/
	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
  KFUcht();
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
    //Prdh();
 /*измерение напряжений и подготовка их значений для записи в файл "TekInf.txt"*/
    izmU();
			sendDebug("izmEnd");
    Uakb();
    Ubyte();
    GrU();
 /*измерение тока заряда/разряда и подготовка их значений для записи в файл "TekInf.txt"*/
    i = 1;
    do{
        izmZn = Read_ADC1();
        //HAL_Delay(100);
        ZnPU[i] =  izmZn;
        i++;
    }while(i < 11);
    z = 0;
    i = 1;
    do{
        z =  z + ZnPU[i];
        i++;
    }while(i < 11);

    izmZn =  z/10.;

    DTOK(izmZn);
		DtDv();//DD1,DD4
 //проверка состояния предохранителя и наличия тока КЗ и подготовка их значений для записи в файл "TekInf.txt"
    Prdh();
 /*измерение показаний датчиков течи электролита
   и подготовка их значений для записи в файл "TekInf.txt"*/
    DTE();
 /*запись в файл "TekInf.txt"*/
//    WTekI();
 //   delay_ms(550);
 /*чтение из файла "TekInf.txt"*/
    //RTekI();// TODO отладка RTekI();

 /*Считаем "дельту" t (dtD)*/
    RDate1();
    VdtD();

 /*====================================*/
 /*====================================*/
    Pr0 = 1;

 /*Проверка ИСПРАВНОСТИ */
     ZnU[27] =  0;
     //port[AdAK] = 0x3A;
		 setUNIO(27,0);setUNIO(26,1);setUNIO(25,0);setUNIO(24,1);setUNIO(29,0);setUNIO(28,1);
     delay_ms(500);
     izmZn = ADC0();
     ZnU[27] = izmZn;	//TODO!!! не работает 2ая МК АК => не мерим 5В контр
  if ((4.6 < ZnU[27]) && (ZnU[27] < 5.1)){
      YstISP(); //ИСПРАВНО
  }
  else{
      SbrISP();//НЕИСПРАВНО
			WTekI();
      goto L37;
  }
/*Анализ состояния ПР. i KZ*/
if ((ZnTekI[356] == 0x30) && (ZnTekI[357] == 0x30)){
    SNN(PR); //Сброс НЕ НОРМА ПР.
}
else{
    YNN(PR);//Установка НЕ НОРМА ПР.
    goto L31;
}
/*Анализ состояния ДТЭ*/
if (ZnTekI[294] == 0x30){
    SNN(DT); //Сброс НЕ НОРМА ДТЭ
}
else{
    YNN(DT);//Установка НЕ НОРМА ДТЭ
    goto L31;
}


/*Анализ состояния датчиков ДД1 ...ДД24*/
if (ZnTekI[282] == 0x31){  /*DD1*/

        if (ZnTekI[300] == 0x31){/*если ЗАРЯД*/
					Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
					Yst(VPZ); //Установить ком-ду "Включить ПОДЗАРЯД АБ"
        }
        /*отсчитываем очередную порцию времени замкнутости ДД*/
L32:     i = 1;
        j = 258;
        do{
            if (ZnTekI[j] == 0x31){
                t24DD[i] =  t24DD[i] + dtDm;
            }
            j++;
            i++;
        }while(j < 282);

        /*проверяем время замкнутости DDi*/
        i = 1;
        do{
            if (t24DD[i] > 90){ /*> 90 min*/
                YNN(DDv);//Установка "DD" не норма
                t90 =  1;
            }

            i++;
        }while(i < 25);
        /*Вывод текущего значения времени замкнутости ДД в мин. */
        /*отсчитываем очередную порцию времени замкнутости ДД в мин.*/
        i = 1;
        j = 0;
        m = 0;
				f = 0;
        do{
            tZmkDD[i] =  t24DD[i];
						if (t24DD[i] >= 1440){ DD24[i] = 1; f = 1;}
						else DD24[i] = 0;

            //Str(tZmkDD[i]:6:1,stroka);
            width = sprintf(stroka,"%6.1f",tZmkDD[i]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
            if (width != 6){//я┐╜шибя┐╜я┐╜!!!
            }

                       ZnTekI[411 + j] = stroka[0];
                       ZnTekI[412 + j] = stroka[1];
                       ZnTekI[413 + j] = stroka[2];
                       ZnTekI[414 + j] = stroka[3];
                       ZnTekI[415 + j] = stroka[4];
                       ZnTekI[416 + j] = stroka[5];

                       ZnArxI[116 + m] = stroka[0];
                       ZnArxI[117 + m] = stroka[1];
                       ZnArxI[118 + m] = stroka[2];
                       ZnArxI[119 + m] = stroka[3];

                       m = m + 4;
                       j = j + 6;
                       i++;
                       }while(i < 25);

                        /*i = 411;
                         do{
                            ZnTekIc[i] = ZnTekI[i];//TODO1 ???
                            i++;
                         }while(i < 555);

                         i = 116;
                         do{
                             ZnArxIc[i] = ZnArxI[i];//TODO1 ???
                             i++;
                         }while(i < 212);*/
                         Pr0 = 1;
}

/*Анализ состояния ДД6*/
if (ZnTekI[283] == 0x30 && !f){
    SNN(DDv);//Сброс НЕ НОРМА ДД
}
else{
    YNN(DDv);//Установка НЕ НОРМА ДД
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
                      /*РАЗРЯД*/
L33: if (t90 == 0){
        ZnTekI[360] = 0x30;
 //       ZnTekIc[360] = '0';//TODO1 ???
    }
    else{
        ZnTekI[360] = 0x31;
 //       ZnTekIc[360] = '1';//TODO1 ???
    }
    if (ZnTekI[300] == 0x32){
   			Sbr(VPZ); //Сброс ком-ды "Включить ПОДЗАРЯД АБ"}
		   	Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
        if (PrR == 0){
            PrR =  1;
            PrZ = 0;
            PrPZ = 0;
/*запоминаем время и дату начала РАЗРЯДА в ячейках с 601 по 610*/
            RDate2();
            i = 401;
            j = 601;
            do{
                ZnTekI[j] =  ZnTekI[i];
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
                i++;
            }while(i < 411);
            j = 611; /*сброс времени и даты конца РАЗРЯДА*/
            do{
                ZnTekI[j] =  0x30;
                //ZnTekIc[j] = ZnTekI[j];//TODO1???
                j++;
            }while(j < 621);

            RDate1();
            Tr0 = tVs;
            CRS = 0;/*"С разр." := 0*/
        }

        SCraz(); /*считаем "С разр."*/
        /* Запись текущего значения разрядной емкости*/
        //Str(CRS:5:2,stroka);
        width = sprintf(stroka,"%5.2f\0",CRS);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
        if (width != 5){//я┐╜шибя┐╜я┐╜!!!
        }
        ZnTekI[384] = stroka[0];
        ZnTekI[385] = stroka[1];
        ZnTekI[386] = stroka[2];
        ZnTekI[387] = stroka[3];
        ZnTekI[388] = stroka[4];

        ZnArxI[212] = stroka[0];
        ZnArxI[213] = stroka[1];
        if (stroka[2] == '.') ZnArxI[214] = stroka[3];
        else ZnArxI[214] = stroka[2];


        /*i = 384;
        do{
            ZnTekIc[i] = ZnTekI[i];//TODO1 ???
            i++;
        }while(i < 389);

        i = 212;
        do{
            ZnArxIc[i] = ZnArxI[i];//TODO1 ???
            i++;
        }while(i < 215);*/

        if (ZnTekI[354] == 0x30){ /*Uvy*/
								 SNN(Cr);  //Сброс "С разр." не норма
                 Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"
                 Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
                 SNN(PR);  //Сброс "Пр." не норма"
            goto L31;
        }
        else{
            if (CRS < 70){
                YNN(Cr);//Установка "С разр." не норма
L34:             Yst(ORaz);//Установить ком-ду "Отключить РАЗРЯД АБ"

     /*запоминаем время и дату отключения РАЗРЯДА в ячейках с 611 по 620*/
                RDate2();

                i = 401;
                j = 611;
                do{
                    ZnTekI[j] =  ZnTekI[i];
                    //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                    j++;
                    i++;
                }while(i < 411);

                goto L31;
            }
            else{
                SNN(Cr);//Сброс "С разр." не норма
                goto L34;
            }
        }

    } /*РАЗРЯД закончился*/
    else{
        if (PrR == 1){
            if (ZnTekI[300] != 0x33){
                PrR=  0;
                CRS =  0;
            }
            Sbr(ORaz);//Сброс ком-ды "Отключить РАЗРЯД АБ"
        }
    }
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
                      /*ЗAРЯД*/
    if (ZnTekI[300] == 0x31){
        if (PrZ == 0){
            PrZ = 1;
            PrR = 0;
            PrPZ = 0;
            RDate2(); /*запоминаем время и дату начала ЗAРЯДА*/
            i = 401;
            j = 601;
            do{
                ZnTekI[j] =  ZnTekI[i];
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
                i++;
            }while(i < 411);
            j = 611; /*сброс времени и даты конца ЗAРЯДА*/
            do{
                ZnTekI[j] =  0x30;
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
            }while(j < 621);

            RDate1();
            Tz0 =  tVs;
            CZS =  0;/*"С зар." := 0*/
        }

        SCzar(); /*считаем "С зaр."*/

  /* Запись текущего значения зарядной емкости*/
  //Str(CZS:5:2,stroka);
  width = sprintf(stroka,"%5.2f\0",CZS);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
  if (width != 5){//я┐╜шибя┐╜я┐╜!!!
  }
  ZnTekI[389] = stroka[0];
  ZnTekI[390] = stroka[1];
  ZnTekI[391] = stroka[2];
  ZnTekI[392] = stroka[3];
  ZnTekI[393] = stroka[4];

  ZnArxI[212] = stroka[0];
  ZnArxI[213] = stroka[1];
  if (stroka[2] == '.') ZnArxI[214] = stroka[3];
  else ZnArxI[214] = stroka[2];

  /*i = 389;
  do{
      ZnTekIc[i] = ZnTekI[i];//TODO1 ???
      i++;
  }while(i < 394);

  i = 212;
  do{
      ZnArxIc[i] = ZnArxI[i];//TODO1 ???
      i++;
  }while(i < 215);*/

  if (ZnTekI[355] == 0x31){ /*Uzy:37V,1.59V,DD1=1*/
L35:   Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"}
		   Yst(VPZ);//Установить ком-ду "Включить ПОДЗАРЯД АБ"
      goto L31;
  }
  else{
      RDate1();
      Tzt = tVs;
      Tz1= Tzt - Tz0;
      if (Tz1 < 0){
          Tz1= (86400 -Tz0) + Tzt;
      }
      /* > 3-х часов*/
      if (Tz1 > 10800) goto L35;
      else{
          Sbr(VPZ); //Сброс ком-ды "Включить ПОДЗАРЯД АБ"
		   		Yst(VZar);//Установить ком-ду "Включить ЗАРЯД АБ"
          goto L31;
      }
  }
    }
    else{/*ЗАРЯД закончился*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
        if (PrZ == 1){
            if (PrSab != 0x33) PrZ = 0;
            RDate1();
            TzK =  tVs;

            PrgZ =  TzK - Tz0;
            if (PrgZ < 0){
                PrgZ = (86400 -Tz0) + TzK;
            }
            x =  PrgZ/3600.;
   /*Вывод полного времени ЗAРЯДА в часах*/
  //Str(x:5:2,stroka);
  width = sprintf(stroka,"%5.2f\0",x);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
  if (width != 5){//я┐╜шибя┐╜я┐╜!!!
  }
  ZnTekI[379] = stroka[0];
  ZnTekI[380] = stroka[1];
  ZnTekI[381] = stroka[2];
  ZnTekI[382] = stroka[3];
  ZnTekI[383] = stroka[4];
  /*i = 379;
  do{
      ZnTekIc[i]=  ZnTekI[i];//TODO1 ???
      i++;
  }while(i =<384);*/

        }
    }
    /*ПОДЗАРЯД*/
    if (ZnTekI[300] == 0x33){
        if (PrPZ == 0){
            PrPZ = 1;

            RDate2(); /*запоминаем время и дату начала ПОДЗAРЯДА*/
            i = 401;
            j = 601;
            do{
                ZnTekI[j] =  ZnTekI[i];
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
                i++;
            }while(i < 411);

            j = 611; /*сброс времени и даты конца ПОДЗAРЯДА*/
            do{
                ZnTekI[j] =  0x30;
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
            }while(j < 621);
        }
    }
    else{
        if (PrPZ == 1){
            PrPZ = 0;
            RDate2(); /*запоминаем время и дату конца ПОДЗAРЯДА*/
            i = 401;
            j = 601;
            do{
                ZnTekI[j] =  ZnTekI[i];
                //ZnTekIc[j] = ZnTekI[j];//TODO1 ???
                j++;
                i++;
            }while(i < 411);
        }
    }
    if ((PrR == 0) && (PrZ == 0)){
        i = 384;
        do{
            ZnTekI[i] =  0x30;
            //ZnTekIc[i] =  '0'; /*обнуление "С" зар./разр*///TODO1 ???
            i++;
        }while(i < 394);

        /*i = 212;
        do{
            ZnArxIc[i] =  '0'; //обнуление "С" зар./разр//TODO1 ???
            i++;
        }while(i < 215);*/
    }
    goto L31;/*вывод на экран значений Uai и Uab    */
//}

L75:  RDate();
    goto L37;

L25:  YstDate();
    YstTime();
    goto L75;

L28:  Riz();
    WTekI();
  /*+++++++++++++++++++++++++++++++++*/
  /*чтение из файла "TekInf.txt"*/
    //RTekI();

     i = 1;
     do{
         ZnUTekI[i] = ZnTekI[i];
         i++;
     }while(i < 766);
     ZnUTekI[765] = KT;
     /*вывод данных для ПЭВМ по Riz*/
     for(i=1;i<=765;i++) SendByte(AdrRY,ZnUTekI[i]);
     i = 1;
     do{
         ZnUTekI[i]=  0;
         i++;
     }while(i < 766);

  /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*вывод данных для КИП(САЭ) по Riz*/
     if (ZnTekI[358] == 0x30){
			 SRNN();
			 YRN();//Riz. норма
     }
     else{
       SRN();
			 YRNN();//Riz. НЕ норма 
     }
     goto L37;

L23: ZapZN();
    //WTekI();
    //RTekI();
    FZavN();

    for(i=1;i<=12;i++) SendByte(AdrRY,ZnS[i]);

    goto L37;

L24:  //RTekI();
     FND();

     for(i=1;i<=20;i++) SendByte(AdrRY,ZnS[i]);
     goto L37;

L26:  ZapDI();
     WTekI();
     //RTekI();
     FDIzg();

     for(i=1;i<=15;i++) SendByte(AdrRY,ZnS[i]);
     RTekI();
     goto L37;


L29:  ChtArx();

     goto L37;

L31:  //WTekI();
      //RTekI();
      if (PrAA == 0){WTekI(); goto L36;}
      else{
          NomZapArx();
//				delay_ms(200);
          ZapArx();

				WTekI();
							sendDebug("WTelIEnd");
				//RTekI();
          goto L36;
      }

L36:
		 i=1;
     do{
          ZnUTekI[i] = ZnTekI[i];
					if (ZnUTekI[i] == 0x03) 
						ZnUTekI[i] = 0x99;	//экранируем символ КТ
          i++;
     }while(i < 765);
     ZnUTekI[765] = KT;
		 ZnUTekI[1] = 0x99;ZnUTekI[2] = 0x99;ZnUTekI[3] = 0x99;ZnUTekI[4] = 0x99;

//for(i=0;i<1;i++)SendByte(AdrRY,i+10);
	HAL_Delay(4000);
     for(i=1;i<=765;i++){ SendByte(AdrRY,ZnUTekI[i]);
			 //delay_ms(1);
		 }
//SendByte(AdrRY,0x03);SendByte(AdrRY,0x03);SendByte(AdrRY,0x03);SendByte(AdrRY,0x03);SendByte(AdrRY,0x03);

     i = 1;
     do{
         ZnUTekI[i] =  0;
         i++;
     }while(i < 766);

     goto L37;


L30:     /*Обнуление времен замкнутости ДД */
     i =  1;
     do{
         t24DD[i] = 0;
         i++;
     }while(i < 25);

     /*Обнуление замкнутости ДД*/
     i =  1;
		 //++++++вариант с "  0.0" в строке времи замкнутости ДД
     do{
         ZnTekI[410+i] = 0x20;
         ZnTekI[411+i] = 0x20;
         ZnTekI[412+i] = 0x20;
         ZnTekI[413+i] = 0x30;
         ZnTekI[414+i] = 0x2E;
         ZnTekI[415+i] = 0x30;
         i+=6;
     }while(i < 145);
		 //++++++вариант с "000000" в строке времи замкнутости ДД
/*     do{
         ZnTekI[410+i] = 0x30;
         i++;
     }while(i < 145);*/
		 //------
     /*i = 0;
     do{
         ZnTekIc[411+i] = ' ';	//TODO1 ???
         ZnTekIc[412+i] = ' ';
         ZnTekIc[413+i] = ' ';
         ZnTekIc[414+i] = '0';
         ZnTekIc[415+i] = '.';
         ZnTekIc[416+i] = '0';
         i = i + 6;
     }while(i <= 138);*/

     /*обнуление ДД6 и t > 90 мин.*/
     //ZnTekIc[283] =  '0';//TODO1 ???
     //ZnTekIc[360] =  '0';//TODO1 ???
     ZnTekI[283] =  0x30;
     ZnTekI[360] =  0x30;
     t90 =  0;
		 WTekI();//ddddddddddddddddddd
     goto L37;

L37:   /*Анализ признака РАБОТЫ с ПЭВМ*/
     PC();
     delay_ms(1);                             /*работаем с ПЭВМ*/
     if (ZnTekI[399] == 0x30) goto L1;
     else goto L73;

L38:  ChtRArx();
     goto L37;

L39: // NomRArx();
     //NRA();
     goto L37;
}
