#include "global.h"
#include "MemoryFunctions.h"
//#include "ROM_1636RR4U.h"
#include "UART.h"
#include <stdint.h>
#include "flash_ext.h"
#include "Flash_SSt25.h"

uint8_t ZnS[65];//формат Pascal =>64+1=65
uint8_t ZnTekI[766];//формат Pascal =>765+1=766
uint8_t ZnX[129];//формат Pascal =>128+1=129
extern uint8_t ZnArxI[256];
extern SPI_HandleTypeDef hspi1;

uint16_t adrArxI = 0;//адрес - номер записи, где храниться последняя запись с архивной информацией из архива испытаний (со 2ого сектора по 4ый)
uint16_t adrArxR = 0;//адрес - номер записи, где храниться последняя запись с архивной информацией из рабочего архива (с 5ого сектора по 8й)
uint8_t fMemoryArxIOverWrite = 0;	//признак того, что память для архива испытаний закончилась и мы начинаем перезаписывать сначала (со 2ого сектора)
uint8_t fMemoryArxROverWrite = 0;	//признак того, что память для рабочего архива закончилась и мы начинаем перезаписывать сначала (со 5ого сектора)

static const uint32_t ADR_TEKI   = 0x000004;   // где лежит TekInf (твоё старое 4)
static const uint32_t ADR_ARXI   = 0x040000;   // испытания: сектора 2..4
static const uint32_t ADR_ARXR   = 0x100000;   // рабочий:   сектора 5..8

#define TEKI_BYTES               771  // 765 + 6 доп.байтов в твоей записи
#define RECORD_SIZE              256  // одна архивная запись

uint32_t adrTekI = 4;// 0x080000;//4;//адрес где храниться текущая информация (в первом секторе)
extern const uint8_t KT; //конец передачи текста


/*Формирование зав. номера на вывод из файла "TekInf.txt"*/
 void FZavN(){
     ZnS[1] = 0x4E;/*N*/
     ZnS[2] = ZnTekI[362];
     ZnS[3] = ZnTekI[363];
     ZnS[4] = ZnTekI[364];
     ZnS[5] = ZnTekI[365];
     ZnS[6] = ZnTekI[366];
     ZnS[7] = ZnTekI[367];
     ZnS[8] = ZnTekI[368];
     ZnS[9] = ZnTekI[369];
     ZnS[10] = ZnTekI[370];
     ZnS[11] = ZnTekI[371];
     ZnS[12] = KT;
}
/*========================================================*/
/*Формирование даты изготовления на вывод из файла "TekInf.txt"*/
 void FDIzg(){
     ZnS[1] = 0x57;/*W*/
     ZnS[2] = ZnTekI[373];
     ZnS[3] = ZnTekI[374];
	   ZnS[4] = 0x2E;
     ZnS[5] = ZnTekI[375];
     ZnS[6] = ZnTekI[376];
     ZnS[7] = 0x2E;
     ZnS[8] = 0x32;
     ZnS[9] = 0x30;
     ZnS[10] = ZnTekI[377];
     ZnS[11] = ZnTekI[378];
     ZnS[12] = 0x20;
     ZnS[13] = 0xA3;/*я┐╜*/
     ZnS[14] = 0x2E;
     ZnS[15] = KT;
}
/*=========================================================*/
 /*Формирование зав. номера и даты изг-я для вывода из файла "TekInf.txt" */
 void FND(){
     ZnS[1] = 0x4D;/*M*/
     ZnS[2] = ZnTekI[362];
     ZnS[3] = ZnTekI[363];
     ZnS[4] = ZnTekI[364];
     ZnS[5] = ZnTekI[365];
     ZnS[6] = ZnTekI[366];
     ZnS[7] = ZnTekI[367];
     ZnS[8] = ZnTekI[368];
     ZnS[9] = ZnTekI[369];
     ZnS[10] =ZnTekI[370];
     ZnS[11] =ZnTekI[371];

     ZnS[12] = ZnTekI[373];
     ZnS[13] = ZnTekI[374];
     ZnS[14] = 0x2E;
     ZnS[15] = ZnTekI[375];
     ZnS[16] = ZnTekI[376];
     ZnS[17] = 0x2E;
     ZnS[18] = ZnTekI[377];
     ZnS[19] = ZnTekI[378];
     ZnS[20] = KT;

}

