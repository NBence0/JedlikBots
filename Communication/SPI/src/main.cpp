#include <Arduino.h>
#include "driver/spi_slave.h"

// ESP32-S3 SPI pin definíciók
#define SPI_HOST_ID   SPI2_HOST // ESP32-S3-on használjuk az SPI2-t (régi nevén HSPI)
#define SPI_MISO      14
#define SPI_MOSI      15
#define SPI_SCLK      16
#define SPI_CS        5

// Puffer a bejövő és kimenő adatoknak
#define BUFFER_SIZE 64
WORD_ALIGNED_ATTR uint8_t spi_rx_buf[BUFFER_SIZE];
WORD_ALIGNED_ATTR uint8_t spi_tx_buf[BUFFER_SIZE];

// Callback függvény, ami egy tranzakció UTÁN hívódik meg
// Itt készítjük elő a következő tranzakcióhoz a választ.
void IRAM_ATTR post_trans_callback(spi_slave_transaction_t *trans) {
    // A fogadott adat (spi_rx_buf[0]) alapján előkészítjük a következő választ
    spi_tx_buf[0] = spi_rx_buf[0] + 1;
}

void setup() {
    Serial.begin(115200);

    // SPI busz konfigurációja
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_SCLK,
        .quadwp_io_num = -1, // Nincs használatban
        .quadhd_io_num = -1, // Nincs használatban
        .max_transfer_sz = BUFFER_SIZE
    };

    // SPI slave interfész konfigurációja
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = SPI_CS,
        .flags = 0,
        .queue_size = 1, // Egyszerre egy tranzakciót kezelünk
        .mode = 0, // SPI mód 0
        .post_setup_cb = NULL,
        .post_trans_cb = post_trans_callback // Callback a tranzakció után
    };

    // SPI busz inicializálása
    esp_err_t ret = spi_slave_initialize(SPI_HOST_ID, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.println("Hiba az SPI slave inicializálásakor!");
        return;
    }

    Serial.println("ESP32-S3 SPI Slave készen áll.");

    // Előkészítjük az első választ (ha a Pi nullát küld, mi egyet válaszolunk)
    spi_tx_buf[0] = 1; 
}

void loop() {
    // Tranzakció beállítása és várólistába helyezése
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 8; // 8 bites (1 bájtos) tranzakció
    t.rx_buffer = spi_rx_buf;
    t.tx_buffer = spi_tx_buf;

    // Várakozás a master tranzakciójára.
    // A portMAX_DELAY biztosítja, hogy a feladat blokkolódik, amíg a tranzakció le nem zajlik.
    esp_err_t ret = spi_slave_transmit(SPI_HOST_ID, &t, portMAX_DELAY);

    if (ret == ESP_OK) {
        // A fogadott adatot a callback már feldolgozta, de itt is kiírhatjuk
        Serial.printf("Fogadva: %d | Következő küldendő: %d\n", spi_rx_buf[0], spi_tx_buf[0]);
    }

    // A loop itt várakozik a következő tranzakcióra.
    // Nincs szükség delay-re.
}