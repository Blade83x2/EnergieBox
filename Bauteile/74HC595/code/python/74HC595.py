#!/usr/bin/env python

#================================================
#
#
#   Extend use of 8 LED with 74HC595.
#   Change the  WhichLeds and sleeptime value under
#   loop() function to change LED mode and speed.
#
#=================================================

import RPi.GPIO as GPIO
import time

SDI   = 23
RCLK  = 24
SRCLK = 5


LED0 = [0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80]    #original mode
BLINK = [0xff,0x00,0xff,0x00,0xff,0x00]         #blink
LED1 = [0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff]    #blink mode 1
LED2 = [0x01,0x05,0x15,0x55,0xb5,0xf5,0xfb,0xff]    #blink mode 2
LED3 = [0x02,0x03,0x0b,0x0f,0x2f,0x3f,0xbf,0xff]    #blink mode 3
#=================================================

def print_message():
    print ("========================================")
    print ("|           LEDs with 74HC595          |")
    print ("|                                      |")
    print ("|       Control LEDs with 74HC595      |")
    print ("|                                      |")
    print ("========================================\n")
    print 'Program is running...'
    print 'Please press Ctrl+C to end the program...'
    raw_input ("Press Enter to begin\n")

def setup():
    GPIO.setmode(GPIO.BCM)    # Number GPIOs by its BCM location
    GPIO.setup(SDI, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(RCLK, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(SRCLK, GPIO.OUT, initial=GPIO.LOW)

# Shift the data to 74HC595
def hc595_shift(dat):
    for bit in range(0, 8): 
        GPIO.output(SDI, 0x80 & (dat << bit))
        GPIO.output(SRCLK, GPIO.HIGH)
        time.sleep(0.001)
        GPIO.output(SRCLK, GPIO.LOW)
    GPIO.output(RCLK, GPIO.HIGH)
    time.sleep(0.001)
    GPIO.output(RCLK, GPIO.LOW)

def main():
    print_message()
    mode = LED0 # Change Mode, modes from LED0 to LED3
    sleeptime = 0.15        # Change speed, lower value, faster speed
    blink_sleeptime = 0.3
    leds = ['-', '-', '-', '-', '-', '-', '-', '-']
    while True:
        # Change LED status from mode
        print "    mode"
        for m in mode:
            hc595_shift(m)
            leds[mode.index(m)] = 1    # Show which led is on
            print leds
            time.sleep(sleeptime)
            leds[mode.index(m)] = '-'  # Show the led is off
        
        print "    blink"
        for m in BLINK:
            hc595_shift(m)
            if (m == 0x00):
                leds = ['-'] * 8
            elif (m == 0xff):
                leds = [1] * 8
            print leds
            time.sleep(blink_sleeptime)

        # Change LED status from mode reverse
        print "    reversed mode"
        for m in reversed(mode):
            hc595_shift(m)
            leds[mode.index(m)] = 1    # Show which led is on
            print leds
            time.sleep(sleeptime)
            leds[mode.index(m)] = '-'  # Show the led is off

        print "    blink"
        for m in BLINK:
            hc595_shift(m)
            if (m == 0x00):
                leds = ['-'] * 8
            elif (m == 0xff):
                leds = [1] * 8
            print leds
            time.sleep(blink_sleeptime)

def destroy():
    GPIO.cleanup()

if __name__ == '__main__':
    setup()
    try:
        main()
    except KeyboardInterrupt:
        destroy()
