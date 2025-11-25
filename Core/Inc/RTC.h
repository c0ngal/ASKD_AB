#ifndef __RTC_H
#define __RTC_H

//#include "opora.h"
#include <stdint.h>

void setTime(uint8_t hour, uint8_t min, uint8_t sec);
uint32_t getTime(void);
uint8_t getHours(void);
uint8_t getMinutes(void);
uint8_t getSeconds(void);
uint8_t getWeekDay(uint16_t year, uint8_t month, uint8_t day);
void setDate(uint16_t year, uint8_t month, uint8_t day);	//исчисление с 01.01.2000 раньше даты работают не корректно
uint16_t getYear(void);
uint8_t getMonth(void);
uint8_t getDay(void);	//день в месяце
//extern uint16_t daysInMonth[12];	//количество дней с начала года если начался соответствующий месяц

#endif //RTC_H
