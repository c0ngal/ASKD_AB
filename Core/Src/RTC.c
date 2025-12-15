#include "RTC.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"


extern RTC_HandleTypeDef hrtc;

void MX_RTC_Init(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    HAL_RTC_Init(&hrtc);
}


void setTime(uint8_t hour, uint8_t min, uint8_t sec){
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hour;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

uint32_t getTime(){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds;
}

uint8_t getHours(){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sTime.Hours;
}

uint8_t getMinutes(){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sTime.Minutes;
}

uint8_t getSeconds(){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sTime.Seconds;
}

uint8_t getWeekDay(uint16_t year, uint8_t month, uint8_t day) {
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    uint16_t K = year % 100;
    uint16_t J = year / 100;
    uint8_t h = (day + 13*(month + 1)/5 + K + K/4 + J/4 + 5*J) % 7;
    // h: 0 = Saturday, 1 = Sunday, 2 = Monday ...
    uint8_t weekday = ((h + 5) % 7) + 1; // 1 = Monday ... 7 = Sunday
    return weekday;
}

void setDate(uint16_t year, uint8_t month, uint8_t day){	//исчисление с 01.01.2000 раньше даты работают не корректно
	RTC_DateTypeDef sDate;

	sDate.Year = year % 100; //1-99
	sDate.Month = month;	 //1-12
	sDate.Date = day;		 //1-31
	sDate.WeekDay = getWeekDay(year, month, day);

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

uint16_t getYear(){//возвращает значение за вычитом 2000 (2011г. -> 11, 2102г. -> 102)
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return /*2000 +*/ sDate.Year;
}
uint8_t getMonth(){
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sDate.Month;
}

uint8_t getDay(){	//день в месяце
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return sDate.Date;
}
