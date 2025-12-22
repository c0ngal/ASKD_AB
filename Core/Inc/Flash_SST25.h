#ifndef __FLASH_SST25_H__
#define __FLASH_SST25_H__
#include <stdio.h>
#include <stdint.h>
#include "global.h"


#define paramFileAdr 0	//����� ����������: �������(4 �����, �������), ������, �����, �������������
#define LCTableFileAdr 0x40000	//����� ������� ��� �������� ������� LC
#define canalFileAdr 0xC0000	//����� ������� ��� �������� ���������� �������
//#define LCTableFileLen 7*TablNRecs	//������ ������ 7 ���� �� ������, 2850 �������
//������� ������� ������, ������� ��������� �� �����
#define LCTableFileLenPart 7*50	//������ ������ 7 ���� �� ������, 50 �� 2850 �������
#define LCTableFileNumPart 57	//��������� ������ ����� ��� ������ (2850/50=57)
#define canalFileLen /*1000*/100*8	//������ ������ 8 ���� �� �����, 1000 �������
#define  TablNRecs  2850 /* 3000 - 150 */

struct tLCTable{
             uint16_t L;
             uint16_t C1;
             uint16_t C2;
             uint8_t SWR;
};
struct tLCTableFile{
	struct tLCTable tables[TablNRecs];
};

struct tCanal{
            char F[7];
            char P;
};
struct tCanalFile{
	struct tCanal canals[100];
};

struct tMyFactor{
		char data [7];
};

struct tConfFile{
	char data[20];
	int freq;
	uint8_t ant;
	uint8_t num;
	float maxPad;
	float maxOtr;
	float VperDB;
	float Vem;
};

int Rd_IDexROM(void);		 //������ ID 1636��4�.
void Write_Enable(void);      //���������� ������.
void Write_Disable(void);      //���������� ������.
uint8_t Rd_Status_Reg(void); //������ �������� �������
void Enable_Write_Status_Reg(void);      //���������� ������ � ������� �������
void Write_Status_Reg(uint8_t);	 //������ �������� ������� (��� ���������� ��������������� ������)
void Sector4KB_Erase(int);	 //�������� �������.
void Sector32KB_Erase(int);	 //�������� �������.
void Sector64KB_Erase(int);	 //�������� �������.
void Chip_Erase(void);	 //�������� �������.
void ByteProgram(int, uint8_t);//������ �����.
void word32Rw1636RR4(int, int);//������ ����� �� 4-� ����.
void SaveConf(struct tConfFile);//������ ������������ � ������ ����� flash ������
void ClearConf(void);//�������� ������������ � ������ ������ flash ������
void SaveLCTable(uint8_t/*, struct tLCTableFile*/);//������ LC �� flash ������
void SaveCanal(struct tCanalFile);//������ ������� �� flash ������
struct tConfFile ReadConfFile(void);		//������ ������������ �� ������� ������� (6 ��������) flash ������
/*struct tLCTableFile*/ void ReadLCTableFile(uint8_t);		//������ ���������� L � C �� flash ������
struct tCanalFile ReadCanalFile(void);		//������ ���������� ������� �� flash ������
struct tLCTable ReadLCTable(uint8_t, uint16_t);		//������ ���������� L � C �� flash ������
void WriteLCTable(uint8_t,uint16_t,struct tLCTable);		//������ ���������� L � C �� flash ������
void Flash_Read(uint32_t addr, uint8_t *data, uint16_t len);
void Flash_PageProgram(uint32_t addr, uint8_t *data, uint16_t len);
#endif //__FLASH_SST25_H__
