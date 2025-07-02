import serial
import time

# Nyisd meg a serial portot – az alap UART eszköz: /dev/serial0
ser = serial.Serial('/dev/serial0', 115200, timeout=1)
time.sleep(2)  # ESP32 feléledési idő

# Küldj egy parancsot az ESP32-nek
ser.write(b'GET\n')

# Várj és olvass választ
response = ser.readline().decode().strip()

print(f"Kapott szám az ESP32-től: {response}")

ser.close()
