#include "Flash_SST25.h"
#include "stm32f4xx_hal.h"                  // Device header
#include "delay.h"

extern SPI_HandleTypeDef hspi1;
extern struct tLCTableFile  FileLCTable;
extern struct tLCTableFile  FileLCTableCopy;


int Rd_IDexROM(void){		 //������ ID SST25.
	uint8_t ID[5] = {0};
	uint8_t CmdReadID = 0x90;
	uint8_t Addr[3] = {0};

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdReadID,1,100);
	HAL_SPI_Transmit(&hspi1,&Addr,1,100);
	HAL_SPI_TransmitReceive(&hspi1,Addr,ID,5,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	return ID[4];
}
void Write_Enable(void){      //���������� ������.
	uint8_t CmdWE = 0x6;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdWE,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

void Write_Disable(void){      //���������� ������.
	uint8_t CmdWD = 0x04;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdWD,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

uint8_t Rd_Status_Reg(void){ //������ �������� ������� ������ 1636��4�.
	uint8_t CmdRdStatus = 0x05;
	uint8_t nullTxForRx[1] = {0};
	uint8_t status = 0;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdRdStatus,1,100);
	HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,&status,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	return status;
}

void Enable_Write_Status_Reg(void){      //���������� ������ � ������� �������
	uint8_t CmdEWSR = 0x50;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdEWSR,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

void Write_Status_Reg(uint8_t status){	 //������ �������� ������� (��� ���������� ��������������� ������)
	uint8_t CmdWrStatus = 0x01;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdWrStatus,1,100);
	HAL_SPI_Transmit(&hspi1,&status,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

/*void Sector4KB_Erase(int adr){	 //�������� ������� 4 kByte
	uint8_t CmdS4KB_Erase = 0x20;
	uint8_t adrArr[3];
	adrArr[0] = (adr>>16)&0xFF;
	adrArr[1] = (adr>>16)&0xFF;
	adrArr[2] = (adr>>16)&0xFF;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdS4KB_Erase,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	HAL_Delay(25);
}*/


void Sector4KB_Erase(int adr){
    uint8_t CmdS4KB_Erase = 0x20;
    uint8_t adrArr[3];
    adrArr[0] = (adr >> 16) & 0xFF;
    adrArr[1] = (adr >>  8) & 0xFF;
    adrArr[2] = (adr      ) & 0xFF;

    Write_Enable();


    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1,&CmdS4KB_Erase,1,100);
    HAL_SPI_Transmit(&hspi1,adrArr,3,100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

    while (Rd_Status_Reg() & 0x01) { /* wait */ }
}


void Sector32KB_Erase(int adr){	 //�������� ������� 32 kByte
	uint8_t CmdS32KB_Erase = 0x52;
	uint8_t adrArr[3];
	adrArr[0] = (adr>>16)&0xFF;
	adrArr[1] = (adr>>16)&0xFF;
	adrArr[2] = (adr>>16)&0xFF;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdS32KB_Erase,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	uint8_t cmd = 0x05;
	uint8_t rx = 1;
	while (rx & 0x01 != 0) {
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1,&cmd,1,100);
		HAL_SPI_Receive(&hspi1,&rx,1,100);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	}



	HAL_Delay(25);
}

void Sector64KB_Erase(int adr){	 //�������� ������� 64 kByte
	uint8_t CmdS64KB_Erase = 0xD8;
	uint8_t adrArr[3];
	adrArr[0] = (adr>>16)&0xFF;
	adrArr[1] = (adr>>16)&0xFF;
	adrArr[2] = (adr>>16)&0xFF;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdS64KB_Erase,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	HAL_Delay(25);
}

void Chip_Erase(void){	 //�������� ����� ����
	uint8_t CmdChip_Erase = 0x60;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdChip_Erase,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	HAL_Delay(50);
}

void ByteProgram(int adr, uint8_t data){//������ �����.
	uint8_t CmdByteProg = 0x02;
	uint8_t adrArr[3];
	adrArr[0] = (adr >> 16) & 0xFF;
	adrArr[1] = (adr >> 8) & 0xFF;
	adrArr[2] = (adr) & 0xFF;

	Write_Enable();

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdByteProg,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_Transmit(&hspi1,&data,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	delay_5us(10);
}

void word32Rw1636RR4(int adr, int data){//������ ����� �� 4-� ����.
	uint8_t i;
	for(i=0;i<4;i++){
		ByteProgram(adr+i,data&0xFF);
		data = data >> 8;
	}
}

void SaveConf(struct tConfFile confData){//������ ����������: �������(4 �����, �������), ������, �����, ������������������������� � ������ ����� flash ������
	int i=0;
	char conf[20];
	int tmpVal = 0;
	conf[0] = confData.freq / 1000;
	conf[1] = confData.freq % 1000 / 100;
	conf[2] = confData.freq % 100 / 10;
	conf[3] = confData.freq % 10;
	conf[4] = confData.ant;
	conf[5] = confData.num;
	tmpVal = (int)(confData.maxPad*100 + 0.5);
	conf[6] = tmpVal / 100;
	conf[7] = (tmpVal % 100) / 10;
	conf[8] = (tmpVal % 10);
	tmpVal = (int)(confData.maxOtr*100 + 0.5);
	conf[9] = tmpVal / 100;
	conf[10] = (tmpVal % 100) / 10;
	conf[11] = (tmpVal % 10);
/*	conf[9] = (uint8_t)(confData.maxOtr);
	conf[10] = (uint8_t)(((confData.maxOtr - conf[9])*10));
	conf[11] = (uint8_t)((((confData.maxOtr - conf[9])*10 - conf[10])*10) + 0.5);
*/
	tmpVal = (int)(confData.VperDB*10000 + 0.5);
	conf[12] = tmpVal / 10000;
	conf[13] = (tmpVal % 10000) / 1000;
	conf[14] = (tmpVal % 1000) / 100;
	conf[15] = (tmpVal % 100) / 10;
	conf[16] = (tmpVal % 10);
/*  conf[12] = (uint8_t)(confData.VperDB);
	conf[13] = (uint8_t)(((confData.VperDB - conf[12])*10));
	conf[14] = (uint8_t)((((confData.VperDB - conf[12])*10-conf[13])*10));
	conf[15] = (uint8_t)(((((confData.VperDB - conf[12])*10-conf[13])*10 - conf[14])*10) + 0.5);
	conf[16] = (uint8_t)((((((confData.VperDB - conf[12])*10-conf[13])*10 - conf[14])*10-conf[15])*10) + 0.5);
*/	tmpVal = (int)(confData.Vem*100 + 0.5);
	conf[17] = tmpVal / 100;
	conf[18] = (tmpVal % 100) / 10;
	conf[19] = (tmpVal % 10);
/*	conf[17] = (uint8_t)(confData.Vem + 0.5);
	conf[18] = (uint8_t)(((confData.Vem - conf[17])*10) + 0.5);
	conf[19] = (uint8_t)((((confData.Vem - conf[17])*10 - conf[18])*10) + 0.5);
*/
	Sector4KB_Erase(paramFileAdr);
	for(i=0;i<20;i++)
	  ByteProgram(paramFileAdr + i,conf[i]);
}

void ClearConf(){//�������� ���������� � ������ ������ flash ������
	Sector4KB_Erase(paramFileAdr);
}
void SaveLCTable(uint8_t numAnt/*, struct tLCTableFile FileLCTable*/){//������ ���������� �� flash ������
	int i=0;
	Sector32KB_Erase(LCTableFileAdr + 0x40000*numAnt);
/*	Sector4KB_Erase(LCTableFileAdr);
	Sector4KB_Erase(LCTableFileAdr+0x1000);
	Sector4KB_Erase(LCTableFileAdr+0x2000);
	Sector4KB_Erase(LCTableFileAdr+0x3000);
	Sector4KB_Erase(LCTableFileAdr+0x4000);*/
	HAL_Delay(30);
	for(i=0;i < TablNRecs;i++){
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 0, (FileLCTable.tables[i].L % 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 1, (FileLCTable.tables[i].L / 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 2, (FileLCTable.tables[i].C1 % 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 3, (FileLCTable.tables[i].C1 / 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 4, (FileLCTable.tables[i].C2 % 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 5, (FileLCTable.tables[i].C2 / 256));
	  ByteProgram(LCTableFileAdr + 0x40000*numAnt + i*7 + 6, (FileLCTable.tables[i].SWR));
	}
}

void SaveCanal(struct tCanalFile FileCanal){//������ ���������� � ������� �� flash ������
	int i=0,j=0;
	Sector4KB_Erase(canalFileAdr);
	Sector4KB_Erase(canalFileAdr+0x1000);
	HAL_Delay(30);
	for(i=0;i < 1000;i++){
		for(j=0;j < 7;j++)
			ByteProgram(canalFileAdr + i*7 + j, FileCanal.canals[i].F[j]);
	  ByteProgram(canalFileAdr + i*7 + 7, FileCanal.canals[i].P);
	}
}




/*void SaveCanal(const struct tCanalFile* FileCanal)
{
    // стереть 8 КБ (два сектора по 4К)
    Write_Enable();
    Sector4KB_Erase(canalFileAdr);
    // flash_wait_wip(2000); // если внутри Sector4KB_Erase нет ожидания

    Write_Enable();
    Sector4KB_Erase(canalFileAdr + 0x1000);
    // flash_wait_wip(2000);

    // пишем по-байтово, шаг = 8
    for (int i = 0; i < 1000; i++) {
        uint32_t off = (uint32_t)i * 8;               // ВАЖНО: *8
        for (int j = 0; j < 7; j++) {
            Write_Enable();
            ByteProgram(canalFileAdr + off + j, FileCanal->canals[i].F[j]);
            // flash_wait_wip(20);
        }
        Write_Enable();
        ByteProgram(canalFileAdr + off + 7, FileCanal->canals[i].P);
        // flash_wait_wip(20);
    }
}*/



struct tConfFile ReadConfFile(void){		//������ ������������ �� ������� ������� (20 ��������) flash ������
	uint8_t CmdDataRead = 0x03;
	uint8_t adrArr[3] = {0};
	uint8_t nullTxForRx[20] = {0};
	struct tConfFile res;

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,(uint8_t*)res.data,20,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	res.freq = res.data[0]*1000 + res.data[1]*100 + res.data[2]*10 + res.data[3];
	res.ant = res.data[4];
	res.num = res.data[5];
	res.maxPad = (res.data[6] + (float)res.data[7] / 10.0 + (float)res.data[8] / 100.0);
	res.maxOtr = (res.data[9] + (float)res.data[10] / 10.0 + (float)res.data[11] / 100.0);
	res.VperDB = (res.data[12] + (float)res.data[13] / 10.0 + (float)res.data[14] / 100.0
							 + (float)res.data[15] / 1000.0 + (float)res.data[16] / 10000.0);
	res.Vem = (res.data[17] + (float)res.data[18] / 10.0 + (float)res.data[19] / 100.0);
	return res;
}


/*struct tLCTableFile*/void ReadLCTableFile(uint8_t antNum){		//������ ������� ���������� �� flash ������
	uint8_t i = 0,j = 0;
	uint8_t CmdDataRead = 0x03;
	uint8_t nullTxForRx[LCTableFileLenPart] = {0};
	struct tLCTableFile res;
	uint8_t LCData [LCTableFileLenPart]= {0};
	uint8_t adrArr[3] = {0};
	for(j=0; j<LCTableFileNumPart;j++){
		adrArr[0] = ((LCTableFileAdr + 0x40000*antNum + j*LCTableFileLenPart) >> 16) & 0xFF;
		adrArr[1] = ((LCTableFileAdr + 0x40000*antNum + j*LCTableFileLenPart) >> 8) & 0xFF;
		adrArr[2] = ((LCTableFileAdr + 0x40000*antNum + j*LCTableFileLenPart)) & 0xFF;

		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
		HAL_SPI_Transmit(&hspi1,adrArr,3,100);
		HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,LCData,LCTableFileLenPart,100);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

		for(i=0; i<50;i++){
			FileLCTable.tables[i+j*50].L = LCData[i*7] + LCData[i*7+1]*256;
			FileLCTable.tables[i+j*50].C1 = LCData[i*7+2] + LCData[i*7+3]*256;
			FileLCTable.tables[i+j*50].C2 = LCData[i*7+4] + LCData[i*7+5]*256;
			FileLCTable.tables[i+j*50].SWR = LCData[i*7+6];
			if (FileLCTable.tables[i+j*50].L > 2047) FileLCTable.tables[i+j*50].L = 2047;
			if (FileLCTable.tables[i+j*50].C1 > 511) FileLCTable.tables[i+j*50].C1 = 511;
			if (FileLCTable.tables[i+j*50].C2 > 127) FileLCTable.tables[i+j*50].C2 = 127;
		}
	}

	//return res;
}

struct tCanalFile ReadCanalFile(void){		//������ ���������� � �������
	uint16_t i = 0,j = 0;
	uint8_t CmdDataRead = 0x03;
	uint8_t nullTxForRx[canalFileLen] = {0};
	struct tCanalFile res;
	uint8_t canalData [canalFileLen]= {0};
	uint8_t adrArr[3] = {0};
	adrArr[0] = (canalFileAdr >> 16) & 0xFF;
	adrArr[1] = (canalFileAdr >> 8) & 0xFF;
	adrArr[2] = (canalFileAdr) & 0xFF;
/* TODO
	HAL_GPIO_WritePin(FSS_FLASH_PORT,FSS_FLASH_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,canalData,canalFileLen,100);
	HAL_GPIO_WritePin(FSS_FLASH_PORT,FSS_FLASH_PIN,GPIO_PIN_SET);

	for(i=0; i<(canalFileLen/8);i++){
		for(j=0; j<7;j++)
  		res.canals[i].F[j] = canalData[i*8+j];
  		res.canals[i].P = canalData[i*8+7];
	}
*/
	return res;
}

struct tLCTable ReadLCTable(uint8_t antNum, uint16_t tableNum){
	//uint8_t i = 0;
	uint8_t CmdDataRead = 0x03;
	uint8_t nullTxForRx[7] = {0};
	struct tLCTable res;
	uint8_t LCData [7]= {0};
	uint8_t adrArr[3] = {0};
	adrArr[0] = ((LCTableFileAdr + 0x40000*antNum + tableNum*7) >> 16) & 0xFF;
	adrArr[1] = ((LCTableFileAdr + 0x40000*antNum + tableNum*7) >> 8) & 0xFF;
	adrArr[2] = ((LCTableFileAdr + 0x40000*antNum + tableNum*7)) & 0xFF;

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,LCData,7,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

	res.L = LCData[0] + LCData[1]*256;
	res.C1 = LCData[2] + LCData[3]*256;
	res.C2 = LCData[4] + LCData[5]*256;
	res.SWR = LCData[6];

	return res;
}

void WriteLCTable(uint8_t antNum, uint16_t tableNum,struct tLCTable LCTable){
}


void Flash_PageProgram(uint32_t addr, uint8_t *data, uint16_t len) {
    uint8_t cmd[4];
    cmd[0] = 0x02; // PAGE PROGRAM
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8)  & 0xFF;
    cmd[3] = (addr)       & 0xFF;
    uint8_t *p = data;
    uint32_t adr = addr;
    uint8_t dr;
    for ( ; adr < addr + len; ++adr) {
    	ByteProgram(adr,*data++);
    	//dr = data[adr];
    	//Flash_Read(adr, &dr, 1);
    }

}

void Flash_Read(uint32_t addr, uint8_t *data, uint16_t len) {
	uint8_t CmdDataRead = 0x03;
	uint8_t adrArr[3] = {0};
	uint8_t nullTxForRx[20] = {0};
    adrArr[0] = (addr >> 16) & 0xFF;
    adrArr[1] = (addr >>  8) & 0xFF;
    adrArr[2] = (addr      ) & 0xFF;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&CmdDataRead,1,100);
	HAL_SPI_Transmit(&hspi1,adrArr,3,100);
	HAL_SPI_Receive(&hspi1, data, len, 100);
	//HAL_SPI_TransmitReceive(&hspi1,nullTxForRx,data,len,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}
