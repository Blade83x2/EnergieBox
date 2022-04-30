#!/usr/bin/env python
import RPi.GPIO as GPIO
import time


pinA = 3
pinB = 5
pinC = 15
pinD = 8
pinE = 10
pinF = 11
pinG = 12
pinDP = 13

def init():
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(pinA, GPIO.OUT)
	GPIO.setup(pinB, GPIO.OUT)
	GPIO.setup(pinC, GPIO.OUT)
	GPIO.setup(pinD, GPIO.OUT)
	GPIO.setup(pinE, GPIO.OUT)
	GPIO.setup(pinF, GPIO.OUT)
	GPIO.setup(pinG, GPIO.OUT)
	GPIO.setup(pinDP, GPIO.OUT)
	print 'gpio init completed!'



def display_0():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.HIGH)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.LOW)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 0'

def display_1():
	GPIO.output(pinA, GPIO.LOW)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.LOW)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.LOW)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 1'
	
	
def display_2():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.LOW)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.HIGH)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 2'
	
def display_3():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 3'	
	
def display_4():
	GPIO.output(pinA, GPIO.LOW)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.LOW)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 4'	
	
def display_5():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.LOW)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 5'	
	
def display_6():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.LOW)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.HIGH)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 6'


def display_7():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.LOW)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.LOW)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 7'	
	
def display_8():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.HIGH)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 8'	
	
	
def display_9():
	GPIO.output(pinA, GPIO.HIGH)
	GPIO.output(pinB, GPIO.HIGH)
	GPIO.output(pinC, GPIO.HIGH)
	GPIO.output(pinD, GPIO.HIGH)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.HIGH)
	GPIO.output(pinG, GPIO.HIGH)
	GPIO.output(pinDP, GPIO.LOW)
	print 'display number 9'
	
def display_dp():
	GPIO.output(pinA, GPIO.LOW)
	GPIO.output(pinB, GPIO.LOW)
	GPIO.output(pinC, GPIO.LOW)
	GPIO.output(pinD, GPIO.LOW)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.LOW)
	GPIO.output(pinDP, GPIO.HIGH)
	print 'display dp'

def clear():
	GPIO.output(pinA, GPIO.LOW)
	GPIO.output(pinB, GPIO.LOW)
	GPIO.output(pinC, GPIO.LOW)
	GPIO.output(pinD, GPIO.LOW)
	GPIO.output(pinE, GPIO.LOW)
	GPIO.output(pinF, GPIO.LOW)
	GPIO.output(pinG, GPIO.LOW)
	GPIO.output(pinDP, GPIO.LOW)
	print 'clear'

def loop():
	while True:
		display_0()
		time.sleep(1)
		display_1()
		time.sleep(1)
		display_2()
		time.sleep(1)
		display_3()
		time.sleep(1)
		display_4()
		time.sleep(1)
		display_5()
		time.sleep(1)
		display_6()
		time.sleep(1)
		display_7()
		time.sleep(1)
		display_8()
		time.sleep(1)
		display_9()
		time.sleep(1)
		display_dp()
		time.sleep(1)
		clear()
		time.sleep(1)
	
if __name__ == '__main__':
	try:
		init()
		loop()
	except KeyboardInterrupt:
		GPIO.cleanup()
		print 'Key Board Interrupt!'
		
	
