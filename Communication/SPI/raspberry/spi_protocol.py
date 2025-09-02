import spidev
from typing import Optional

# Parancsok (Master -> ESP32)
CMD_PING = 0
CMD_RESTART = 1
CMD_READ_GYRO = 2
CMD_CALIBRATE_GYRO = 3
CMD_READ_COLOR_SENSOR = 4
CMD_SET_LED = 7
CMD_TEST = 100
CMD_TEST2 = 101

# V�laszok (ESP32 -> Master)
RSP_ACK = 100
RSP_PONG = 101
RSP_GYRO_DATA = 102
RSP_COLOR_DATA = 103
RSP_TEST2_OK = 111
RSP_ERROR_CRC = 200
RSP_UNKNOWN_CMD = 201

PACKET_SIZE = 6
PAYLOAD_SIZE = 4


def _calculate_crc16_modbus(data: bytes) -> int:
    """
    Szabv�nyos CRC-16/MODBUS sz�m�t�s.
    Ez egy priv�t f�ggv�ny, a PacketUtilsCRC oszt�ly haszn�lja.
    """
    crc = 0xFFFF
    poly = 0xA001  # Ez a 0x8005 polinom reflekt�lt form�ja
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ poly
            else:
                crc >>= 1
    return crc

class PacketUtilsCRC:
    @staticmethod
    def build_packet(cmd: int, p1: int = 0, p2: int = 0, p3: int = 0) -> bytes:
        payload = bytearray([cmd & 0xFF, p1 & 0xFF, p2 & 0xFF, p3 & 0xFF])
        checksum = _calculate_crc16_modbus(bytes(payload))
        payload.append(checksum & 0xFF)
        payload.append((checksum >> 8) & 0xFF)
        return bytes(payload)

    @staticmethod
    def parse_packet(packet: bytes) -> Optional[dict]:
        if len(packet) != PACKET_SIZE:
            print(f"Hiba: �rv�nytelen csomaghossz: {len(packet)} b�jt.")
            return None
            
        payload = packet[:PAYLOAD_SIZE]
        received_checksum = packet[4] | (packet[5] << 8)
        calculated_checksum = _calculate_crc16_modbus(payload)

        if received_checksum != calculated_checksum:
            print(f"!!! CRC HIBA !!! V�rt/Sz�molt: {hex(calculated_checksum)}, Kapott: {hex(received_checksum)}")
            return None
            
        return {"rsp_code": payload[0], "param1": payload[1], "param2": payload[2], "param3": payload[3]}

class SpiComm:
    def __init__(self, bus: int = 0, device: int = 0, speed_hz: int = 100000):
        self.spi = None
        try:
            self.spi = spidev.SpiDev()
            self.spi.open(bus, device)
            self.spi.max_speed_hz = speed_hz
            self.spi.mode = 0  # CPOL=0, CPHA=0
            print(f"SPI port megnyitva: /dev/spidev{bus}.{device} @ {speed_hz} Hz")
        except FileNotFoundError:
            print("KRITIKUS HIBA: SPI eszk�z nem tal�lhat�. Enged�lyezve van az interf�sz?")
            self.spi = None

    def execute_command(self, cmd: int, p1: int = 0, p2: int = 0, p3: int = 0) -> Optional[dict]:
        if not self.spi:
            return None
            
        request_packet = PacketUtilsCRC.build_packet(cmd, p1, p2, p3)
        print(f"K�ld�s ->       Csomag: {[hex(b) for b in request_packet]}")
        
        response_bytes = bytes(self.spi.xfer2(list(request_packet)))
        print(f"Nyers v�lasz <- Csomag: {[hex(b) for b in response_bytes]}")
        
        response_data = PacketUtilsCRC.parse_packet(response_bytes)
        if response_data:
            print(f"�rtelmezett v�lasz: {response_data}")
            
        return response_data

    def close(self):
        if self.spi:
            self.spi.close()
            print("SPI port lez�rva.")