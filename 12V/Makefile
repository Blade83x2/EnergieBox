mymcp23017:12V.o mymcp23017.o
	gcc -Wall -o 12V 12V.o mymcp23017.o -lwiringPi -std=gnu99
12V.o: 12V.c mymcp23017.h
	gcc -Wall -c 12V.c -lwiringPi -std=gnu99
clean:
	rm 12V.o mymcp23017.o 12V
