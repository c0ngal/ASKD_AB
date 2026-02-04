#ifndef __MEMORY_FUNCTIONS_H
#define __MEMORY_FUNCTIONS_H

#include <stdint.h>                           // Include standard types 

extern uint8_t ZnS[65];   //массив для посылки данных из памяти на ПК по UART, используются  с 1 по  24 байты (первые 25 в си, кроме 1-ого(с индексом 0))//формат pascal 64+1=65
extern uint8_t ZnX[129];//формат pascal 128+1=129
extern uint8_t ZnTekI[766];/*массив данных для ввода/вывода в byte-текущая инф-я*///формат pascal 765+1=766
extern uint8_t tmpArx[1024];//массив данных для временного хранения при перезаписи данных, когда закончилась память  при записи архива
extern float tVs;/*текущее время в сек.*/


void FZavN(void);
 /*Формирование зав. номера и даты изг-я для вывода из файла "TekInf.txt" */
 void FND(void);
  /*Запись заводского номера в ZnTekI(ZnTekIс) при вводе*/
 void ZapZN(void);
 void ZapDI(void);
 void FDIzg(void);
void FND(void);
void RTekI(void);
void ChtArx(void);
void WTekI(void);
void ZapArx(void);
/*Чтение архива из внешнего файла и запись его в массив ZnArxI[i](ZnArxIc[i])*/
 void ChtRArx(void); 
void ZapRArx(void); /* Запись в архив*/
	 
void EraseRabArx(void);
void EraseArx(void);
void EraseTekI(void);
void InitMemory(void);

static void read_record(uint32_t base, uint32_t index);

#endif //__MEMORY_FUNCTIONS_H
