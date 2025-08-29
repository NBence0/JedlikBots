from color_recognition import ColorRecognizer
from uart_sending import SerialComm
import time
import serial

# UART inicializálása (állítsd be a megfelelő portot)


color_recognizer = ColorRecognizer()
comm = SerialComm(port="/dev/ttyS0", baudrate=576000)
comm.send_command(0)
time.sleep(0.5)

COLOR_CODES = {
    "Red": 1,
    "Yellow": 2,
    "Green": 3,
    "Blue": 4,
    "Purple": 5,
    "Unknown": 6,
}

for i in range(20000):
    color=color_recognizer.get_color(640, 360)
    color_code = COLOR_CODES.get(color, 0xFF)
    comm.send_command(7, color_code)
    time.sleep(0.01)
comm.send_command(0)
time.sleep(5)
comm.send_command(7, 5)
