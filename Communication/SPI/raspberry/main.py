import spidev
import time

# SPI busz és eszköz inicializálása
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI busz 0, eszköz 0 (CS0 - GPIO 8)

# SPI beállítások
spi.max_speed_hz = 1000000
spi.mode = 0

counter = 0

try:
    while True:
        # Adat előkészítése küldésre (egy byte)
        to_send = [counter]

        # Adatcsere (küldés és fogadás egyszerre)
        # Az xfer2 metódus listát vár és listát ad vissza
        received_data = spi.xfer2(to_send)

        print(f"Küldve: {to_send[0]}, Fogadva: {received_data[0]}")

        # Számláló növelése (és körbejárása 255 után)
        counter = (counter + 1) % 256

        # Várakozás a következő ciklus előtt
        time.sleep(1)

except KeyboardInterrupt:
    spi.close()
    print("\nProgram leállítva.")