# main.py

import time
from spi_protocol import SpiComm, CMD_PING, CMD_TEST, CMD_TEST2, RSP_ACK, RSP_PONG, RSP_TEST2_OK

def main():
    esp_comm = SpiComm(bus=0, device=0, speed_hz=100000)
    
    if not esp_comm.spi:
        print("SPI inicializ�l�si hiba, a program le�ll.")
        return

    try:
        response = esp_comm.execute_command(CMD_PING)
        if response and response['rsp_code'] == RSP_ACK:
            print("SIKER")
        time.sleep(0.5)

        response = esp_comm.execute_command(CMD_TEST)
        if response and response['rsp_code'] == RSP_PONG:
            print(">>> SIKER: A PONG v�lasz meg�rkezett a PING-re.")
        else:
            print(">>> HIBA: A PONG v�lasz nem �rkezett meg.")
        time.sleep(0.5)

        print("\n--- 3. Tranzakci�: CMD_TEST2 ---")
        response = esp_comm.execute_command(CMD_TEST2, p1=10, p2=20, p3=30)
        
        if response and response['rsp_code'] == RSP_PONG:
            print(">>> SIKER: A PONG v�lasz meg�rkezett a CMD_TEST-re.")
        else:
            print(">>> HIBA: A PONG v�lasz a CMD_TEST-re nem �rkezett meg.")
        time.sleep(0.5)
        
        response = esp_comm.execute_command(CMD_PING)
        if response and response['rsp_code'] == RSP_TEST2_OK:
            if response['param1'] == 10 and response['param2'] == 20 and response['param3'] == 30:
                print(">>> SIKER: A visszak�ld�tt param�terek is helyesek!")
            else:
                print(">>> HIBA: A visszakapott param�terek nem egyeznek!")
        else:
            print(">>> HIBA: Nem a v�rt v�lasz �rkezett a CMD_TEST2-re.")

    except KeyboardInterrupt:
        print("\nProgram megszak�tva.")
        
    finally:
        esp_comm.close()

if __name__ == "__main__":
    main()