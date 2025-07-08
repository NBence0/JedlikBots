from color_recognition import ColorRecognizer
import time
import serial

# UART inicializálása (állítsd be a megfelelő portot)
ser = serial.Serial('/dev/ttyS0', baudrate=115200, timeout=1)

recognizer = ColorRecognizer()
time.sleep(2)

# Színkódok hozzárendelése
COLOR_CODES = {
    "Red":     0x01,
    "Green":   0x02,
    "Blue":    0x03,
    "Yellow":  0x04,
    "Unknown": 0xFF
}

def send_color(color_name):
    color_code = COLOR_CODES.get(color_name, 0xFF)
    ser.write(bytes([color_code]))
    print(color_code)

# Folyamatos futás
try:
    while True:
        color = recognizer.get_color(x=640, y=360)
        print("Detected color:", color)
        send_color(color)
        time.sleep(0.1)
finally:
    recognizer.stop()
    ser.close()
