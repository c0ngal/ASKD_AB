#include "ServiceFunctions.h"
#include "MemoryFunctions.h"
#include "math.h"
#include "RTC.h"
#include "ADC.h"
#include "UART.h"
#include "UNIO.h"
#include "global.h"

//========================================================
/*Преобразование ДТОК в ток, а затем в символы и запись их в
                            массивы ZnTekIc(ZnTekI)*/

  
	/*Чтение dTok из файла dTok.txt и преобразование его в real*/
// void RdTok(){
////  char [] strDtok;
//  Assign(txtDtok,'dTok.txt');
//     /*0xI-*/
//     Reset(txtDtok);
//     /*0xI+*/
//     ReadLn(txtDtok,str1TekI);

//  for i= 1 to 6 do ZnDtokc[i] =  str1TekI[i];
//  for i= 1 to 255 do str1TekI[i]=  ' ';

//  Close(txtDtok);

// strDtok =  ZnDtokc[1]+ZnDtokc[2]+ZnDtokc[3]+ZnDtokc[4]+ZnDtokc[5]; /*+ZnDtokc[6];*/

// Val(strDtok,t,q);
//}

void DTOK(float x){

//   char ks1 [10];	//формат СИ
   //uint16_t r;
	//kkkkkkkkkkkkkkkk
 float y1,z1;
//kkkkkkkkkkkkkkkk
	 int width = 0;
//TODO
//    RdTok();

/*+++++++++++++++++++++++++++++++*/
    x = x / 0.00775;
    if ((fabs(x) < 10.) && (fabs(x) > 1.)) x = 1;
  // x= x - t;    
    y = x;
    z = x;

      /*ЗАРЯД с маленьким током ==> П О Д З А Р Я Д*/
    if (fabs(x) < 2){

                                PrSab = 0x33;
                                PrIo  = 0x30;

//                                Str(x:6:3,stx);/*Преобразование действит.числа в его строковое представление в символах*/
                                width = sprintf(kS,"%6.3f\0",x);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
                                if (width != 6){//Ошибка!!!
                                }
																
																if (F_DEBUG){
																	sendDebug("Tok");sendDebug(kS);
																}

                                ZnTekI[287] = kS[0];
                                ZnTekI[288] = kS[1];
                                ZnTekI[289] = kS[2];
                                ZnTekI[290] = kS[3];
                                ZnTekI[291] = kS[4];
                                ZnTekI[292] = kS[5];

                                ZnArxI[12] = kS[1];
                                ZnArxI[13] = kS[2];
                                ZnArxI[14] = kS[3];
                                ZnArxI[15] = kS[5];

                                if (ZnTekI[287] == '-') PrIo = 0x31;

                                Tok = x;

                                goto L1;
    }

         /*ЗАРЯД*/
    if ((y < 0) && (fabs(y) >= 2)){

                                PrSab = 0x31;
                                y = y + kZr;
                                Tok = y;
//                                Str(y:6:3,stx);/*Преобразование действит.числа в его строковое представление в символах*/
                                width = sprintf(kS,"%6.3f\0",y);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
                                if (width != 6){//ошибка!!!
                                }

                                ZnTekI[287] = kS[0];
                                ZnTekI[288] = kS[1];
                                ZnTekI[289] = kS[2];
                                ZnTekI[290] = kS[3];
                                ZnTekI[291] = kS[4];
                                ZnTekI[292] = kS[5];

																y1 = (int)(y + 0.5);//округление до целого
                                //Str(y1:5:1,ks1);
																width = sprintf(kS,"%5.1f\0",y1);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
                                if (width != 5){//ошибка!!!
                                }

                                ZnArxI[12] = kS[1];
                                ZnArxI[13] = kS[2];
                                ZnArxI[14] = kS[3];
                                ZnArxI[15] = kS[4];

                                goto L1;
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
    }
		

          /*РАЗРЯД*/
    if ((z > 0) && (z >= 2)){

                                PrSab = 0x32;

                                z = z - kRr;

                                Tok = z;													
                                //Str(z:6:3,stx);/*Преобразование действит.числа в его строковое представление в символах*/
                                width = sprintf(kS,"%6.3f\0",z);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
                                if (width != 6){//ошибка!!!
                                }
                                ZnTekI[287] = kS[0];
                                ZnTekI[288] = kS[1];
                                ZnTekI[289] = kS[2];
                                ZnTekI[290] = kS[3];
                                ZnTekI[291] = kS[4];
                                ZnTekI[292] = kS[5];

                                z1 = (int)(z + 0.5);//округление до целого
//                                Str(z1:5:1,ks1);
																width = sprintf(kS,"%5.1f\0",z1);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
                                if (width != 5){//ошибка!!!
                                }

                                ZnArxI[12] = kS[0];
                                ZnArxI[13] = kS[1];
                                ZnArxI[14] = kS[2];
                                ZnArxI[15] = kS[3];	//TODO спросить , этот символ - '.', м.б. преобразование д.б. 4:1, а не 5:1


                                goto L1;
    }

L1:   ZnTekI[300] =  PrSab;
     //ZnTekIc[300] =  ZnTekI[300];	//TODO ???
     ZnArxI[1] =  PrSab;
  //   ZnArxIc[1] =  ZnArxI[1];		//TODO ???
     ZnTekI[394] =  PrIo;
//     ZnTekIc[394] =  ZnTekI[394];		//TODO ???

/*  r = 287;
  do{
  ZnTekIc[r] = ZnTekI[r];	//TODO ???
  r++;
  }while(r < 296);

  r = 12;
  do{}
  ZnArxIc[r] = ZnArxI[r];	//TODO ???
  r++;
  }while(r < 16);*/
 }

 /*========================================================*/
