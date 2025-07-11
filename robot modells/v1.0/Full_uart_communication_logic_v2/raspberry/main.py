# File: main.py
import time
from color_recognition import ColorRecognizer
from serial_protocol import SerialComm, CMD_SET_LED, CMD_PING, CMD_RESTART, RSP_ACK

SERIAL_PORT = "/dev/ttyS0"
BAUDRATE = 576000

COLOR_CODES = {
    "Red": 1,
    "Yellow": 2,
    "Green": 3,
    "Blue": 4,
    "Purple": 5,
    "Unknown": 6,
}

def main():
    # init
    color_recognizer = ColorRecognizer()
    comm = SerialComm(port=SERIAL_PORT, baudrate=BAUDRATE)
    
    if not comm.is_opened():
        color_recognizer.stop()
        print("Exiting due to serial port error.")
        return

    """
    if comm.execute_command(CMD_PING):
        print("Connection successfully established.")
    else:
        print("Error: ESP32 is not responding. Please check the connection and power supply.")
        comm.close()
        color_recognizer.stop()
        return
    """
    try:
        while True:
            color_name = color_recognizer.get_color(640, 360)
            color_code = COLOR_CODES.get(color_name, COLOR_CODES["Unknown"])
            
            response = comm.execute_command(CMD_SET_LED, color_code, quiet=True)
            
            if response:
                # Simplified logging to avoid flooding the console
                if response["cmd"] == RSP_ACK and response["param2"] == color_code:
                    # print(f"Successfully set: {color_name} ({color_code})")
                    pass
                else:
                    print(f"Unexpected response received: {response}")
            else:
                # No response is a critical error
                print("Error: Communication with ESP32 lost.")
                break # Exit loop
            
            time.sleep(0.05) # Loop time control

    finally:
        if comm.is_opened():
            comm.execute_command(CMD_SET_LED, 0) # "0" turns off the LED
            comm.close()
        color_recognizer.stop()

if __name__ == "__main__":
    main()
