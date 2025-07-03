import serial
import time

ser = serial.Serial("/dev/serial0", 115200, timeout=1)
print("Kérés-válasz üzemmód")

while True:
    ser.write(b"GET\n")  # Kérés küldése
    time.sleep(0.01)      # Várjuk a választ

    response = ser.readline().decode().strip()
    if response:
        print(f"Válasz az ESP32-től: {response}")
