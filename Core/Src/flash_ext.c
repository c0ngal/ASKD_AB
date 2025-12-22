#include "flash_ext.h"


static SPI_HandleTypeDef *s_hspi = NULL;
static GPIO_TypeDef *s_cs_port = NULL;
static uint16_t s_cs_pin = 0;

extern SPI_HandleTypeDef hspi1;

static inline void CS_L(void){ HAL_GPIO_WritePin(s_cs_port, s_cs_pin, GPIO_PIN_RESET); }
static inline void CS_H(void){ HAL_GPIO_WritePin(s_cs_port, s_cs_pin, GPIO_PIN_SET); }

static HAL_StatusTypeDef txrx(const uint8_t *tx, uint8_t *rx, size_t n){
    return HAL_SPI_TransmitReceive(s_hspi, (uint8_t*)tx, rx, n, FLASH_TIMEOUT_MS);
}
static HAL_StatusTypeDef tx(const uint8_t *tx, size_t n){
    return HAL_SPI_Transmit(s_hspi, (uint8_t*)tx, n, FLASH_TIMEOUT_MS);
}
static HAL_StatusTypeDef rx(uint8_t *rxbuf, size_t n){
    return HAL_SPI_Receive(s_hspi, rxbuf, n, FLASH_TIMEOUT_MS);
}
static HAL_StatusTypeDef write_enable(void){
    uint8_t cmd = CMD_WRITE_ENABLE;
    CS_L();
    HAL_StatusTypeDef st = tx(&cmd,1);
    CS_H();
    return st;
}

void flash_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin){
    s_hspi = hspi;
    s_cs_port = cs_port;
    s_cs_pin  = cs_pin;
    CS_H(); // чип не выбран по умолчанию
}

HAL_StatusTypeDef flash_wait_busy(uint32_t timeout_ms){
    uint32_t start = HAL_GetTick();
    uint8_t cmd = CMD_READ_STATUS1;
    uint8_t sr  = 0;
    do{
        CS_L();
        HAL_StatusTypeDef st = tx(&cmd,1);
        if(st!=HAL_OK){ CS_H(); return st; }
        st = rx(&sr,1);
        CS_H();
        if(st!=HAL_OK) return st;
        if((sr & SR_WIP)==0) return HAL_OK;
        HAL_Delay(1);
    }while(HAL_GetTick()-start < timeout_ms);
    return HAL_TIMEOUT;
}

HAL_StatusTypeDef flash_read(uint32_t addr, uint8_t *buf, size_t len){
    uint8_t hdr[4] = { CMD_READ_DATA,
                       (uint8_t)(addr>>16),
                       (uint8_t)(addr>>8),
                       (uint8_t)(addr) };
    CS_L();
    //HAL_StatusTypeDef st = tx(hdr, sizeof(hdr));
    HAL_SPI_StateTypeDef st = HAL_SPI_GetState(s_hspi);
    if(st==HAL_OK) st = rx(buf, len);
    CS_H();
    return st;
}

HAL_StatusTypeDef flash_erase_sector(uint32_t sector_addr){
    HAL_StatusTypeDef st;
    if((st = write_enable())!=HAL_OK) return st;
    uint8_t cmd[4] = { CMD_SECTOR_ERASE,
                       (uint8_t)(sector_addr>>16),
                       (uint8_t)(sector_addr>>8),
                       (uint8_t)(sector_addr) };
    CS_L();
    st = tx(cmd, sizeof(cmd));
    CS_H();
    if(st!=HAL_OK) return st;
    return flash_wait_busy(40000); // стирание долгое
}

HAL_StatusTypeDef flash_write(uint32_t addr, const uint8_t *buf, size_t len){
    HAL_StatusTypeDef st;
    while(len){
        // граница страницы
        uint32_t page_off = addr % FLASH_PAGE_SIZE;
        uint32_t chunk = FLASH_PAGE_SIZE - page_off;
        if(chunk > len) chunk = len;

        if((st = write_enable())!=HAL_OK) return st;

        uint8_t hdr[4] = { CMD_PAGE_PROGRAM,
                           (uint8_t)(addr>>16),
                           (uint8_t)(addr>>8),
                           (uint8_t)(addr) };
        CS_L();
        st = tx(hdr, sizeof(hdr));
        if(st==HAL_OK) st = tx(buf, chunk);
        CS_H();
        if(st!=HAL_OK) return st;

        st = flash_wait_busy(1000);
        if(st!=HAL_OK) return st;

        addr += chunk;
        buf  += chunk;
        len  -= chunk;
    }
    return HAL_OK;


}



uint32_t flash_read_id(void){
    uint8_t cmd = CMD_READ_ID;
    uint8_t id[3] = {0};
    CS_L();
    tx(&cmd,1);
    rx(id,3);
    CS_H();
    return ((uint32_t)id[0]<<16)|((uint32_t)id[1]<<8)|id[2];
}





