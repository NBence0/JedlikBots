# main.py

import time
from spi_protocol import SpiComm, CMD_PING, CMD_TEST, CMD_TEST2, RSP_ACK, RSP_PONG, RSP_TEST2_OK

def main():
    """A f� alkalmaz�slogika."""
    esp_comm = SpiComm(bus=0, device=0, speed_hz=100000)
    
    # Ha az SPI portot nem siker�lt megnyitni, ne folytassuk
    if not esp_comm.spi:
        print("SPI inicializ�l�si hiba, a program le�ll.")
        return

    try:
        # Az ESP v�lasza mindig egy tranzakci�t k�sik.
        # A teszt logika ezt helyesen kezeli.

        print("\n--- 1. Tranzakci�: Szinkroniz�ci� ---")
        # Elk�ld�nk egy PING-et, �s megkapjuk az ESP indul�skor el�k�sz�tett ACK-j�t.
        response = esp_comm.execute_command(CMD_PING)
        if response and response['rsp_code'] == RSP_ACK:
            print(">>> SIKER: Az indul�si ACK v�lasz meg�rkezett.")
        else:
            print(">>> HIBA: Az indul�si ACK v�lasz nem �rkezett meg, vagy a CRC hib�s volt.")
        time.sleep(0.5)

        print("\n--- 2. Tranzakci�: CMD_TEST ---")
        # Elk�ldj�k a CMD_TEST-et, �s megkapjuk a v�laszt az el�z� PING-re (RSP_PONG).
        response = esp_comm.execute_command(CMD_TEST)
        if response and response['rsp_code'] == RSP_PONG:
            print(">>> SIKER: A PONG v�lasz meg�rkezett a PING-re.")
        else:
            print(">>> HIBA: A PONG v�lasz nem �rkezett meg.")
        time.sleep(0.5)

        print("\n--- 3. Tranzakci�: CMD_TEST2 ---")
        # Elk�ldj�k a CMD_TEST2-t, �s megkapjuk a v�laszt az el�z� CMD_TEST-re (szint�n RSP_PONG).
        response = esp_comm.execute_command(CMD_TEST2, p1=10, p2=20, p3=30)
        if response and response['rsp_code'] == RSP_PONG:
            print(">>> SIKER: A PONG v�lasz meg�rkezett a CMD_TEST-re.")
        else:
            print(">>> HIBA: A PONG v�lasz a CMD_TEST-re nem �rkezett meg.")
        time.sleep(0.5)
        
        print("\n--- 4. Tranzakci�: V�lasz lek�r�se ---")
        # Elk�ld�nk egy �jabb PING-et, �s most kapjuk meg a v�laszt a CMD_TEST2-re (RSP_TEST2_OK).
        response = esp_comm.execute_command(CMD_PING)
        if response and response['rsp_code'] == RSP_TEST2_OK:
            print(">>> SIKER: Meg�rkezett az RSP_TEST2_OK v�lasz.")
            if response['param1'] == 10 and response['param2'] == 20 and response['param3'] == 30:
                print(">>> SIKER: A visszak�ld�tt param�terek is helyesek!")
            else:
                print(">>> HIBA: A visszakapott param�terek nem egyeznek!")
        else:
            print(">>> HIBA: Nem a v�rt v�lasz �rkezett a CMD_TEST2-re.")

    except KeyboardInterrupt:
        print("\nProgram megszak�tva.")
    finally:
        # Biztos�tjuk, hogy a kapcsolat mindig lez�ruljon
        esp_comm.close()

if __name__ == "__main__":
    main()