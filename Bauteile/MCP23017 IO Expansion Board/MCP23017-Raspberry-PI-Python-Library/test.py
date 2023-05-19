#! /usr/bin/python
import smbus
import time
import sys

b = smbus.SMBus(1)
address = 0x22 # I2C Adresse
PinDict= {"7": 0x80, "6":0x40, "5":0x20, "4":0x10,"3":0x08, "2":0x04, "1":0x02, "0":0x01, "all":0xff, "off":0x00}

def get(row):
    read = b.read_byte_data(address,row)
    return read

def set(row,data):
    b.write_byte_data(0x20,0x00,0x00) # Bank A Ausgang
    b.write_byte_data(0x20,0x01,0x00) # Bank B Ausgang
    write = b.write_byte_data(address,row,data)
    read = get(row)
    return read

print("Waehle Bank a oder b")

bank =raw_input("Auswahl: ")

if bank == "a":
    row = 0x14
elif bank == "b":
    row = 0x15

print ("Waehle Ausgang 0-7 \n")
print ("Tippe 'all' fuer alle Pins der gewahlten Bank \n")
print ("Tippe 'off' um alle Pins der gewaehlten Bank auszuschalten  \n")
var =raw_input("Auswahl: ")
data = PinDict[var]
data = int(data)

result = set(row,data)

print ("Bank",bank,"hat nun Wert",hex(result))

#####---------Test---------###########
#b.write_byte_data(0x20,0x00,0x01) # Bank A Eingang
#b.write_byte_data(0x20,0x01,0x01) # Bank B Eingang

#block = "GPA"
#var = get (0x12)

#if var == 0x00:
#    var = get (0x13)

#    block = "GPB"
#    if var == 0x00:
#    print "Kein Schalter aktiv"
#    sys.exit()

#print "Der Schalter ist aktiviert an", block, "Wert:",hex(var)
######--------Test Ende-----------#############! /usr/bin/python