/* Установка даты в часах RTC*/
 void YstDate(){

    //regs:  registers;
	int index = 1;
	while (ZnX[index] != 'D') ++index;
	++index;
    //a = ZnX[2];
	a = ZnX[index++];
    a = a & 0x0F;
//    a = a << 4;
    //c = ZnX[3];
    c = ZnX[index++];
    c = c & 0x0F;
    dd = a*10 + c; //a | c;
    //send (0x07) для dd

    //a = ZnX[4];
    a = ZnX[index++];
    a = a & 0x0F;
    //a = a << 4;
    //c = ZnX[5];
    c = ZnX[index++];
    c = c & 0x0F;
    dm = a*10 + c; //a | c;
    //send (0x09) для dm

    //a = ZnX[6];
    a = ZnX[index++];
    a = a & 0x0F;
    //a = a << 4;
    //c = ZnX[7];
    c = ZnX[index++];
    c = c & 0x0F;
    dg = a*10 + c; //a | c;
    //send (0x0D) для dg
		setDate(dg,dm,dd);	//год указывается только последние две цифры	
}
/*========================================================*/
/* Установка времени в часах RTC*/
 void YstTime(){

	int index = 1;
	while (ZnX[index] != 'D') ++index;
	index += 7;
    //a = ZnX[8];
	a = ZnX[index++];
    a = a & 0x0F;
    //a = a << 4;
    //c = ZnX[9];
    c = ZnX[index++];
    c = c & 0x0F;
    tch = a*10 + c; //a | c;
    //send (0x05) для tch

    //a = ZnX[10];
    a = ZnX[index++];
    a = a & 0x0F;
    //a = a << 4;
    //c = ZnX[11];
    c = ZnX[index++];
    c = c & 0x0F;
    tm = a*10 + c; //a | c;
    //send (0x05) для tm
    //ts  = 0x30;//??? точно????
	 ts	= 30;	//секунды не устанавливаются, поэтому ставим половину минуты?
    //send (0x05) для ts ???
	 setTime(tch,tm,ts);
  //      инициализация RTC                       */

}
/*=======================================================*/
/*  Считывание информации с RTC и
  формирование массива данных о текущем времени*/
 void RDate(){
 /*считываемые данные в формате BCD, затем они переводятся в условный
  BCD-формат (данные только в 1-ом полубайте),из условного BCD-формата  переводятся в КОИ-7*/
  uint8_t ch1,ch2,mn1,mn2,d1,d2,m1,m2,g1,g2,sh1,sh2; //byte; /*переменные текущих времени и даты*/
  unsigned short TVm ;/*текущее время в мин.текущего съема*/
	 int width = 0;

  uint16_t p,q,s,t;
  float m,n;
	uint16_t i = 0;

  d1 = getDay() / 10; /*десятки дней*/
  d2 = getDay() % 10;; /*единицы дней*/

  m1 = getMonth() / 10; /*десятки месяцев*/
  m2 = getMonth() % 10; /*единицы месяцев*/

  g1 = getYear() / 10; /*десятки лет*/
  g2 = getYear() % 10; /*единицы лет*/


  //send (0x85)
  ch1 = getHours() / 10; /*десятки часов*/
  ch2 = getHours() % 10; /*единицы часов*/

  //send (0x83)
  mn1 = getMinutes() / 10;/*десятки минут*/
  mn2 = getMinutes() % 10; /*единицы минут*/

  //send (0x81)
  sh1 = getSeconds() / 10; /*десятки секунд*/
  sh2 = getSeconds() % 10; /*единицы секунд*/


  p = ch1/* - '0'*/;
  q = ch2/* - '0'*/;
  p = p * 10;
  p = p + q;
  p = p * 60;

  s = mn1/* - '0'*/;
  t = mn2/* - '0'*/;
  s = s * 10;
  s = s + t;

  TVm = p + s;

  tVs = TVm * 60;
  m = sh1*10;
  n = m+sh2;
  tVs = tVs+n;/*текущее время в сек.(1...86399)[в пределах суток]*/

  d1  = d1 | 0x30;
  d2  = d2 | 0x30;
  m1  = m1 | 0x30;
  m2  = m2 | 0x30;
  g1  = g1 | 0x30;
  g2  = g2 | 0x30;
  ch1  = ch1 | 0x30;
  ch2  = ch2 | 0x30;
  mn1  = mn1 | 0x30;
  mn2  = mn2 | 0x30;

    ZnS[12] = ch1;
    ZnS[13] = ch2;
    ZnS[15] = mn1;
    ZnS[16] = mn2;
    ZnS[2] = d1;
    ZnS[3] = d2;
    ZnS[5] = m1;
    ZnS[6] = m2;
    ZnS[8] = g1;
    ZnS[9] = g2;
    ZnS[1] = 0x54;/*T*/
    ZnS[4] = 0x2E;/*.*/
    ZnS[7] = 0x2E;/*.*/
    ZnS[10] = 0x20;/* */
    ZnS[11] = 0x20;/* */
    ZnS[14] = 0x2D;/*-*/
    ZnS[17] = 0x20;/* */
    ZnS[18] = 0x20;/* */
    ZnS[19] = 0x20;/* */
    ZnS[24] = KT;
    /*преобразование текущего времени в минутах (word) в символы (byte)*/
    //Str(TVm:4,stroka);
    width = sprintf(stroka,"%4d\0",TVm);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
    if (width != 4){//ошибка!!!
    }
    ZnS[20] = stroka[0];
    ZnS[21] = stroka[1];
    ZnS[22] = stroka[2];
    ZnS[23] = stroka[3];

    for (i=1;i<=24;i++) SendByte(AdrRY,ZnS[i]);
}
/*========================================================*/
/* Считывание информации с RTC и
  формирование массива данных о текущем времени*/
 void RDate2(){
 /*считываемые данные в формате BCD, затем они переводятся в условный
  BCD-формат (данные только в 1-ом полубайте),из условного BCD-формата
  переводятся в КОИ-7*/
  //uint16_t  p,q,s,t;
  //float m,n;
  uint8_t ch1,ch2,mn1,mn2,d1,d2,m1,m2,g1,g2/*,sh1,sh2*/; //byte; /*переменные текущих времени и даты*/
  //unsigned short TVm ;/*текущее время в мин.текущего съема*/


  d1 = getDay() / 10; /*десятки дней*/
  d2 = getDay() % 10;; /*единицы дней*/

  m1 = getMonth() / 10; /*десятки месяцев*/
  m2 = getMonth() % 10; /*единицы месяцев*/

  g1 = getYear() / 10; /*десятки лет*/
  g2 = getYear() % 10; /*единицы лет*/


  //send (0x85)
  ch1 = getHours() / 10; /*десятки часов*/
  ch2 = getHours() % 10; /*единицы часов*/

  //send (0x83)
  mn1 = getMinutes() / 10;/*десятки минут*/
  mn2 = getMinutes() % 10; /*единицы минут*/

  //send (0x81)
  //sh1 = getSeconds() / 10; /*десятки секунд*/
  //sh2 = getSeconds() % 10; /*единицы секунд*/

   d1  = d1 | 0x30;
    d2  = d2 | 0x30;
    m1  = m1 | 0x30;
    m2  = m2 | 0x30;
    g1  = g1 | 0x30;
    g2  = g2 | 0x30;
    ch1  = ch1 | 0x30;
    ch2  = ch2 | 0x30;
    mn1  = mn1 | 0x30;
    mn2  = mn2 | 0x30;

    ZnTekI[401] = d1;
    ZnTekI[402] = d2;
    ZnTekI[403] = m1;
    ZnTekI[404] = m2;
    ZnTekI[405] = g1;
    ZnTekI[406] = g2;
    ZnTekI[407] = ch1;
    ZnTekI[408] = ch2;
    ZnTekI[409] = mn1;
    ZnTekI[410] = mn2;

    /*r = 401;
    do{
        ZnTekIc[r] = ZnTekI[r];	//TODO ???
        r++;
    }while(r < 411);*/

    ZnArxI[2] = d1;
    ZnArxI[3] = d2;
    ZnArxI[4] = m1;
    ZnArxI[5] = m2;
    ZnArxI[6] = g1;
    ZnArxI[7] = g2;
    ZnArxI[8] = ch1;
    ZnArxI[9] = ch2;
    ZnArxI[10] = mn1;
    ZnArxI[11] = mn2;

    /*r = 2;
    do{
        ZnArxIc[r] = ZnArxI[r];	//TODO
        r+;
    }while(r < 12);*/
}
/*========================================================*/
/*Считывание информации с RTC и
  формирование массива данных о текущем времени без вывода по RS-232*/
 void RDate1(){
 /*считываемые данные в формате BCD, затем они переводятся в условный  BCD-формат
  (данные только в 1-ом полубайте),из условного BCD-формата  переводятся в КОИ-7*/
  //uint32_t p,q,s,t,m,n;
  uint8_t ch1,ch2,mn1,mn2,/*d1,d2,m1,m2,g1,g2,*/sh1,sh2; //byte; //переменные текущих времени и даты
//  unsigned short TVm ;//текущее время в мин.текущего съема


  //d1 = getDay() / 10; /*десятки дней*/
  //d2 = getDay() % 10;; /*единицы дней*/

  //m1 = getMonth() / 10; /*десятки месяцев*/
  //m2 = getMonth() % 10; /*единицы месяцев*/

  //g1 = getYear() / 10; /*десятки лет*/
  //g2 = getYear() % 10; /*единицы лет*/


  //send (0x85)
  ch1 = getHours() / 10; /*десятки часов*/
  ch2 = getHours() % 10; /*единицы часов*/

  //send (0x83)
  mn1 = getMinutes() / 10;/*десятки минут*/
  mn2 = getMinutes() % 10; /*единицы минут*/

  //send (0x81)
  sh1 = getSeconds() / 10; /*десятки секунд*/
  sh2 = getSeconds() % 10; /*единицы секунд*/

  tVs = (ch1*10 + ch2)*3600 + (mn1*10 + mn2)* 60  + sh1*10 + sh2;
  tVs1 = tVs/60;
  tVs24 = (int)(tVs/10);

}
/*========================================================*/
  /*Подсчет периода опроса ["дельты Т"]*/
 void VdtD(){
  tD1 = tVs;
  if (Pr0 == 0) tD2 = tVs;
  dtD = tD1 - tD2;

  if (dtD <= 0) dtD = 10;/*10  сек. - период опроса*/
  tD2 = tVs;
  dtDch = dtD/3600; /*dtD - в час.*/
  dtDm = dtD/60; /*dtD - в мин.*/
 }
