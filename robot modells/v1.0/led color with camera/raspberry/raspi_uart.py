import serial
import time

ser = serial.Serial("/dev/serial0", 9600, timeout=1)
print("Kérés-válasz üzemmód")

while True:
    ser.write(b"GET\n")  # Kérés küldése
    time.sleep(0.5)      # Várjuk a választ

    response = ser.readline().decode().strip()
    if response:
        print(f"Válasz az ESP32-től: {response}")

    time.sleep(2)  # Következő kérés 2 mp múlva
