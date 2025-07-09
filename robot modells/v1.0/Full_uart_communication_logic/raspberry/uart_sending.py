import serial
import struct
import time

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
        packet.append(checksum & 0xFF)        # low byte
        packet.append((checksum >> 8) & 0xFF) # high byte
        return bytes(packet)

    @staticmethod
    def parse_packet(packet: bytes) -> dict:
        if len(packet) != 7 or packet[0] != 0xAA:
            raise ValueError("Invalid packet format")

        payload = packet[:5]
        received_checksum = packet[5] | (packet[6] << 8)
        calc_checksum = PacketUtils.fletcher16(payload)

        if received_checksum != calc_checksum:
            raise ValueError("Checksum mismatch")

        return {
            "cmd": packet[1],
            "param1": packet[2],
            "param2": packet[3],
            "param3": packet[4],
        }

class SerialComm:
    def __init__(self, port="/dev/tty0", baudrate=115200, timeout=1):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        print(f" Serial opened on {port} @ {baudrate} baud")

    def send_command(self, cmd: int, p1: int = 0, p2: int = 0, p3: int = 0):
        packet = PacketUtils.build_packet(cmd, p1, p2, p3)
        self.ser.write(packet)
        #print(f"Sent: {[hex(b) for b in packet]}")

    def receive_response(self) -> dict | None:
        if self.ser.in_waiting >= 7:
            data = self.ser.read(7)
            print(f"Received: {[hex(b) for b in data]}")
            try:
                return PacketUtils.parse_packet(data)
            except ValueError as e:
                print(e)
        else:
            print("No response received")
        return None

    def close(self):
        self.ser.close()
        print("Serial closed")