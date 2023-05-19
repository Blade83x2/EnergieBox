#include <wiringPi.h>
#include <stdio.h>

//Set pin control all digital IO
int pinA = 8;
int pinB = 9;
int pinC = 3;
int pinD = 15;
int pinE = 16;
int pinF = 0;
int pinG = 1;
int pinDP = 2;


void digital_0(void) //Display number 0
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, HIGH);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, LOW);
	digitalWrite(pinDP, LOW);
}
void digital_1(void) //Display number 1
{
	digitalWrite(pinA, LOW);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, LOW);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, LOW);
	digitalWrite(pinDP, LOW);
}
void digital_2(void) //Display number 2
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, LOW);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, HIGH);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_3(void) //Display number3
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_4(void) //Display number 4
{
	digitalWrite(pinA, LOW);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, LOW);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_5(void) //Display number 5
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, LOW);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_6(void) //Display number 6
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, LOW);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, HIGH);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_7(void) //Display number7
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, LOW);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, LOW);
	digitalWrite(pinDP, LOW);
}
void digital_8(void) //Display number 8
{
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, HIGH);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinDP, LOW);
}
void digital_9(void) //Display number 9
{
	digitalWrite(pinA,HIGH);
	digitalWrite(pinB,HIGH);
	digitalWrite(pinC,HIGH);
	digitalWrite(pinD,HIGH);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF,HIGH);
	digitalWrite(pinG,HIGH);
	digitalWrite(pinDP,LOW);
}

void digital_dp(void) //Display dp
{
	digitalWrite(pinA, LOW);
	digitalWrite(pinB, LOW);
	digitalWrite(pinC, LOW);
	digitalWrite(pinD, LOW);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, LOW);
	digitalWrite(pinDP, HIGH);
}

void clear(void) //clear
{
	digitalWrite(pinA, LOW);
	digitalWrite(pinB, LOW);
	digitalWrite(pinC, LOW);
	digitalWrite(pinD, LOW);
	digitalWrite(pinE, LOW);
	digitalWrite(pinF, LOW);
	digitalWrite(pinG, LOW);
	digitalWrite(pinDP, LOW);
}
void init()
{
	pinMode(pinA, OUTPUT);
	pinMode(pinB, OUTPUT);
	pinMode(pinC, OUTPUT);
	pinMode(pinD, OUTPUT);
	pinMode(pinE, OUTPUT);
	pinMode(pinF, OUTPUT);
	pinMode(pinG, OUTPUT);
	pinMode(pinDP, OUTPUT);
}

void main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed!\n");
		return -1;
	}
	
	init();
	while(1)
	{
		digital_0();
		delay(1000);
		digital_1();
		delay(1000);
		digital_2();
		delay(1000);
		digital_3();
		delay(1000);
		digital_4();
		delay(1000);
		digital_5();
		delay(1000);
		digital_6();
		delay(1000);
		digital_7();
		delay(1000);
		digital_8();
		delay(1000);
		digital_9();
		delay(1000);
		digital_dp();
		delay(1000);
		clear();
		delay(1000);
	}
	
}
