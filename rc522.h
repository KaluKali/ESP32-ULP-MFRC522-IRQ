#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"

#define PCD_IDLE						0x00   //NO action; Cancel the current command
#define PCD_AUTHENT						0x0E   //Authentication Key
#define PCD_RECEIVE						0x08   //Receive Data
#define PCD_TRANSMIT					0x04   //Transmit data
#define PCD_TRANSCEIVE					0x0C   //Transmit and receive data,
#define PCD_RESETPHASE					0x0F   //Soft Reset
#define PCD_CALCCRC						0x03   //CRC Calculate

/* Mifare_One card command word */
#define PICC_REQIDL						0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL						0x52   // find all the cards antenna area
#define PICC_ANTICOLL					0x93   // anti-collision
#define PICC_SElECTTAG					0x93   // election card
#define PICC_AUTHENT1A					0x60   // authentication key A
#define PICC_AUTHENT1B					0x61   // authentication key B
#define PICC_READ						0x30   // Read Block
#define PICC_WRITE						0xA0   // write block
#define PICC_DECREMENT					0xC0   // debit
#define PICC_INCREMENT					0xC1   // recharge
#define PICC_RESTORE					0xC2   // transfer block data to the buffer
#define PICC_TRANSFER					0xB0   // save the data in the buffer
#define PICC_HALT						0x50   // Sleep

/* MFRC522 Registers */
//Page 0: Command and Status
#define REG_RESERVED00			0x00
#define REG_COMMAND				0x01
#define REG_COMM_IE_N			0x02
#define REG_DIV1_EN				0x03
#define REG_COMM_IRQ			0x04
#define REG_DIV_IRQ				0x05
#define REG_ERROR				0x06
#define REG_STATUS1				0x07
#define REG_STATUS2				0x08
#define REG_FIFO_DATA			0x09
#define REG_FIFO_LEVEL			0x0A
#define REG_WATER_LEVEL			0x0B
#define REG_CONTROL				0x0C
#define REG_BIT_FRAMING			0x0D
#define REG_COLL				0x0E
#define REG_RESERVED01			0x0F
//Page 1: Command
#define REG_RESERVED10			0x10
#define REG_MODE				0x11
#define REG_TX_MODE				0x12
#define REG_RX_MODE				0x13
#define REG_TX_CONTROL			0x14
#define REG_TX_AUTO				0x15
#define REG_TX_SELL				0x16
#define REG_RX_SELL				0x17
#define REG_RX_THRESHOLD		0x18
#define REG_DEMOD				0x19
#define REG_RESERVED11			0x1A
#define REG_RESERVED12			0x1B
#define REG_MIFARE				0x1C
#define REG_RESERVED13			0x1D
#define REG_RESERVED14			0x1E
#define REG_SERIALSPEED			0x1F
//Page 2: CFG
#define REG_RESERVED20			0x20
#define REG_CRC_RESULT_M		0x21
#define REG_CRC_RESULT_L		0x22
#define REG_RESERVED21			0x23
#define REG_MOD_WIDTH			0x24
#define REG_RESERVED22			0x25
#define REG_RF_CFG				0x26
#define REG_GS_N				0x27
#define REG_CWGS_PREG			0x28
#define REG__MODGS_PREG			0x29
#define REG_T_MODE				0x2A
#define REG_T_PRESCALER			0x2B
#define REG_T_RELOAD_H			0x2C
#define REG_T_RELOAD_L			0x2D
#define REG_T_COUNTER_VALUE_H	0x2E
#define REG_T_COUNTER_VALUE_L	0x2F
//Page 3:TestRegister
#define REG_RESERVED30			0x30
#define REG_TEST_SEL1			0x31
#define REG_TEST_SEL2			0x32
#define REG_TEST_PIN_EN			0x33
#define REG_TEST_PIN_VALUE		0x34
#define REG_TEST_BUS			0x35
#define REG_AUTO_TEST			0x36
#define REG_VERSION				0x37
#define REG_ANALOG_TEST			0x38
#define REG_TEST_ADC1			0x39
#define REG_TEST_ADC2			0x3A
#define REG_TEST_ADC0			0x3B
#define REG_RESERVED31			0x3C
#define REG_RESERVED32			0x3D
#define REG_RESERVED33			0x3E
#define REG_RESERVED34			0x3F

typedef void(*rc522_tag_callback_t)(uint8_t*);

typedef struct {
    int miso_io;
    int mosi_io;
    int sck_io;
    int sda_io;
    rc522_tag_callback_t callback;
} rc522_start_args_t;

esp_err_t rc522_spi_init(int miso_io, int mosi_io, int sck_io, int sda_io);
esp_err_t rc522_write_n(uint8_t addr, uint8_t n, uint8_t *data);
esp_err_t rc522_write(uint8_t addr , uint8_t val);
uint8_t* rc522_read_n(uint8_t addr, uint8_t n) ;
uint8_t rc522_read(uint8_t addr);
#define rc522_fw_version() rc522_read(0x37)
esp_err_t rc522_init();

esp_err_t rc522_set_bitmask(uint8_t addr, uint8_t mask);
esp_err_t rc522_clear_bitmask(uint8_t addr, uint8_t mask);
esp_err_t rc522_antenna_on();
uint8_t* rc522_calculate_crc(uint8_t *data, uint8_t n);
uint8_t* rc522_card_write(uint8_t cmd, uint8_t *data, uint8_t n, uint8_t* res_n);
uint8_t* rc522_request(uint8_t* res_n);
uint8_t* rc522_anticoll();
uint8_t* rc522_get_tag();
esp_err_t rc522_start(rc522_start_args_t start_args);
esp_err_t rc522_resume();
esp_err_t rc522_pause();

#ifdef __cplusplus
}
#endif
