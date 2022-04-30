#include <wiringPi.h>
#include <stdio.h>

#define BuzzerPin      0

int main(void)
{
	if(wiringPiSetup() == -1)
	{ 
		printf("setup wiringPi failed !");
		return -1; 
	}

	pinMode(BuzzerPin,  OUTPUT);
	while(1)
	{
			digitalWrite(BuzzerPin, HIGH);
			delay(500);
			digitalWrite(BuzzerPin, LOW);
			delay(500);	
	}
	digitalWrite(BuzzerPin, LOW);
	return 0;
}
