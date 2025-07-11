# File: main.py
import time
from color_recognition import ColorRecognizer
from serial_protocol import SerialComm, CMD_SET_LED, CMD_PING, CMD_RESTART, RSP_ACK

# --- Konfiguráció ---
SERIAL_PORT = "/dev/ttyS0"
BAUDRATE = 576000

COLOR_CODES = {
    "Red": 1,
    "Yellow": 2,
    "Green": 3,
    "Blue": 4,
    "Purple": 5,
    "Unknown": 6, # Dedikált kód az ismeretlen színnek
}

def main():
    print("Program indítása...")
    # Objektumok inicializálása
    color_recognizer = ColorRecognizer()
    comm = SerialComm(port=SERIAL_PORT, baudrate=BAUDRATE)
    
    if not comm.is_opened():
        color_recognizer.stop()
        print("Kilépés a soros port hiba miatt.")
        return

    # Kapcsolat tesztelése egy PING paranccsal
    print("Kapcsolat tesztelése az ESP32-vel...")
    if comm.execute_command(CMD_PING):
        print("Kapcsolat sikeresen felépítve.")
    else:
        print("Hiba: Az ESP32 nem válaszol. Kérlek ellenőrizd a kapcsolatot és a tápellátást.")
        comm.close()
        color_recognizer.stop()
        return

    try:
        print("Fő ciklus elindítva. Leállítás: Ctrl+C")
        while True:
            # Szín felismerése
            color_name = color_recognizer.get_color(640, 360)
            color_code = COLOR_CODES.get(color_name, COLOR_CODES["Unknown"])
            
            # Parancs végrehajtása és válasz fogadása
            response = comm.execute_command(CMD_SET_LED, color_code, quiet=True)
            
            if response:
                # Egyszerűsített kiíratás, hogy ne árassza el a konzolt
                if response["cmd"] == RSP_ACK and response["param2"] == color_code:
                    # A kiíratást kikommentelheted, ha túl sok
                    print(f"Sikeresen beállítva: {color_name} ({color_code})")
                else:
                    print(f"Váratlan válasz kapva: {response}")
            else:
                # Ha nincs válasz, az komoly hiba, érdemes lehet megállni
                print("Hiba: Kommunikáció megszakadt az ESP32-vel.")
                break # Kilépés a ciklusból
            
            time.sleep(0.05) # Ciklusidő szabályozása

    except KeyboardInterrupt:
        print("\nProgram leállítása a felhasználó által...")
    finally:
        # Program lezárása, erőforrások felszabadítása
        if comm.is_opened():
            print("LED-ek lekapcsolása...")
            # A "0" paraméter jelzi a kikapcsolást az ESP32-n
            comm.execute_command(CMD_SET_LED, 0) 
            comm.close()
        color_recognizer.stop()
        print("Program befejezve.")

if __name__ == "__main__":
    main()
    