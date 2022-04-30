#include <wiringPi.h>
#include <stdio.h>

#define LedPin		4
#define ButtonPin 	1

int main(void){
	// When initialize wiring failed, print messageto screen
	if(wiringPiSetup() == -1){
		printf("setup wiringPi failed !");
		return 1; 
	}
	
	pinMode(LedPin, OUTPUT); 
	pinMode(ButtonPin, INPUT);
	// Pull up to 3.3V,make GPIO1 a stable level
	pullUpDnControl(ButtonPin, PUD_UP);

	digitalWrite(LedPin, HIGH);
	printf("LED off\n");

	while(1){
		// Indicate that button has pressed down
		if(digitalRead(ButtonPin) == 0){
			// Led on
			digitalWrite(LedPin, LOW);
			printf("LED on\n");
		}
		else{
			// Led off
			digitalWrite(LedPin, HIGH);
			printf("LED off\n");
		}
	}
	return 0;
}