/*==============================================*/
 /*подсчет "С разр."*/
 void SCraz(){
 float Cr;/*дельта "С разр."*/
 float iRaz;/*текущий ток разряда*/

   iRaz = Tok;/*A*/
   Cr = iRaz * dtDch;
   CRS = CRS + Cr;  /*A*час*/
 }
/*==============================================*/
 /*подсчет "С зар."*/
 void SCzar(){

 float Cz;  /*дельта "С зар."*/
 float iZar;/*текущий ток зaряда*/

   iZar = Tok;/*A*/
   Cz = iZar * dtDch;
   CZS = CZS + fabs(Cz);  /*A*час*/
 }
/*==============================================*/
 /*Измерение полных напряжений АБ и Ак[i]*/
 void izmU(){

  uint8_t a;
  uint16_t j,i;
//TODO ??? по умолчанию на передачу
// port[AdAK+3] = 0x00;//установка разъема J8 (каналы 0...23) CPU188R
  //             на "передачу" для управления аналоговым коммутатором
	 //Без мультиплексера
// delay_ms(5);

 for (i=1;i<=32;i++) ZnU[i]=  0;
 i = 0;
// a = 0x00;
	 //устанавливаем изначально на первый выход (нулевой выход I0 первой МК АК)
 setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,0);setUNIO(29,1);setUNIO(28,0);//setUNIO(29,1);setUNIO(28,0);
    delay_ms(2);
 do{
     
   // port[AdAK] = 0x20 + a;
    switch (i){
    		case 1: setUNIO(24, 1); break; // 1000
    		case 2: setUNIO(24, 0); setUNIO(25, 1); break; //0100
    		case 3: setUNIO(24, 1); break; // 1100
    		case 4: setUNIO(26, 1); setUNIO(24, 0); setUNIO(25, 0); break; //0010
    		case 5: setUNIO(24, 1); break; //1010
    		case 6: setUNIO(25, 1); setUNIO(24, 0); break; //0110
    		case 7: setUNIO(24, 1); break; //1110
    		case 8: setUNIO(27, 1); setUNIO(26, 0);setUNIO(25, 0);setUNIO(24, 0);break; // 0001
    		case 9: setUNIO(24, 1); break; //1001
    		case 10: setUNIO(25, 1); setUNIO(24, 0); break; // 0101
    		case 11: setUNIO(24, 1); break; //1101
    		case 12: setUNIO(26, 1); setUNIO(25, 0); setUNIO(24, 0); break;//0011
    		case 13: setUNIO(24, 1); break; //1011
    		case 14: setUNIO(24, 0); setUNIO(25, 1); break; //0111
    		case 15: setUNIO(24, 1); break; //1111
    		case 16: setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,0); setUNIO(29,0);setUNIO(28,1); break;
    		case 17: setUNIO(24, 1); break; // 1000
    		case 18: setUNIO(24, 0); setUNIO(25, 1); break; //0100
    		case 19: setUNIO(24, 1); break; // 1100
    		case 20: setUNIO(26, 1); setUNIO(24, 0); setUNIO(25, 0); break; //0010
    		case 21: setUNIO(24, 1); break; //1010
    		case 22: setUNIO(25, 1); setUNIO(24, 0); break; //0110
    		case 23: setUNIO(24, 1); break; //1110
    		case 24: setUNIO(27, 1); setUNIO(26, 0);setUNIO(25, 0);setUNIO(24, 0);break; // 0001
    		case 25: setUNIO(24, 1); break; //1001
    		case 26: setUNIO(25, 1); setUNIO(24, 0); break; // 0101
    		case 27: setUNIO(24, 1); break; //1101
    		case 28: setUNIO(26, 1); setUNIO(25, 0); setUNIO(24, 0); break;//0011
    		case 29: setUNIO(24, 1); break; //1011
    		case 30: setUNIO(24, 0); setUNIO(25, 1); break; //0111
    		case 31: setUNIO(24, 1); break; //1111

			/*case 1:
			case 3: 
			case 5: 
			case 7: 
			case 9: 
			case 11: 
			case 13: 
			case 15: 
			case 17: 
			case 19: 
			case 21: 
			case 23: 
			case 25: 
			case 27: 
			case 29: 
			case 31: setUNIO(27,1); break;
			case 2:
			case 6:
			case 10:
			case 14:
			case 18:
			case 22:
			case 26:
			case 30: setUNIO(27,0);setUNIO(26,1); break;
			case 4:
			case 12:
			case 20:
			case 28: setUNIO(27,0);setUNIO(26,0);setUNIO(25,1); break;
			case 8:
			case 24: setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,1); break;
			case 16: setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,0); setUNIO(29,0);setUNIO(28,1); break;*/
    }
    delay_ms(80);
		i++;//дальше работаем со счетчиком в формате Pascal (от 1цы)

    /*switch (i){
    case 8:
    case 14:
    case 20:
        delay_ms(750);
    }*/
    //делаем 20 измерений	//TODO!!!! для отладки только 5, в итоге 7
	izmZn = ADC0(); //1
	/*HAL_Delay(1);
	izmZn += ADC0(); //2
	HAL_Delay(1);
	izmZn += ADC0(); // 3
	HAL_Delay(1);
	izmZn += ADC0(); // 4*/



    /*j = 1;
    do{
        izmZn = ADC0();
        //HAL_Delay(100);
        ZnUU[j] = fabs(izmZn);
        j++;
    //}while(j < 6);
    //}while(j < 21);
    }while(j < 8);

    x = 0;
    j = 1;
    do{
        x = x + ZnUU[j];
        j++;
//    }while(j < 6);
//    }while(j < 21);
    }while(j < 8);
    //izmZn = x/5.;
    //izmZn = x/20.;
    izmZn = x/7.;
	*/

	//izmZn /= 4.;

    if (i>=1 && i < 8)
			izmZn = izmZn * 4.00f;//7.;//4.02;//4.01	//TODO!!! другой мультиплексер
    else if (i < 14)
        izmZn =  izmZn * 3.00f;//5.;//3.01;//3.00	//TODO!!! другой мультиплексер
    else if (i < 20)
        izmZn = izmZn * 2.00f;//3.;//2.00;//2.00	//TODO!!! другой мультиплексер

    ZnU[i] = fabs(izmZn);