void RdArxByte1636RR4(uint32_t addr)
{
    /*(void)addr;
     // ВРЕМЕННО: забиваем буфер нулями
    for (int i = 0; i < 256; ++i) {
        ZnArxI[i] = 0;
    }*/


    Flash_Read(addr, &ZnArxI[1], 255);
    //for (int k = 0; k < 255; ++k) SendByte(AdrRY, ZnArxI[k]); // pascal
    //SendByte(AdrRY, KT);
}

  /*Запись заводского номера в ZnTekI(ZnTekIс) при вводе*/
 void ZapZN(){
  //uint16_t r = 2;
	 uint16_t r = 1;
  int cr = r;
  while (ZnX[cr] != 'N') ++cr;
  int i = cr + 1;
  int j = 2;
  for ( ; i <= cr + 10; ++i, ++j) {
	  ZnTekI[j + 360] = ZnX[i];
  }
  /*do{
  ZnTekI[r+360] = ZnX[r];
  r++;
  }while(r < 12);*/
//Orig - не соответствует заиси => неправильный ответ на оманду
/*  do{
  ZnTekI[r+363] = ZnX[r];
  r++;
  }while(r < 9);
*/
  /*r = 365;
  do{
  ZnTekIc[r] = ZnTekI[r]; //TODO ???
  r++;
  }while(r < 372);*/
}
//========================================================
 //Запись даты изготовления в ZnTekI(ZnTekIс) при вводе
 void ZapDI(){
  //uint16_t r = 2;
	 uint16_t r = 1;
  int cr = r;
  while (ZnX[cr] != 'W') ++cr;
  int i = cr + 1;
  int j = 2;
  for ( ; i <= cr + 6; ++i, ++j) {
	  ZnTekI[j + 371] = ZnX[i];
  }

  /*do{
  ZnTekI[r+371] = ZnX[r];
  r++;
 }while (r < 8);*/

/*  r = 373;
  do{
  ZnTekIc[r] = ZnTekI[r];	//TODO ???
  r++;
  }while (r < 379);*/
 }

/*==============================================*/
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
/* Чтение текущих данных из файла TekInf.txt в массивы ZnTekI[ZnTekIc]*/
/*void RTekI(){
	 // читаем 771 байт из ADR_TEKI
	 uint8_t tmp[TEKI_BYTES] = {0};
	 if (flash_read(ADR_TEKI, tmp, TEKI_BYTES) != HAL_OK) return;

	 // первые 765 байт -> ZnTekI[1..765] (Pascal-стиль)
	 for (int i=0;i<765;i++) ZnTekI[i+1] = tmp[i];

	 // доп. байты
	 fMemoryArxIOverWrite = tmp[765];
	 fMemoryArxROverWrite = tmp[766];
	 adrArxI = ((uint16_t)tmp[767] << 8) | tmp[768];
	 adrArxR = ((uint16_t)tmp[769] << 8) | tmp[770];
}*/

void RTekI(void){
     uint8_t tmp[TEKI_BYTES] = {0};
     /*uint8_t CmdDataRead = 0x03;
     uint8_t adrArr[3];*/

     Flash_Read(ADR_TEKI, tmp, sizeof(tmp));

     /*adrArr[0] = (ADR_TEKI >> 16) & 0xFF;
     adrArr[1] = (ADR_TEKI >>  8) & 0xFF;
     adrArr[2] = (ADR_TEKI      ) & 0xFF;

     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
     HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
     HAL_SPI_Transmit(&hspi1,adrArr,3,100);
     HAL_SPI_Receive(&hspi1,tmp,TEKI_BYTES,100);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);*/

	 for (int i=0;i<765;i++) ZnTekI[i+1] = tmp[i];

	 fMemoryArxIOverWrite = tmp[765];
	 fMemoryArxROverWrite = tmp[766];
	 adrArxI = ((uint16_t)tmp[767] << 8) | tmp[768];
	 adrArxR = ((uint16_t)tmp[769] << 8) | tmp[770];
 }

