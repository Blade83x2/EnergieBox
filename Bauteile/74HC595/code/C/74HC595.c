#include <wiringPi.h>
#include <stdio.h>

#define   SDI   4   //serial data input
#define   RCLK  5   //memory clock input(STCP)
#define   SRCLK 21   //shift register clock input(SHCP)

unsigned char LED[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


void pulse(int pin){
	digitalWrite(pin, 0);
	digitalWrite(pin, 1);
}

void SI(unsigned char byte){
	int i;

	for(i=0;i<8;i++){
		digitalWrite(SDI, ((byte & (0x80 >> i)) > 0));
		pulse(SRCLK);
	}
}

void init(void){
	pinMode(SDI, OUTPUT); //make P23 output
	pinMode(RCLK, OUTPUT); //make P24 output
	pinMode(SRCLK, OUTPUT); //make P5 output

	digitalWrite(SDI, 0);
	digitalWrite(RCLK, 0);
	digitalWrite(SRCLK, 0);
}

int main(void){
	int i;

	if(wiringPiSetup() == -1){ //when initialize wiring failed, print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}

	init();


	printf("\n");
	printf("\n");
	printf("========================================\n");
	printf("|           LEDs with 74HC595          |\n");
	printf("|       Control LEDs with 74HC595      |\n");
	printf("|                                      |\n");
	printf("========================================\n");
	printf("\n");
	printf("\n");

	while(1){
		for(i=0;i<8;i++){
			SI(LED[i]);
			pulse(RCLK);
			delay(150);
			//printf("i = %d\n",i);
		}
		delay(500);

		for(i=0;i<3;i++){
			SI(0xff);
			pulse(RCLK);
			delay(100);
			SI(0x00);
			pulse(RCLK);
			delay(100);
		}
		delay(500);
//		digitalWrite(RCLK,0);

		for(i=0;i<8;i++){
			SI(LED[8-i-1]);
			pulse(RCLK);
			delay(150);
		}
		delay(500);

	    for(i=0;i<3;i++){
			SI(0xff);
			pulse(RCLK);
			delay(100);
			SI(0x00);
			pulse(RCLK);
			delay(100);
		}
		delay(500);
	}

	return 0;
}

