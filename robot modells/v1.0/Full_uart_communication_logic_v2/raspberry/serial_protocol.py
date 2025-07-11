# File: serial_protocol.py
import serial
import time

# --- Protokoll Konstansok ---
# Ezeket a konstansokat az ESP32 kód is használja
# Parancsok (RPI -> ESP32)
CMD_PING = 0
CMD_RESTART = 1
CMD_SET_LED = 7

# Válaszok (ESP32 -> RPI)
RSP_ACK = 100       # Sikeres parancs végrehajtás
RSP_PONG = 101      # Válasz a PING-re
RSP_ERROR = 200     # Hiba történt (pl. rossz checksum)
RSP_UNKNOWN_CMD = 201 # Ismeretlen parancs

class PacketUtils:
    @staticmethod
    def fletcher16(data: bytes) -> int:
        sum1 = 0
        sum2 = 0
        for byte in data:
            sum1 = (sum1 + byte) % 255
            sum2 = (sum2 + sum1) % 255
        return (sum2 << 8) | sum1

    @staticmethod
    def build_packet(cmd: int, p1: int = 0, p2: int = 0, p3: int = 0) -> bytes:
        packet = bytearray([0xAA, cmd, p1, p2, p3])
        checksum = PacketUtils.fletcher16(packet)
        packet.append(checksum & 0xFF)
        packet.append((checksum >> 8) & 0xFF)
        return bytes(packet)

    @staticmethod
    def parse_packet(packet: bytes) -> dict | None:
        if len(packet) != 7 or packet[0] != 0xAA:
            print("Hiba: Érvénytelen csomag formátum.")
            return None

        payload = packet[:5]
        received_checksum = packet[5] | (packet[6] << 8)
        calc_checksum = PacketUtils.fletcher16(payload)

        if received_checksum != calc_checksum:
            print(f"Hiba: Ellenőrzőösszeg nem egyezik! Várt: {hex(calc_checksum)}, Kapott: {hex(received_checksum)}")
            return None

        return {
            "cmd": packet[1],
            "param1": packet[2],
            "param2": packet[3],
            "param3": packet[4],
        }

class SerialComm:
    def __init__(self, port="/dev/ttyS0", baudrate=576000, timeout=2.0):
        try:
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            print(f"Soros port megnyitva: {port} @ {baudrate} baud")
        except serial.SerialException as e:
            print(f"Nem sikerült megnyitni a soros portot: {e}")
            self.ser = None

    def execute_command(self, cmd: int, p1: int = 0, p2: int = 0, p3: int = 0, quiet=False) -> dict | None:
        """Elküld egy parancsot és megvárja a választ."""
        if not self.ser:
            return None
            
        self.ser.flushInput() # Tisztítjuk a bejövő puffert a biztonság kedvéért
        packet = PacketUtils.build_packet(cmd, p1, p2, p3)
        
        if not quiet:
            print(f"Küldés -> CMD: {cmd}, P1: {p1}, Csomag: {[hex(b) for b in packet]}")
        
        self.ser.write(packet)
        
        # Várunk a válaszra (7 bájt)
        response_bytes = self.ser.read(7)
        
        if len(response_bytes) < 7:
            if not quiet:
                print("Hiba: Időtúllépés, nem érkezett válasz az ESP32-től.")
            return None
        
        response_data = PacketUtils.parse_packet(response_bytes)
        
        if response_data and not quiet:
            print(f"Válasz <- CMD: {response_data['cmd']}, P1: {response_data['param1']}, Csomag: {[hex(b) for b in response_bytes]}")
        
        return response_data

    def is_opened(self) -> bool:
        return self.ser is not None and self.ser.is_open

    def close(self):
        if self.ser and self.ser.is_open:
            print("Soros port lezárása...")
            self.ser.close()