/*==============================================*/
 /*Чтение архива из внешнего файла и запись его в массив ZnArxI[i](ZnArxIc[i])*/
 void ChtArx(){
	 int i=0,k=0;
	 if (fMemoryArxIOverWrite){	//память переполнилась и начала перезаписоваться заново, поэтому считываем данные из последующих секторов 
		 i = ((adrArxI / 1024) + 1) * 1024;	//начинаем со следующего сектора => вычисляем номер текущего сектора (начиная со 2ого) добавляем 1 и умножают на кол-во записей в секторе (1024 записи по 256 байт)
		 for (;(i<1024*3);i++){	//считываем данные с последующих секторов (до 5 невключительно)
			 RdArxByte1636RR4(i*256 + 0x040000);	//адрес = номер записи * кол-во байт в записи (на самом деле не 1 меньше) + 0x040000, так как записываем начиная со 2ого сектора
			 //посылка


			 for(k=0;k<255;k++) {
				 //SendByte(AdrRY, 0x33);
					SendByte(AdrRY,ZnArxI[k+1]);	//формат pascal

			 }
			 SendByte(AdrRY,0x03);
		 }
	 }
	 for (i=0;i<adrArxI;i++){	//от 0 до текущего номера
			 RdArxByte1636RR4(i*256 + 0x040000);	//адрес = адрес второго сектора + номер записи * кол-во байт в записи (на самом деле на 1 меньше)
			 //посылка

			 for(k=0;k<255;k++) {
				 SendByte(AdrRY,ZnArxI[k+1]);//формат pascal
				 //SendByte(AdrRY, 0x33);
			 }
			 SendByte(AdrRY,KT);
	 }
	 for(k=0;k<255;k++)
		SendByte(AdrRY,0x23);
	 SendByte(AdrRY,KT);
	 
  //Assign(FArxI,'Archive.dat');/*связывает имя внешнего файла с файловой переменной*/

     /*0xI-*/ /*Директива, отключающая контроль ошибок ввода-вывода*/
//     Reset(FArxI);/*открывает существующий файл для чтения*/
     /*0xI+*/ /*Директива, включающая контроль ошибок ввода-вывода*/
/*     if IOResult <> 0 then
                          begin
                       Rewrite(FArxI);//создание нового файла
                          if IOResult <> 0 then
                          write('Ошибка открытия файла')
                          end
                           else
                            // while NOT EOF(FArxI) do Read(FArxI,ArxM);
                              while NOT EOF(FArxI) do begin
                                               Read(FArxI,ArxM);
                                               With ArxM do
                                                     begin
                                           ArxC(ZnArxC)=  ArxM;
                                           i= 1;
                                           do{
                                           ZnArxIc[i]= ZnArxC[i];
                                           i++;
                                           }while(i < 256);

     for (uint i=1;i<=255;i++) ZnArxInf[i] =  ZnArxIc[i]; //ZnArxI[i];    
     ZnArxInf[256] = 0x03;

     for (uint i=1;i<=256;i++) do SendByte(AdrRY,ZnArxInf[i]);
                                                    end;
                                                end;
//      Close(FArxI);

     for (uint i=1;i<=255;i++) ZnArxInf[i] = 0x23; //#
     ZnArxInf[256] = 0x03;
     for (uint i=1;i<=256;i++) SendByte(AdrRY,ZnArxInf[i]);*/
    }
