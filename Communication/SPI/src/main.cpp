#include <Arduino.h>
#include "driver/spi_slave.h"
#include "esp_crc.h"
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"
#include "Constans.h"


// --- SPI Konfiguráció ---
#define SPI_HOST_ID   SPI2_HOST
#define SPI_MISO      12
#define SPI_MOSI      13
#define SPI_SCLK      11
#define SPI_CS        7

#define PACKET_SIZE 6 // cmd, p1, p2, p3, crc_low, crc_high
#define PAYLOAD_SIZE 4 // Az adatbájtok száma, amire a CRC-t számoljuk
WORD_ALIGNED_ATTR uint8_t spi_rx_buf[PACKET_SIZE];
WORD_ALIGNED_ATTR uint8_t spi_tx_buf[PACKET_SIZE];

// --- Perifériák ---
Led led;
BNO08xGyro gyro;
ColorSensor colorSensor;

// --- Válasz előkészítő segédfüggvény ---
void prepare_response(uint8_t rsp_code, uint8_t d1 = 0, uint8_t d2 = 0, uint8_t d3 = 0) {
    spi_tx_buf[0] = rsp_code;
    spi_tx_buf[1] = d1;
    spi_tx_buf[2] = d2;
    spi_tx_buf[3] = d3;
    
    // CRC-16 számítása az első 4 bájtra hardveres gyorsítással
    uint16_t crc = esp_rom_crc16_le(0xFFFF, spi_tx_buf, PAYLOAD_SIZE);
    
    spi_tx_buf[4] = crc & 0xFF;        // Low byte
    spi_tx_buf[5] = (crc >> 8) & 0xFF; // High byte
}

void setup() {
    Serial.begin(115200);
    led.begin();
    colorSensor.begin();
    gyro.begin();
    led.off();

    // SPI busz konfigurációja
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_SCLK,
        .quadwp_io_num = -1, 
        .quadhd_io_num = -1, 
        .max_transfer_sz = PACKET_SIZE
    };
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = SPI_CS, .flags = 0, .queue_size = 1, .mode = 0
    };
    esp_err_t ret = spi_slave_initialize(SPI_HOST_ID, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.println("Hiba az SPI slave inicializálásakor!");
        return;
    }

    // Kezdeti "ACK" válasz előkészítése az első tranzakcióhoz
    prepare_response(RSP_ACK, 0, 0, 0);
    Serial.println("SPI Slave (CRC-16) készen áll.");
}

void processCommand() {
    // 1. CRC ellenőrzése
    uint16_t received_crc = (spi_rx_buf[5] << 8) | spi_rx_buf[4];
    uint16_t calculated_crc = esp_rom_crc16_le(0xFFFF, spi_rx_buf, PAYLOAD_SIZE);

    if (received_crc != calculated_crc) {
        prepare_response(RSP_ERROR_CRC);
        Serial.printf("CRC Hiba! Kapott: %04X, Számolt: %04X\n", received_crc, calculated_crc);
        return;
    }

    // 2. Parancs feldolgozása (a logika változatlan)
    uint8_t cmd = CMD_TEST;
    uint8_t param1 = 1;
    uint8_t param2 = 2;

  switch (cmd) {
    case CMD_PING:
    prepare_response(RSP_PONG);
  break;
    case CMD_RESTART:
    prepare_response(RSP_ACK, cmd);
    delay(100);
    ESP.restart();
    break;

  case CMD_READ_GYRO:
    if (gyro.update()) {
      float roll, pitch, yaw;
      gyro.getEuler(roll, pitch, yaw);
      Serial.print("Roll: "); Serial.print(roll);
      Serial.print(" Pitch: "); Serial.print(pitch);
      Serial.print(" Yaw: "); Serial.println(yaw);
    } else {
      Serial.println("Gyro update failed. No new data.");
    }
    break; // HIÁNYZÓ BREAK!
  case CMD_CALIBRATE_GYRO:
    // Példa: 90 fokra kalibrálás
    gyro.calibrateToAngle(param1);
    prepare_response(RSP_ACK, cmd); // Nyugta küldése a kalibráció után
    break;
  case CMD_READ_COLOR_SENSOR:
    uint16_t value;
    colorSensor.readOneChannel(param2, value);
    Serial.print("Color channel ");
    Serial.print(param2);
    Serial.print("value : ");
    Serial.println(value);
    break;
  case CMD_SET_LED:
    led.handleColor(param1);
    // Nyugta: ACK, az eredeti parancsra, az eredeti paraméterrel
    prepare_response(RSP_ACK, cmd, param1);
    break;
  case CMD_TEST:
    // Teszt parancs: csak visszaküld egy PONG választ
    prepare_response(RSP_PONG);
    
    Serial.println("Teszt parancs fogadva, PONG válasz küldve.");
    break;


  default:
    prepare_response(RSP_UNKNOWN_CMD, cmd); // Ismeretlen parancs jelzése
    break;
  }
}

void loop() {
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = PACKET_SIZE * 8;
    t.rx_buffer = spi_rx_buf;
    t.tx_buffer = spi_tx_buf;

    if (spi_slave_transmit(SPI_HOST_ID, &t, portMAX_DELAY) == ESP_OK) {
        processCommand();
    }
}