//    a = a + 0x01;
    //i++; //уже прибавили раньше
 }while(i < 29);
 //TODO!!!! для отладки только 2
// }while(i < 16);
 //Без мультиплексера
/*  ZnU[2] = 29.61;
   ZnU[3] = 28.40;
   ZnU[4] = 27.21;
   ZnU[5] = 26.00;
   ZnU[6] = 24.82;
   ZnU[7] = 23.60;
   ZnU[8] = 22.42;
   ZnU[9] = 21.20;
   ZnU[10] =20.00;
   ZnU[11] =18.80;
   ZnU[12] =17.63;
   ZnU[13] =16.40;
   ZnU[14] =15.23;
   ZnU[15] =14.00;
   ZnU[16] =12.80;
   ZnU[17] =9.00;
   ZnU[18] = 8.00;
   ZnU[19] = 6.90;
   ZnU[20] = 6.00;
   ZnU[21] = 5.00;
   ZnU[22] = 4.00;
   ZnU[23] = 3.00;
   ZnU[24] = 2.05;
   ZnU[25] = 1.00;
   ZnU[26] = 1.00;
   ZnU[27] = 5.00;
   ZnU[28] = 0.60;*/
 }
/*==============================================*/
/*Вычисление истинных значений напряжений Ак[i] + полное Uab*/
 void Uakb(){
	 //Без мультиплексера
	  uint16_t j,i = 2;
    float d;
	 
    //uint16_t i = 2;
    do{
        ZnUakb[i]= ZnU[i] - ZnU[i+1];// + 0.01;
        i++;
    }while(i < 25);

//    ZnUakb[1] = ZnUakb[1] - 0.00;
//    ZnUakb[2] = ZnUakb[2] - 0.00;/*АК № 1*/
//    ZnUakb[3] = ZnUakb[3] - 0.00;/*АК № 2*/
//    ZnUakb[4] = ZnUakb[4] - 0.00;/*АК № 3*/
//    ZnUakb[5] = ZnUakb[5] - 0.00;/*АК № 4*/
//    ZnUakb[6] = ZnUakb[6] - 0.00;/*АК № 5*/
//    ZnUakb[7] = ZnUakb[7] - 0.00;/*АК № 6*/
//    ZnUakb[8] = ZnUakb[8] - 0.00;/*АК № 7*/
//    ZnUakb[9] = ZnUakb[9] - 0.00;/*АК № 8*/
//    ZnUakb[10] = ZnUakb[10] - 0.00;/*АК № 9*/
//    ZnUakb[11] = ZnUakb[11] - 0.00;/*АК № 10*/
//    ZnUakb[12] = ZnUakb[12] - 0.00;/*АК № 11*/
//    ZnUakb[13] = ZnUakb[13] - 0.00;/*АК № 12*/
//    ZnUakb[14] = ZnUakb[14] - 0.00;/*АК № 13*/
//    ZnUakb[15] = ZnUakb[15] - 0.00;/*АК № 14*/
//    ZnUakb[16] = ZnUakb[16] - 0.00;/*АК № 15*/
//    ZnUakb[17] = ZnUakb[17] - 0.00;/*АК № 16*/
//    ZnUakb[18] = ZnUakb[18] - 0.00;/*АК № 17*/
//    ZnUakb[19] = ZnUakb[19] - 0.00;/*АК № 18*/

//    ZnUakb[25] = ZnU[25] + 0.00; /*U ak #24*/*/
		ZnUakb[2] = ZnUakb[2] + kUr[1];/*АК № 1*/
    ZnUakb[3] = ZnUakb[3] + kUr[2];/*АК № 2*/
    ZnUakb[4] = ZnUakb[4] + kUr[3];/*АК № 3*/
    ZnUakb[5] = ZnUakb[5] + kUr[4];/*АК № 4*/
    ZnUakb[6] = ZnUakb[6] + kUr[5];/*АК № 5*/
    ZnUakb[7] = ZnUakb[7] + kUr[6];/*АК № 6*/
    ZnUakb[8] = ZnUakb[8] + kUr[7];/*АК № 7*/
    ZnUakb[9] = ZnUakb[9] + kUr[8];/*АК № 8*/
    ZnUakb[10] = ZnUakb[10] + kUr[9];/*АК № 9*/
    ZnUakb[11] = ZnUakb[11] + kUr[10];/*АК № 10*/
    ZnUakb[12] = ZnUakb[12] + kUr[11];/*АК № 11*/
    ZnUakb[13] = ZnUakb[13] + kUr[12];/*АК № 12*/
    ZnUakb[14] = ZnUakb[14] + kUr[13];/*АК № 13*/
    ZnUakb[15] = ZnUakb[15] + kUr[14];/*АК № 14*/
    ZnUakb[16] = ZnUakb[16] + kUr[15];/*АК № 15*/
    ZnUakb[17] = ZnUakb[17] + kUr[16];/*АК № 16*/
    ZnUakb[18] = ZnUakb[18] + kUr[17];/*АК № 17*/
    ZnUakb[19] = ZnUakb[19] + kUr[18];/*АК № 18*/
    ZnUakb[20] = ZnUakb[20] + kUr[19];/*АК № 19*/
    ZnUakb[21] = ZnUakb[21] + kUr[20];/*АК № 20*/
    ZnUakb[22] = ZnUakb[22] + kUr[21];/*АК № 21*/
    ZnUakb[23] = ZnUakb[23] + kUr[22];/*АК № 22*/
    ZnUakb[24] = ZnUakb[24] + kUr[23];/*АК № 23*/
    ZnUakb[25] = ZnU[25] + kUr[24]; /*U ak № 24*/

//без мультиплексера
    //ZnUakb[1] = ZnU[1];/*U аб*/
		i = 2;
    d = 0;
    do{
			d = d + ZnUakb[i];
			i++;
    }while(i<26);
    ZnUakb[1] = d + 1.2;//TODO!!! при отладке не хватает d;также изменено в Riz() //U  ab
		
    ZnUakb[27] = ZnU[27]; /*+5 V контр.*/
    ZnUakb[28] = ZnU[28];/* delta Uo  */

 }
