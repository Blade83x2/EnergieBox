#!/usr/bin/env python3

import serial
import time
from pymodbus.client import ModbusSerialClient

PORTS = ["/dev/ttyACM0", "/dev/ttyUSB0"]
BAUDRATES = [2400, 4800, 9600, 115200]


# ----------------------------
# Voltronic Test
# ----------------------------
def test_voltronic(port, baud):
    try:
        ser = serial.Serial(port, baud, timeout=2)
        time.sleep(1)

        ser.write(b"QPIGS\r")
        time.sleep(1)

        resp = ser.readline().decode(errors="ignore").strip()
        ser.close()

        if resp and "(" in resp:
            return True, resp

    except:
        pass

    return False, None


# ----------------------------
# Modbus Test
# ----------------------------
def test_modbus(port, baud):
    try:
        client = ModbusSerialClient(
            port=port,
            baudrate=baud,
            parity='N',
            stopbits=1,
            bytesize=8,
            timeout=1
        )

        if not client.connect():
            return False, None

        # Test common register
        result = client.read_holding_registers(address=0, count=1, slave=1)

        client.close()

        if not result.isError():
            return True, result.registers

    except:
        pass

    return False, None


# ----------------------------
# MAIN DETECTOR
# ----------------------------
def detect():
    print("\n🔍 START AUTO PROTOCOL DETECTION\n")

    for port in PORTS:
        for baud in BAUDRATES:

            print(f"Testing {port} @ {baud}")

            # --- 1. Voltronic ---
            ok, resp = test_voltronic(port, baud)
            if ok:
                print("\n✅ DETECTED: VOLTRONIC / AXPERT")
                print("PORT:", port)
                print("BAUD:", baud)
                print("SAMPLE:", resp)
                return

            # --- 2. Modbus ---
            ok, resp = test_modbus(port, baud)
            if ok:
                print("\n✅ DETECTED: MODBUS RTU (EPEVER-like)")
                print("PORT:", port)
                print("BAUD:", baud)
                print("SAMPLE REGISTER:", resp)
                return

    print("\n❌ NO PROTOCOL DETECTED")
    print("Possible issues:")
    print("- wrong cable")
    print("- inverter not active")
    print("- USB is charge-only")


if __name__ == "__main__":
    detect()

