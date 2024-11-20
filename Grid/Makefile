mymcp23017:grid.o mymcp23017.o
	gcc -Wall -o grid grid.o mymcp23017.o -lwiringPi -std=gnu99
grid.o: grid.c mymcp23017.h
	gcc -Wall -c grid.c -lwiringPi -std=gnu99
clean:
	rm grid.o mymcp23017.o 12V
