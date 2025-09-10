// communication.cpp

#include "communication.h"
#include "command_handler.h" // Hogy hívhassa a parancsfeldolgozót
#include "esp_crc.h"
#include "Constans.h"
#include <Arduino.h>
#include "SPI.h"
#include "driver/spi_slave.h"


// Globális bufferek, de csak ebben a file-ban látszódnak
namespace {
    WORD_ALIGNED_ATTR uint8_t spi_rx_buf[PACKET_SIZE];
    WORD_ALIGNED_ATTR uint8_t spi_tx_buf[PACKET_SIZE];
}

void setupSpiSlave() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = PACKET_SIZE
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = SPI_CS,
        .flags = 0,
        .queue_size = 1,
        .mode = 0
    };

    esp_err_t ret = spi_slave_initialize(SPI_HOST_ID, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.println("Hiba az SPI slave inicializálásakor!");
        return;
    }
    
    prepare_response(RSP_ACK, 0, 0, 0); // Kezdeti válasz
    Serial.println("SPI Slave (CRC-16) készen áll.");
}

void prepare_response(uint8_t rsp_code, uint8_t d1, uint8_t d2, uint8_t d3) {
    spi_tx_buf[0] = rsp_code;
    spi_tx_buf[1] = d1;
    spi_tx_buf[2] = d2;
    spi_tx_buf[3] = d3;
    
    uint16_t crc = esp_rom_crc16_le(0xFFFF, spi_tx_buf, PAYLOAD_SIZE);
    
    spi_tx_buf[4] = crc & 0xFF;
    spi_tx_buf[5] = (crc >> 8) & 0xFF;
}

void handleSpiTransaction() {
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = PACKET_SIZE * 8;
    t.rx_buffer = spi_rx_buf;
    t.tx_buffer = spi_tx_buf;

    if (spi_slave_transmit(SPI_HOST_ID, &t, portMAX_DELAY) == ESP_OK) {
        processCommand(spi_rx_buf);
    }
}