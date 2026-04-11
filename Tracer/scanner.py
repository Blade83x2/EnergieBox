#!/usr/bin/env python3

from pymodbus.client import ModbusSerialClient
import time

PORT = "/dev/ttyACM0"
BAUD = 115200   # ggf. 2400 testen

SLAVES = range(1, 20)      # typische Inverter
REGISTERS = range(0, 200)  # Standard Bereich + erweitern

def connect():
    client = ModbusSerialClient(
        port=PORT,
        baudrate=BAUD,
        parity='N',
        stopbits=1,
        bytesize=8,
        timeout=1
    )
    return client


def scan():
    client = connect()

    if not client.connect():
        print("❌ Keine Verbindung zum Serial Port")
        return

    print("✔ Modbus Scanner gestartet\n")

    for slave in SLAVES:
        print(f"\n🔎 Test Slave ID {slave}")

        for reg in REGISTERS:
            try:
                result = client.read_holding_registers(
                    address=reg,
                    count=1,
                    slave=slave
                )

                if not result.isError():
                    value = result.registers[0]
                    print(f"✔ Slave {slave} | Reg {hex(reg)} = {value}")

            except Exception:
                pass

        time.sleep(0.2)

    client.close()
    print("\n✅ Scan abgeschlossen")


if __name__ == "__main__":
    scan()