/*==============================================*/
/*Преобразование напряжений АБ и Ак[i] из real в строку в byte*/
 void Ubyte(){
 int width = 0;
 uint16_t t,s/*,r,q*/;
 char stx[10];	//формат СИ
//TODO1
 ZnUakb[1] = ((float)((int)(ZnUakb[1]*100.)))/100.; /*округление до 100-ых*/

  s = 1;
  t = 5;
  do{
//  Str(ZnUakb[s]:6:3,stx);
      int width = sprintf(stx,"%6.3f\0",ZnUakb[s]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
		if (width != 6){//ошибка!!!
      }
      ZnTekI[t]  = stx[0];
      ZnTekI[t+1] = stx[1];
      ZnTekI[t+2] = stx[2];
      ZnTekI[t+3] = stx[3];
      ZnTekI[t+4] = stx[4];
      ZnTekI[t+5] = stx[5];
      t = t + 10;
      s++;
  }while(t < 255);

//  Str(ZnUakb[1]:6:3,stx);
  width = sprintf(stx,"%6.3f\0",ZnUakb[1]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
  if (width != 6){//я┐╜шибя┐╜я┐╜!!!
  }
  ZnArxI[16] = stx[0];
  ZnArxI[17] = stx[1];
  ZnArxI[18] = stx[3];
  ZnArxI[19] = stx[4];
   /*r = 1;
   do{
       ZnArxIc[r+15]= ZnArxI[r+15];	//TODO1 ???
       r++;
   }while(r < 5);*/

  s = 2;
  t = 20;
  do{
//  Str(ZnUakb[s]:6:3,stx);
      int width = sprintf(stx,"%6.3f\0",ZnUakb[s]);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 6, я┐╜я┐╜сле я┐╜я┐╜я┐╜я┐╜той - 3 я┐╜я┐╜я┐╜я┐╜я┐╜, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
      if (width != 6){//ошибка!!!
      }
      ZnArxI[t]   = stx[1];
      ZnArxI[t+1] = stx[3];
      ZnArxI[t+2] = stx[4];
      t = t + 3;
      s++;
  }while(s < 26);

/*  q = 1;
  do{
      ZnTekIc[q] = ZnTekI[q];	//TODO ???
      q++;
  }while(q < 256);*/

   /*r = 1;
   do{
       ZnArxIc[r+19] = ZnArxI[r+19];	//TODO ???
       r++;
   }while(r < 73);*/
}

/*==============================================*/
/*Анализ граничных напряжений АБ и Ак[i] и выработка признаков отключения АБ*/
/*   с записью всех признаков в массивы ZnTekI (ZnTekIc) */
/*Um = 1 ==>отключить АБ от внешней цепи*/
/*Ub = 1 ==>отключить АБ от ЗУ*/
 void GrU(){

 uint16_t t,s/*,r*/;

   if ((ZnUakb[1] > 37.0) || (ZnUakb[1] == 37.0)) U37 =  0x31;
   else U37 = 0x30;
   if ((ZnUakb[1] < 27.0) || (ZnUakb[1] == 27.0)) U27 =  0x31;
   else U27 = 0x30;
    s=1;
    t=2;
  do{
   if ((ZnUakb[t] > 1.59) || (ZnUakb[t] == 1.59)){

         w159[t]=  w159[t] + 1;
         switch (w159[t]){
         case 1: U16[s]= 0x30; break;
         case 2: U16[s]= 0x30; break;
         case 3: U16[s]= 0x31; break;
         }
   }
   else{
       w159[t]= 0;
       U16[s]= 0x30;
   }

  if ((ZnUakb[t] < 0.5) || (ZnUakb[t] == 0.5)) U05[s] = 0x31;
                  else U05[s]= 0x30;
   t++;
   s++;
  }while(s < 25);

   s = 1;
   t = 0;
   do{
   if (U16[s] == 0x31) t= 1;
   s++;
   }while (s < 25);
   if ((U37 == 0x31) || (t == 1 ) || (DD1 == 0x31)) Uzy = 0x31;
                  else Uzy = 0x30;
   if ((U37 == 0x31) || (t == 1 )) Uay = 0x31;
   else Uay = 0x30;

   s = 1;
   t = 0;
   do{
   if (U05[s] == 0x31) t = 1;
   s++;
   }while(s < 25);
   if ((U27 == 0x31) | (t == 1 )) Uvy = 0x31;
   else Uvy = 0x30;

   ZnTekI[352] = U37;
   ZnTekI[353] = U27;
   ZnTekI[354] = Uvy;
   ZnTekI[355] = Uzy;
   ZnTekI[359] = Uay;

   s = 1;
   do{
   ZnTekI[s+303] = U05[s];
   s++;
   }while(s < 25);
   s = 1;
   do{
   ZnTekI[s+327] = U16[s];
   s++;
   }while(s < 25);

   /*s = 293;
   do{
   ZnTekIc[s] = ZnTekI[s];	//TODO ???
   s++;
   }while(s < 361);*/
}
/*==============================================*/
/*Обработка показаний датчиков давления и формирование признаков*/
 void DtDv(){
 //uint8_t a,b/*,c,d*/,f;
 uint16_t r,t;

	 //old version
  //a = port[0x0100];/*DD 1...8*/
  //b = port[0x0104];/*DD 9...12*/
  //c = port[0x0110];/*DD 13...20*/
  //d = port[0x0114];/*DD 21...24*/
	 
  //a = portw[AdrI1];{DD 1...16}
  //b = portw[AdrI2];{DD 17...24}
/*   r = 1;
   do{
   f = a & 0x0001;
   ZnDDl[r] = f;
   a = a >> 1;
   r++;
   }while(r < 17);
   r = 17;
   do{
   f = b & 0x0001;
   ZnDDl[r] = f;
   b = b >> 1;
   r++;
   }while(r < 25);*/
	 
	 for (r=0;r<25;r++){
		 ZnDDl[r+1] = getDigitalInSPI(r);
	 }
	 
/*    r=13;
   do{
   f = c & 0x01;
   ZnDD[r] = f;
   c = c >> 1;
   r++;
   }while(r < 21);
   r = 21;
   do{
   f = d & 0x01;
   ZnDD[r] = f;
   d = d >> 1;
   r++;
   }while(r < 25);*/
//анализ состояний ДД
   r = 1;
   DD1 = 0x30;
   do{
   if (ZnDDl[r] == 0x0001) DD1 = 0x31;
   r++;
   }while(r < 25);

   r =   1;
   t =   0;

   do{
   if (ZnDDl[r] == 0x0001) t = t + 1;
   r++;
   }while(r < 25);

   if ((t > 6) || (t == 6)) DD6 = 0x31;
   else DD6 = 0x30;

   r = 1;
   do{    
		 if (ZnDDl[r] > 0) ZnDD[r] = 0x31;
		 else ZnDD[r] = 0x30;
     r++;
   }while(r < 25);

   r = 1;
   do{
   ZnTekI[r+257] = ZnDD[r];
   r++;
   }while(r < 25);

   r = 1;
   do{
   ZnArxI[91+r] = ZnDD[r];
   r++;
   }while(r < 25);

   ZnTekI[282] = DD1;
   ZnTekI[283] = DD6;
   //ZnTekIc[282] = ZnTekI[282];	//TODO1 ???
   //ZnTekIc[283] = ZnTekI[283];	//TODO1 ???

   ZnArxI[218] = DD6;
   //ZnArxIc[218] = ZnArxI[218];	//TODO1 ???

   /*r = 1;
   do{
   ZnTekIc[r+257] = ZnTekI[r+257];	//TODO1 ???
   r++;
   }while(r < 25);*/

   /*r = 1;
   do{
   ZnArxIc[r+91] = ZnArxI[r+91];	//TODO1 ???
   r++;
   }while(r < 25);*/

//L1:
 }
//========================================================
 /*Обработка показаний ДТЭ, формирование признака и запись в ZnTekIc(ZnTekI)*/
 void DTE(){

 //uint16_t r;
 
  float r;
//  port[AdAK+3] =$00;/*установка разъема J8 (каналы 0...23) CPU188R на "передачу" для A,B,C1,С2*/
//  port[AdAK]=$3D;/*адрес съема напряжения ДТЭ*/
	//setUNIO(27,1);setUNIO(26,1);setUNIO(25,0);setUNIO(24,1);setUNIO(31,0);setUNIO(30,1);
	setUNIO(27,1);setUNIO(26,0);setUNIO(25,1);setUNIO(24,1);setUNIO(29,0);setUNIO(28,1);
  delay_ms(50);
  r = ADC0();
	//r = 5.0; //TODO!!! отладка с неработающим АК1
	 
  if (r < 2.5) PrDTE = 0x31; else PrDTE = 0x30;

  ZnTekI[294] = PrDTE;
//  ZnTekIc[294] = ZnTekI[294];	//TODO1 ???
  ZnArxI[216] = PrDTE;
  //ZnArxIc[216] = ZnArxI[216];	//TODO1 ???
  }
/*==============================================*/
/*Анализ состояния предохранителя, тока КЗ и формирование признаков*/
 void Prdh(){
 //uint16_t r;
 //char stx[10];
 //port[AdAK+3] = 0x00;
 //port[AdAK] = 0x20;
	setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,0);setUNIO(29,1);setUNIO(28,0);     
 delay_ms(50);

 izmZn = ADC0();
 if (izmZn < 0.5){
	 			if (F_DEBUG){
					sprintf(kS,"%6.3f\0",izmZn);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
					sendDebug("PrEr=");sendDebug(kS);}
	 delay_ms(100);
	 izmZn = ADC0();
	 			if (F_DEBUG){
					sprintf(kS,"%6.3f\0",izmZn);   //преобразование в строку минимальной длинны 6 и кол-во знаков после запятой 4, width- реальная длинна строки
					sendDebug("Pr2=");sendDebug(kS);}
	 if (izmZn < 0.5)PrPrdh = 0x31;
	 else PrPrdh = 0x30;
 }
 else PrPrdh = 0x30;/*Пр.-сгорел ==> "1"*/
// if izmZn < 5.0 then PrPrdh = 0x31 else PrPrdh = 0x30;//Пр.-сгорел ==> "1"
 izmZn = Read_ADC1();
 izmZn = izmZn / 0.04;
 if (izmZn >= 500) PrKZ = 0x31;
 else PrKZ = 0x30;/*ток > 500 A ==> "1"*/

 ZnTekI[356] = PrPrdh;
 ZnTekI[357] = PrKZ;

 ZnArxI[217] = PrPrdh;
 ZnArxI[215] = PrKZ;
 //ZnArxIc[217] =  ZnArxI[217];	//TODO ???
 //ZnArxIc[215] =  ZnArxI[215];	//TODO ???

   /*r =356;
   do{
   ZnTekIc[r] =  ZnTekI[r];	//TODO ???
   r++;
   }while(r < 358);*/
 }
/*==============================================*/

void NomZapArx(){ /* Определение № записи архивa испытаний*/

  //uint16_t r;
	int width = 0;

  w = w + 1;
	if (w >= 3*1024) 	//если память архива испытаний заполнена, то иначинаем с начала
		w = 0;
  //Str(w:5, stroka);
  sprintf(stroka,"%5d",w);
	
  ZnTekI[760] = stroka[0];
  ZnTekI[761] = stroka[1];
  ZnTekI[762] = stroka[2];
  ZnTekI[763] = stroka[3];
  ZnTekI[764] = stroka[4];
//TODO ???
/*	  ZnArxIc[250] = stroka[0];
  ZnArxIc[251] = stroka[1];
  ZnArxIc[252] = stroka[2];
  ZnArxIc[253] = stroka[3];
  ZnArxIc[254] = stroka[4];

  ZnTekIc[760] = stroka[0];
  ZnTekIc[761] = stroka[1];
  ZnTekIc[762] = stroka[2];
  ZnTekIc[763] = stroka[3];
  ZnTekIc[764] = stroka[4];*/

   /*r = 1;
   do{
   ZnTekI[759 + r] = ZnTekIc[759 + r];//TODO ???
   r++;
   }while(r < 6);

   r = 0;
   do{
   ZnArxI[250 + r] = ZnArxIc[ 250 + r];	//TODO ???
   r++;
   }while(r < 5);*/

 }
/*========================================================*/
/*Анализ состояния "R" изоляции и формирование признаков*/
 void Riz(){
/* label 1;   */
 //uint8_t f;
 float U2,Ub,Rvx,g,h;
 uint16_t /*r,*/i;
 //char st[10];
// stU1 : string;
 //stU2 : string;

 //old version
	 // по умолчанию все на передачу
/*   //port[AdrBA+3] = 0x0A;  //установка разъема J9 (каналы 23...47:A-вывод,В-ввод,
      //                    С[0-3]-вывод,С[4-7]-ввод) CPU188R для режима Rиз.
   //port[AdrBA] =  0x00;    //установка регистра "А" выходов разъема J9 в "0"
	 for (i=0;i<8;i++){
		 setUNIO(27+i,0);}*/

	 
   U1 = 0;        /*напряжениe между "-"АБ и корпусом*/
   U2 = 0;        /*напряжениe между "+"АБ и корпусом*/
   Ub = ZnUakb[1];/*полное напряжениe АБ(между "+" и "-")*/
   //Ub = ZnUakb[1] - 1.2;//TODO!!! при отладке не хватало в ZnUab[1], но здесь тогда много, также изменено в Uab()
   Rvx = 0.1;     /*"R" входа АЦП равное 100 кОм*/

 		  //port[AdrO3] = $01; {включение режима измерения Rиз.(реле - на корпус)}
			setDigitalOutSPI(16,1);	//TODO проверить номер выхода
   delay_ms(5000);

  /*измерение напряжения между "-"АБ и корпусом*/

   
    //port[AdAK] = 0x39;/*вход Ai25 (минус АБ)*/
			 setUNIO(27,1);setUNIO(26,0);setUNIO(25,0);setUNIO(24,1);setUNIO(29,0);setUNIO(28,1);
    delay_ms(1000);
    izmZn = ADCR();
    U1 = izmZn;// * 4.01; //у Николая Павловича без умножения, хотя не понятно почему
		delay_ms(200);

  /*измерение напряжения между "+"АБ и корпусом*/

    //port[AdAK] = 0x20;/*вход Ai0 (плюс АБ)*/
  	 setUNIO(27,0);setUNIO(26,0);setUNIO(25,0);setUNIO(24,0);setUNIO(29,1);setUNIO(28,0);
  	delay_ms(1000);
    izmZn = ADCR();
    U2 = izmZn;// * 4.01;//у Николая Павловича без умножения, хотя не понятно почему
		delay_ms(200);

    //port[AdrO3] = $00;{выключение режима измерения Rиз.(реле - на землю)}
			setDigitalOutSPI(16,0);	//TODO проверить номер выхода

    delay_ms(500);

    g = fabs(U1) + fabs(U2);
    h = (Ub/g -1)* Rvx;
		if (h <= 0.5) PrRiz = 0x31;
    //if (h >= 0.3) PrRiz = 0x31;	//TODO Разобраться, не понятно работает КП!!!
    else PrRiz = 0x30;

    ZnTekI[358] = PrRiz;
    //ZnTekIc[358] =  ZnTekI[358];	//TODO ???
 }
/*==============================================*/
/*======================================================*/
/*void NomRArx(){ // Определение № записи рабочего архива
  //uint16_t r; 
  int width = 0;
  wr = wr + 1;
//  Str(wr:5, stroka);
  width = sprintf(stroka,"%5d\0",wr);   //я┐╜я┐╜ я┐╜я┐╜сла я┐╜ я┐╜я┐╜рокя┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜ 5, я┐╜ width я┐╜я┐╜я┐╜я┐╜твительная┐╜ я┐╜я┐╜я┐╜я┐╜я┐╜я┐╜ я┐╜я┐╜рокя┐╜
  if (width != 5){//я┐╜шибя┐╜я┐╜!!!
  }
*/
	//TODO ???
  /*ZnArxIc[250] = stroka[0];
  ZnArxIc[251] = stroka[1];
  ZnArxIc[252] = stroka[2];
  ZnArxIc[253] = stroka[3];
  ZnArxIc[254] = stroka[4];


  ZnTekIc[755] = stroka[0];
  ZnTekIc[756] = stroka[1];
  ZnTekIc[757] = stroka[2];
  ZnTekIc[758] = stroka[3];
  ZnTekIc[759] = stroka[4];*/
	/*
	ZnArxI[250] = stroka[0];
  ZnArxI[251] = stroka[1];
  ZnArxI[252] = stroka[2];
  ZnArxI[253] = stroka[3];
  ZnArxI[254] = stroka[4];


  ZnTekI[755] = stroka[0];
  ZnTekI[756] = stroka[1];
  ZnTekI[757] = stroka[2];
  ZnTekI[758] = stroka[3];
  ZnTekI[759] = stroka[4];*/

   /*r = 1;
   do{
   ZnTekI[754 + r] = ZnTekIc[754 + r];	//TODO ???
   r++;
   }while(r < 6);

    r=0;
   do{
   ZnArxI[250 + r]= byte(ZnArxIc[ 250 + r]);	//TODO ???
   r++;
   }while(r < 6);*/
 /*}*/
/*========================================================*/
/*void NRA(){ // Подготовка и вывод № записи рабочего архива

 uint16_t i;

  RTekI();

  ZnS[1] = 0x52;//R
  ZnS[2] = ZnTekI[755];
  ZnS[3] = ZnTekI[756];
  ZnS[4] = ZnTekI[757];
  ZnS[5] = ZnTekI[758];
  ZnS[6] = ZnTekI[759];
  ZnS[7] = KT;

  for (i=1;i<=7;i++) SendByte(AdrRY,ZnS[i]);
 }*/
//========================================================
/*Обработка признака работы с ПЭВМ и запись его в ZnTekIc(ZnTekI)*/
void PC(){
uint16_t a;
	
//  port[AdrI] = $00;{чтобы можно было читать регистры входов}
//// port[AdrId]= $AA;{установка антидребезга 4.5 mS}

  //a = portw[AdrI2];{DD 17...32}
  //a = !a;
  //a = a & 0x1000;
  if (a != 0) PrPC = 0x30; else PrPC = 0x31;
	a = getDigitalInSPI(28);	//признак работы с ПЭВМ - 29 вход => 28й, если считать от нуля
	//a = 1; //TODO debug //
	//TODO сделать обработку ошибок -1
  if (a == 0) PrPC = 0x30; else PrPC = 0x31;
  //исправить на ==

  ZnTekI[399] = PrPC;

  //ZnTekIc[399] = ZnTekI[399];	//TODO ???
 }
/*========================================================*/
/*Установка команды АСКД "ИСПРАВНО"*/
 void YstISP()
  {
  //port[AdrO4] = $01;
		setDigitalOutSPI(24,1);
  }
//============================================
/*Сброс команды АСКД "ИСПРАВНО" */
 void SbrISP()
  {
  //port[AdrO4] = $00;
		setDigitalOutSPI(24,0);
  }
//============================================
/*Установка команды с АСКД на САЭ*/
 void Yst(uint8_t m){
	 uint8_t a;//,b;
	 uint8_t i=0;
	 //+++old version
  //a = port[AdrO1];
  //b = a | m;
  //port[AdrO1] = b;
	 //---old version
		a = 0x01;
		 for(i=0;i<8;i++){
			 if (a & m){
				 setDigitalOutSPI(i,1);
			 }
			 a = a << 1;
		 }
 }
//============================================
/*Сброс команды с АСКД на САЭ*/
 void Sbr(uint8_t m){
	 uint8_t a;//,b;
	 uint8_t i=0;
	 /*+++old version
  a = port[AdrO1];
  b = a & m;
  if (b > 0) {
                 b = a ^ m;
		             //port[AdrO1] = b;
                 }
	 //---old version*/
		a = 0x01;
		 for(i=0;i<8;i++){
			 if (a & m){
				 setDigitalOutSPI(i,0);
			 }
			 a = a << 1;
		 }
 }
//============================================
/*Установка команды Riz НЕ НОРМА*/
 void YRNN(){
 //uint8_t a,b;
  //a = port[AdrO2];
  //b = a | 0x01;
  //b = b | 0x04;
  //port[AdrO2] = b;
setDigitalOutSPI(8,1);
setDigitalOutSPI(10,1);
	 
  ZnArxI[230] = 0x32;
  //ZnArxIc[230]= chr(ZnArxI[230]);
  ZnTekI[752] = 0x32;
  //ZnTekIc[752] = chr(ZnTekI[752]);
 }
//============================================
 /*Сброс команды Riz НЕ НОРМА*/
 void SRNN(){
 //uint8_t a,b,c;
/*  a = port[AdrO2];
  b = a & 0x01;
  if (b > 0) b = a ^ 0x01;
  c = a & 0x04;
  if (c > 0) b = b ^ 0x04;
  if (b == 0x00) a = a & 0xFA;
  port[AdrO2] = a;*/
	setDigitalOutSPI(8,0);
	 setDigitalOutSPI(10,0);

	 ZnArxI[230] = 0x30;
  //ZnArxIc[230] = chr(ZnArxI[230]);
  ZnTekI[752] = 0x30;
  //ZnTekIc[752] = chr(ZnTekI[752]);

 }
//============================================
/*Установка команды Riz НОРМА*/
 void YRN(){
 //uint8_t a,b;
  //a = port[AdrO2];
//  b = a | 0x02;
//  b = b | 0x04;
  //port[AdrO2] = b;
setDigitalOutSPI(9,1);
setDigitalOutSPI(10,1);

  ZnArxI[230] = 0x31;
  //ZnArxIc[230] = chr(ZnArxI[230]);
  ZnTekI[752] = 0x31;
  //ZnTekIc[752] = chr(ZnTekI[752]);

 }
//============================================
  /*Сброс команды Riz НОРМА*/
 void SRN(){
 //uint8_t a,b,c;
  //a = port[AdrO2];
/*  b = a & 0x02;
  if (b > 0) b = a ^ 0x02;
  c = a & 0x04;
  if (c > 0) b = b ^ 0x04;
  if (b == 0x00) a = a & 0xF9;
*/
//  port[AdrO2] = a;
setDigitalOutSPI(9,0);
	 setDigitalOutSPI(10,0);

	 ZnArxI[230] = 0x30;
  //ZnArxIc[230] = chr(ZnArxI[230]);
  ZnTekI[752] = 0x30;
  //ZnTekIc[752] = chr(ZnTekI[752]);
 }
//============================================
/*Установка "НЕ НОРМА"*/
 void YNN(uint8_t m){
 uint8_t a;//,b;
	 uint8_t i=0;
  //a = port[AdrO2];
//  b = a | m;
	 a = 0x01;
	 for (i=0;i<8;i++){
			 if (a & m){
				 setDigitalOutSPI(i+8,1);	//+8 т.к. 2й байт
			 }
			 a = a << 1;
	 }
//  port[AdrO2] = b;
 }
//============================================
/*Сброс "НЕ НОРМА"*/
 void SNN(uint8_t m){
 uint8_t a;//,b,c;
	 uint8_t i=0;
  //a = port[AdrO2];
/*  b = a & m;
  c = a & 0x07;
  if (b > 0) b = b ^ m;
  c = c | b;*/
  //port[AdrO2] = c;
	 a = 0x01;
	 for (i=0;i<7;i++){	//не норма не сбрасываем
			 if (a & m){
				 setDigitalOutSPI(i+8,0);	//+8 т.к. 2й байт
			 }
			 a = a << 1;
	 }
	 //если не осталось ошибок (Не норма ДТЭ, Не норма Пр, Не Норма ДД, Не Норма р.емк. не установлены, по крайней мере в памяти)
	 if (!getSaveDigitalOut(11) && !getSaveDigitalOut(12) && !getSaveDigitalOut(13) && !getSaveDigitalOut(14))
		 setDigitalOutSPI(15,0);	//сбрасываем признак НЕ НОРМА АБ
 }
/*============================================*/

void Sbros(){

Sbr(VZar);//Сброс ком-ды "Включить ЗАРЯД АБ"
 Sbr(VRaz);//Сброс ком-ды "Включить РАЗРЯД АБ"
 Sbr(VPZ); //Сброс ком-ды "Включить ПОДЗАРЯД АБ"
 Sbr(VDR); //Сброс ком-ды "Включить ДОРАЗРЯД АБ"
 
 /*a = 0x04;//Сброс ком-ды "Включить ЗАРЯД АБ"
 SVixA4(a);
 a = 0x08;//Сброс ком-ды "Включить РАЗРЯД АБ"
 SVixA4(a);
 a = 0x02;//Сброс ком-ды "Включить ПОДЗАРЯД АБ"
 SVixA4(a);
 a = 0x01;//Сброс ком-ды "Включить ДОРАЗРЯД АБ"
 SVixA4(a);*/
 }
/*kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk*/
/*Прием коэффициентов по команде и запись их в "TekInf.txt" */
void KFUzap(){
 uint16_t i,m;
/*Запись коэффициентов в "TekInf.txt"*/
           i = 621;
           m = 1;
           do {
           ZnTekI[i]= kU[m];
           //ZnTekIc[i]= kUc[m];
           m++;
           i++;
           }while(i < 741);
           delay_ms(100);
           WTekI();
 }
/*========================================================*/
/*Считывание коэффициентов из "TekInf.txt" и запись их в массив "kUr[1..24]" */
void KFUcht(){
 uint16_t i,m;
 char ka[6];
  /*Чтение коэффициентов из "TekInf.txt"*/

           i = 621;
           m = 1;
           do{
//						 kUc[m]= ZnTekIc[i];
						 kU[m]= ZnTekI[i];
						 m++;
						 i++;
           }while(i < 741);

  /* массив символов в массив чисел */
					 //TODO1 ???
           i = 1;
           m = 0;
           do{
						 ka[0] = kU[m + 1];
						 ka[1] = kU[m + 2];
						 ka[2] = kU[m + 3];
						 ka[3] = kU[m + 4];
						 ka[4] = kU[m + 5];
						 ka[5] = 0;

						 kUr[i] = strtof (ka,NULL);
						 i++;
						 m = m + 5;
           }while(i < 25);

          /* m=620;
           i=1;
           repeat

           Str(kUr[i]:5:2,ka);

           ZnTekIc[m + 1] = ka[1];
           ZnTekIc[m + 2] = ka[2];
           ZnTekIc[m + 3] = ka[3];
           ZnTekIc[m + 4] = ka[4];
           ZnTekIc[m + 5] = ka[5];

           m = m + 5;
           inc(i);
           until(i = 25);*/

           /*i= 621;
           repeat
           ZnTekI[i]= byte(ZnTekIc[i]);
           inc(i);
           until(i = 741);*/



 }
/*========================================================*/
/*Прием коэффициентов по ТОКУ по команде и запись их в "TekInf.txt"  */
void KFIzap(){
  uint16_t i,m;
          i= 741;
           m= 1;
           do{
						 ZnTekI[i]= kI[m];
						 //ZnTekIc[i]= kIc[m];
						 m++;
						 i++;
           }while(i < 751);

           WTekI();
 }
/*========================================================*/
 void KFIIcht(){
 //TODO надо ли?
          kSz[0]= ZnTekI[741];
          kSz[1]= ZnTekI[742];
          kSz[2]= ZnTekI[743];
          kSz[3]= ZnTekI[744];
          kSz[4]= ZnTekI[745];
	 
					kZr = strtof (kSz,NULL);

          kSr[0]= ZnTekI[746];
          kSr[1]= ZnTekI[747];
          kSr[2]= ZnTekI[748];
          kSr[3]= ZnTekI[749];
          kSr[4]= ZnTekI[750];

					kRr = strtof (kSr,NULL);
 }
/*kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk*/


void processCalibrationCommands() {
	//if (!byteAvailable()) return;

	uint8_t cmd = GetByte(AdrRY);

	if (cmd == 0xA1) {
		handleWritePoint();
	}
	else if (cmd == 0xA2) {
		handleCalibration();
	}
	else {
		return; //пришел мусор или не та команда
	}



}

void handleWritePoint(void)
{
	uint8_t ch = GetByte(AdrRY);
	if (ch > 1) return; // у нас только 0 и 1
	char d[4];
	for (int i = 0; i < 4; i++)
		d[i] = (char)GetByte(AdrRY);

	uint16_t V = parse4digits_to_u16(d);

	uint16_t A = (ch == 0) ? ADC0_raw() : ADC1_raw();

	if (ch == 0) {
		if (n0 < 2) { V0[n0] = V; A0[n0] = A; n0++; }
	    else { V0[1] = V; A0[1] = A; }
	} else {
		if (n1 < 2) { V1_[n1] = V; A1_[n1] = A; n1++; }
		else { V1_[1] = V; A1_[1] = A; }
	}

}


void handleCalibration(void)
{
    uint8_t ch = GetByte(AdrRY);
    if (ch > 1) return;

    uint16_t V_a, V_b, A_a, A_b;

    if (ch == 0) {
        if (n0 < 2) return;
        V_a = V0[0]; V_b = V0[1];
        A_a = A0[0]; A_b = A0[1];
    } else {
        if (n1 < 2) return;
        V_a = V1_[0]; V_b = V1_[1];
        A_a = A1_[0]; A_b = A1_[1];
    }

    // чтобы не делить на ноль и не ловить мусор
    if (A_a == A_b) return;

    float Vhi = (V_a > V_b) ? (float)V_a : (float)V_b;
    float Vlo = (V_a > V_b) ? (float)V_b : (float)V_a;
    float Ahi = (V_a > V_b) ? (float)A_a : (float)A_b;
    float Alo = (V_a > V_b) ? (float)A_b : (float)A_a;

    float k = (Vhi - Vlo) / (Ahi - Alo);   // мВ на 1 ADC-единицу
    float D = Vhi / k - Ahi;

    if (ch == 0) { k0 = k; D0 = D; }
    else         { k1 = k; D1 = D; }
}


static uint16_t parse4digits_to_u16(char d[4])
{
    return (uint16_t)((d[0]-'0')*1000 +
                      (d[1]-'0')*100  +
                      (d[2]-'0')*10   +
                      (d[3]-'0'));
}