/*==============================================*/
/*Запись текущих данных из массивов ZnTekI[ZnTekIc] в файл TekInf.txt*/
 void WTekI(void){
     // Стираем сектор с TekInf
	 Sector4KB_Erase(ADR_TEKI);
     //flash_erase_sector(ADR_TEKI & ~(FLASH_SECTOR_SIZE - 1));

     // Готовим буфер 771 байт
     uint8_t tmp[TEKI_BYTES] = {0};
     for (int i=0;i<765;i++) tmp[i] = ZnTekI[i+1];
     tmp[765] = fMemoryArxIOverWrite;
     tmp[766] = fMemoryArxROverWrite;
     tmp[767] = (uint8_t)(adrArxI >> 8);
     tmp[768] = (uint8_t)(adrArxI);
     tmp[769] = (uint8_t)(adrArxR >> 8);
     tmp[770] = (uint8_t)(adrArxR);

     Flash_PageProgram(ADR_TEKI, tmp, sizeof(tmp));
     //RTekI();
 }

/*==============================================*/
 void ZapArx(void){
     ZnArxI[255] = 0x0A;

     uint32_t addr = ADR_ARXI + (uint32_t)adrArxI * RECORD_SIZE;


     if ((addr % FLASH_SECTOR_SIZE) == 0){
         //flash_erase_sector(addr);
    	 Sector4KB_Erase(addr);
     }

     // Запись 256 байт (Pascal: 1..255)
     //flash_write(addr, &ZnArxI[1], 255);

     Flash_PageProgram(addr, &ZnArxI[1], 255);
     // сдвиг адреса/переполнение
     adrArxI++;
     if (adrArxI >= 3*1024){
         fMemoryArxIOverWrite++;
         adrArxI = 0;
     }

	 
   //Assign(FArxI,'Archive.dat');/*связывает имя внешнего файла с файловой переменной*/
     /*0xI-*/ /*Директива, отключающая контроль ошибок ввода-вывода*/
   /*  Reset(FArxI);//открывает существующий файл для чтения
     if IOResult <> 0 then
                          begin
                       Rewrite(FArxI);//создание нового файла
                          if IOResult <> 0 then
                          write('Ошибка открытия файла')
                          end
                           else
                            while NOT EOF(FArxI) do read(FArxI,ArxM);

      *//*0xI+*/ /*Директива, включающая контроль ошибок ввода-вывода*/
/*
  With ArxM do
            begin
     ZnArxIc[255] = 0x0A;

     i = 1;
     do{
     ZnArxC[i]=  ZnArxIc[i];
     i++;
     }while(i < 256);

     ArxM=  ArxC(ZnArxC);
     Write(FArxI,ArxM);

     Close(FArxI);
            end;*/
 }
 
 /*==============================================*/
 /*Чтение архива из внешнего файла и запись его в массив ZnArxI[i](ZnArxIc[i])*/
 static void read_record(uint32_t base, uint32_t index){
     uint8_t rec[255];
     Flash_Read(base + index*RECORD_SIZE, rec, 255);
     for (int k=0;k<255;k++) SendByte(AdrRY, rec[k]); // pascal
     SendByte(AdrRY, KT);
 }

 /*
 void ChtArx(void){ // испытания
     if (fMemoryArxIOverWrite){
         uint32_t start = ((adrArxI / 1024) + 1) * 1024;
         for (uint32_t i = start; i < 3*1024; ++i) read_record(ADR_ARXI, i);
     }
     for (uint32_t i = 0; i < adrArxI; ++i) read_record(ADR_ARXI, i);

     for (int k=0;k<255;k++) SendByte(AdrRY, 0x23);
     SendByte(AdrRY, KT);
 }
 */

 /*void ChtRArx(void){ // рабочий
	 //fMemoryArxROverWrite = 0;
     if (fMemoryArxROverWrite){
         uint32_t start = ((adrArxR / 1024) + 1) * 1024;
         for (uint32_t i = start; i < 4*1024; ++i) read_record(ADR_ARXR, i);
     }

     if (fMemoryArxROverWrite){
         for (uint32_t i = adrArxR; i < 4*1024; ++i) read_record(ADR_ARXR, i);
     }
     //read_record(ADR_ARXR, 0);
     //read_record(ADR_ARXR, adrArxR - 1);
     for (uint32_t i = 0; i < adrArxR; ++i) read_record(ADR_ARXR, i);
     //for (uint32_t i = 0; i < adrArxR; ++i) read_record(0x0, i);

     for (int k=0;k<255;k++) SendByte(AdrRY, 0x23);
     SendByte(AdrRY, KT);
 }*/




 static void RdArxByte_New(uint32_t adr)
 {
     Flash_Read(adr, &ZnArxI[1], 255);

 }

 void ChtRArx(void)
 {
     int i=0,k=0;

     if (fMemoryArxROverWrite){
         i = ((adrArxR / 1024) + 1) * 1024;
         for (; i < 1024*4; i++){
             RdArxByte_New(ADR_ARXR + i*256);

             for (k=0; k<255; k++)
                 SendByte(AdrRY, ZnArxI[k+1]); // как в старом

             SendByte(AdrRY, 0x03); // KT
         }
     }

     for (i=0; i<adrArxR; i++){
         RdArxByte_New(ADR_ARXR + i*256);

         for (k=0; k<255; k++)
             SendByte(AdrRY, ZnArxI[k+1]);


         SendByte(AdrRY, 0x03); // KT
     }

     for (k=0; k<255; k++)
         SendByte(AdrRY, 0x23);

     SendByte(AdrRY, 0x03); // KT
 }

	 
  /*Assign(FArxI,'RabArx.dat');//связывает имя внешнего файла с файловой переменной
     Reset(FArxI);//открывает существующий файл для чтения
     if IOResult <> 0 then
                          begin
                       Rewrite(FArxI);//создание нового файла
                          if IOResult <> 0 then
                          write('Ошибка открытия файла')
                          end
                           else
                              while NOT EOF(FArxI) do begin
                                               Read(FArxI,ArxM);
                                               With ArxM do
                                                     begin
                                           ArxC(ZnArxC)=  ArxM;
                                           i= 1;
                                           do{
                                           ZnArxIc[i]= ZnArxC[i];
                                           i++;
                                           }while (i < 256);

     for i= 1 to 255 do ZnArxInf[i] =  byte(ZnArxIc[i]);  //ZnArxI[i]; 
     ZnArxInf[256] =  0x03;

     for i= 1 to 256 do SendByte(AdrRY,ZnArxInf[i]);
                                                    end;
                                                end;
      //Close(FArxI);

     for (uint i =1;i<=255;i++) ZnArxInf[i] = 0x23; //#
     ZnArxInf[256] = 0x03;
     for (uint i =1;i<=256;i++) do SendByte(AdrRY,ZnArxInf[i]);
    }*/
