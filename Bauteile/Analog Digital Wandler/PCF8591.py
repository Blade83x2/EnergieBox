#!/usr/bin/python3
#Mit diesem Programm wird ein über den I2C Bus angeschlossener Expander ausgelesen und angesteuert
#Zusammenspiel mit Node Red
#Es handelt sich um den Portexpander 8591 mit 4 AI und 1 AO

import smbus, time
import sys

Arg = sys.argv #Liste erstellen mit allen Systemargumenten
Arg.pop(0) #Namen der Datei rauslöschen
print(Arg) #Erhaltene Systemargumente ausgeben

bus = smbus.SMBus(1)
Portexpander = 0x48 #Adresse des Portexpanders

def get(register):
    read = bus.read_byte_data(Portexpander, 0x40+register)
    time.sleep(0.01)
    read = bus.read_byte_data(Portexpander, 0x40+register)
    return read

def set(register, daten):
    write = bus.write_byte_data(Portexpander, 0x40+register, daten) #0x40 steht dabei als Grundvoraussetzung, dass AnalogOut aktiviert ist.
    return

Wiedergabe = False #Hilfsvariable
set(3, int(Arg[0])) #Analogen Ausgang setzen mit Argument 0 aus Liste

print("Ain0:",get(0))
print("Ain1:",get(1))
print("Ain2:",get(2))
print("Ain3:",get(3))
print(Arg[1]) #Argumente 1 aus Liste ausgeben
#while True:
   

#AIn0 = LDR
#AIn1 = Unbelegt
#AIn2 = Temperaturfühler
#AIn3 = Potentiometer
#AOut = LED
