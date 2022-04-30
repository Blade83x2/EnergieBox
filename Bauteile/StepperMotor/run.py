#!/usr/bin/env python  

from time import sleep
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
STEP = 17
DIR = 27
EN = 23
GPIO.setup(STEP, GPIO.OUT) # Frequenz
GPIO.setup(DIR, GPIO.OUT) # Richtung
GPIO.setup(EN, GPIO.OUT) # An/Aus
steps = 9500
usDelay = 950
uS = 0.000001 # (950*0.000001=0,00095)
GPIO.output(EN, GPIO.LOW) # Treiber aktivieren
print("[press ctrl+c to end the script]")
try:
    while True:
        GPIO.output(DIR, GPIO.HIGH) # Richtung nach oben
        for i in range(steps):
            GPIO.output(STEP, GPIO.HIGH)
            sleep(uS * usDelay)
            GPIO.output(STEP, GPIO.LOW)
            sleep(uS * usDelay)
        sleep(2)
        GPIO.output(DIR, GPIO.LOW) # Richtung nach unten
        for i in range(steps):
            GPIO.output(STEP, GPIO.HIGH)
            sleep(uS * usDelay)
            GPIO.output(STEP, GPIO.LOW)
            sleep(uS * usDelay)
        sleep(2)
except KeyboardInterrupt:
    GPIO.output(EN, GPIO.HIGH) # Treiber deaktivieren