/*==============================================*/
 void ZapRArx(void){
     ZnArxI[255] = 0x0A;
     uint32_t addr = ADR_ARXR + (uint32_t)adrArxR * RECORD_SIZE;
	 //Sector4KB_Erase(addr);
     /*if ((addr % FLASH_SECTOR_SIZE) == 0){

         //flash_erase_sector(addr);
    	 Sector4KB_Erase(addr);
     }*/


     if ((adrArxR % (FLASH_SECTOR_SIZE / RECORD_SIZE)) == 0) {
         uint32_t sector_addr = ADR_ARXR + (uint32_t)adrArxR * RECORD_SIZE;
         sector_addr &= ~(FLASH_SECTOR_SIZE - 1); // выровнять на 4KB
         Sector4KB_Erase(sector_addr);
     }

     //flash_write(addr, &ZnArxI[1], 255);
     Flash_PageProgram(addr, &ZnArxI[1], 255);
     //Flash_PageProgram(0x0, &ZnArxI[1], 255);

     adrArxR++;
     if (adrArxR >= 4*1024){
         fMemoryArxROverWrite++;
         adrArxR = 0;
     }

     // обновим строковый номер в ZnTekI
     char s[6]; // 5 символов + '\0'
     snprintf(s, sizeof(s), "%5u", (unsigned)adrArxR);
     ZnTekI[755] = s[0];
     ZnTekI[756] = s[1];
     ZnTekI[757] = s[2];
     ZnTekI[758] = s[3];
     ZnTekI[759] = s[4];
 }


	 
 /*Assign(FArxI,'RabArx.dat');
     Reset(FArxI);
     if IOResult <> 0 then
                          begin
                       Rewrite(FArxI);
                          if IOResult <> 0 then
                          write('Ошибка открытия файла')
                          end
                           else
                            while NOT EOF(FArxI) do read(FArxI,ArxM);

  With ArxM do
            begin
     ZnArxIc[255]=  char(0x0A);

     i= 1;
     do{
     ZnArxC[i]=  ZnArxIc[i];
     i++;
     }while(i < 256);

      ArxM=  ArxC(ZnArxC);
     Write(FArxI,ArxM);

     Close(FArxI);
            end;*/
 //}

 
 void EraseRabArx(void){
     adrArxR = 0; fMemoryArxROverWrite = 0;
     ZnTekI[755] = ZnTekI[756] = ZnTekI[757] = ZnTekI[758] = 0x20;
     ZnTekI[759] = '0';
     // 4 сектора
     /*flash_erase_sector(ADR_ARXR + 0*FLASH_SECTOR_SIZE);
     flash_erase_sector(ADR_ARXR + 1*FLASH_SECTOR_SIZE);
     flash_erase_sector(ADR_ARXR + 2*FLASH_SECTOR_SIZE);
     flash_erase_sector(ADR_ARXR + 3*FLASH_SECTOR_SIZE);*/
     /*Sector4KB_Erase(ADR_ARXR + 0*FLASH_SECTOR_SIZE);
     Sector4KB_Erase(ADR_ARXR + 1*FLASH_SECTOR_SIZE);
     Sector4KB_Erase(ADR_ARXR + 2*FLASH_SECTOR_SIZE);
     Sector4KB_Erase(ADR_ARXR + 3*FLASH_SECTOR_SIZE);*/

     for (uint32_t a = ADR_ARXR; a < ADR_ARXR + 0x100000; a += FLASH_SECTOR_SIZE) {
         Sector4KB_Erase(a);
     }
 }

 void EraseArx(void){
     // 3 сектора (испытания)
     /*flash_erase_sector(ADR_ARXI + 0*FLASH_SECTOR_SIZE);
     flash_erase_sector(ADR_ARXI + 1*FLASH_SECTOR_SIZE);
     flash_erase_sector(ADR_ARXI + 2*FLASH_SECTOR_SIZE);*/
	 Sector4KB_Erase(ADR_ARXI + 0*FLASH_SECTOR_SIZE);
	 Sector4KB_Erase(ADR_ARXI + 1*FLASH_SECTOR_SIZE);
	 Sector4KB_Erase(ADR_ARXI + 2*FLASH_SECTOR_SIZE);
     fMemoryArxIOverWrite = 0;
     adrArxI = 0;
 }

 void EraseTekI(void){
     //flash_erase_sector(ADR_TEKI & ~(FLASH_SECTOR_SIZE-1));
	 Sector4KB_Erase(ADR_TEKI & ~(FLASH_SECTOR_SIZE-1));
 }

 void InitMemory(void){
     if ((adrArxR == 0) && (fMemoryArxROverWrite == 0)) EraseRabArx();
     if ((adrArxI == 0) && (fMemoryArxIOverWrite == 0)) EraseArx();
 }

