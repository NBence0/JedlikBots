#pragma once
#include <Arduino.h>

// --- Parancsok ---
const uint8_t CMD_PING              = 0;
const uint8_t CMD_RESTART           = 1;
const uint8_t CMD_READ_GYRO         = 2;
const uint8_t CMD_CALIBRATE_GYRO    = 3;
const uint8_t CMD_READ_COLOR_SENSOR = 4;
const uint8_t CMD_READ_BME          = 5;
const uint8_t CMD_SET_LED           = 7;
const uint8_t CMD_TEST              = 100;
const uint8_t CMD_NOP               = 255;


// --- Válaszok ---
const uint8_t RSP_ACK               = 100; 
const uint8_t RSP_PONG              = 101;
const uint8_t RSP_GYRO_DATA         = 102;
const uint8_t RSP_COLOR_DATA        = 103;
const uint8_t RSP_BME_DATA          = 104;
const uint8_t RSP_ERROR_CRC         = 200;
const uint8_t RSP_ERROR_GYRO        = 201;
const uint8_t RSP_UNKNOWN_CMD       = 201;

// --- SPI Konfiguráció a kommunikációs bus ---
#define SPI_HOST_ID   SPI2_HOST
#define SPI_MISO      13
#define SPI_MOSI      11
#define SPI_SCLK      12
#define SPI_CS        10

// --- SPI Konfiguráció az öszes spi eszközhöz ---
#define SPI2_HOST_ID   SPI3_HOST
#define SPI2_MISO      20
#define SPI2_MOSI      21
#define SPI2_SCLK      22
#define SPI2_CS        23


#define PACKET_SIZE 6 // cmd, p1, p2, p3, crc_low, crc_high
#define PAYLOAD_SIZE 4 // Az adatbájtok száma, amire a CRC-t számoljuk

#define LED_PIN     38
#define LED_COUNT   8

#define DATA_READY_PIN 25 // esp és RPI közti pin

/*// --- SPI Multiplexer (74HC154) ---
#define MUX_PIN_A 2
#define MUX_PIN_B 3
#define MUX_PIN_C 4
#define MUX_PIN_D 5

// Eszközök csatornái
#define MUX_CHANNEL_GYRO 0
#define MUX_CHANNEL_BME 1
#define MUX_CHANNEL_BNO_CS 2
// ... stb.
#define MUX_NO_DEVICE_SELECTED 15 // Egy nem használt csatorna a kikapcsoláshoz*/



// test CS lábak
#define BME280_CS    1
#define BNO08X_CS    35



#define BNO08X_INT    36
#define BNO08X_RESET  48
