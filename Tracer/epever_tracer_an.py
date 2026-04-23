from pymodbus.client import ModbusSerialClient
import configparser
config = configparser.ConfigParser()
config.read('/Energiebox/Grid/config.ini')
epever_port = config['system']['epeverPort']
epever_baudrate = int(config['system']['epeverBaudrate'])
client = ModbusSerialClient(
    port=epever_port,
    baudrate=epever_baudrate,
    parity='N',
    stopbits=1,
    bytesize=8,
    timeout=1
)

def read_scaled(register, scale=1):
    result = client.read_input_registers(address=register, count=1, slave=1)
    if result.isError():
        return None
    return result.registers[0] / scale

if client.connect():
    print(f"PV Array: Maximale Eingangs Spannung in Volt = {read_scaled(0x3000, 100)}V")
    print(f"PV Array: Maximaler Eingangs Strom in Ampere = {read_scaled(0x3001, 100)}A")
    print(f"PV Array: Maximale Eingangs Leistung in Watt = {read_scaled(0x3002, 100)}W")
    print(f"PV Array: Aktuelle Spannung in Volt = {read_scaled(0x3100, 100)}V")
    print(f"PV Array: Aktueller Strom in Ampere = {read_scaled(0x3101, 100)}A")
    print(f"PV Array: Aktuelle Leistung in Watt = {read_scaled(0x3102, 100)}W")
    print(f"PV Array: Generierte Energie heute = {read_scaled(0x330C, 100)}kWh")
    print(f"Batterie: System Spannung in Volt = {read_scaled(0x3004, 100)}V")
    print(f"Batterie: Maximaler Ladestrom in Ampere = {read_scaled(0x3005, 100)}A")
    print(f"Batterie: Maximale Ladeleistung in Watt = {read_scaled(0x3006, 100)}W")
    print(f"Batterie: Aktuelle Spannung in Volt = {read_scaled(0x3104, 100)}V")
    print(f"Batterie: Derzeitiger Ladestrom in Ampere = {read_scaled(0x3105, 100)}A")
    print(f"Batterie: Derzeitige Ladeleistung in Watt = {read_scaled(0x3106, 100)}W")
    print(f"Batterie: Maximale Spannung heute in Volt = {read_scaled(0x3302, 100)}V")
    print(f"Batterie: Minimale Spannung heute in Volt = {read_scaled(0x3303, 100)}V")
    print(f"Batterie: Aufnahme in Ampere Stunden = {read_scaled(0x9001, 1)}Ah")
    print(f"Batterie: Ladezustand in Prozent = {read_scaled(0x311A, 1)}%")
    client.close()
else:
    print("Keine Verbindung")
