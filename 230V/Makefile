mymcp23017:230V.o mymcp23017.o
	gcc -Wall -o 230V 230V.o mymcp23017.o -lwiringPi -std=gnu99
230V.o: 230V.c mymcp23017.h
	gcc -Wall -c 230V.c -lwiringPi -std=gnu99
clean:
	rm 230V.o mymcp23017.o 230V
