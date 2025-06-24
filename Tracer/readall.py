#!/usr/bin/env python3
import time
from pymodbus.client import ModbusSerialClient as ModbusClient
from pymodbus.mei_message import *
from pyepsolartracer.registers import registers,coils
import serial.rs485
import logging
logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.ERROR)
# HIER sowie in Client.py PORT ANPASSEN, JENACHDEM WAS ls -la /dev/tty* ANZEIGT nach dem Einstecken des USB Kabels
client = ModbusClient(method='rtu', port='/dev/ttyACM0', baudrate=115200, stopbits = 1, bytesize = 8, timeout=1)
client.connect()
try:
    client.socket.rs485_mode = serial.rs485.RS485Settings()
except:
    pass

request = ReadDeviceInformationRequest(unit=1)
response = client.execute(request)
unit = 1
chunks = []
last = 0x3000 - 1
chunk = []
for reg in registers:
    if reg.address != last + 1:
        chunks.append(chunk)
        chunk = []
    chunk.append(reg)
    last = reg.address
for chunk in chunks:
    reg = chunk[0]
    count = len(chunk)
    if reg.is_input_register():
        rr = client.read_input_registers(address=reg.address, count=count, slave = unit)
    else:
        rr = client.read_holding_registers(address=reg.address, count=count, slave = unit)
for reg in registers:
    if reg.is_input_register():
        rr = client.read_input_registers(address=reg.address, count=reg.size, slave = unit)
    else:
        rr = client.read_holding_registers(address=reg.address, count=reg.size, slave = unit)
    value = reg.decode(rr)
    print (value)
for reg in coils:
    if reg.is_coil():
        rr = client.read_coils(address = reg.address, count = reg.size, slave = unit)
    elif reg.is_discrete_input():
        rr = client.read_discrete_inputs(address = reg.address, count = reg.size, slave = unit)
    value = reg.decode(rr)
    print ( value)
