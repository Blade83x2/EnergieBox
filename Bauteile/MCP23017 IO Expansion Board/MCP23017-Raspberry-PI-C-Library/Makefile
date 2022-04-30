mymcp23017:main.o mymcp23017.o
	gcc -Wall -o mcp23017 main.o mymcp23017.o -lwiringPi -std=gnu99
main.o: mpc_setup.c mymcp23017.h
	gcc -Wall -c mpc_setup.c -lwiringPi -std=gnu99
mymcp23017.o: mymcp23017.c mymcp23017.h
	gcc -Wall -c mymcp23017.c -lwiringPi -std=gnu99
clean:
	rm main.o mymcp23017.o mymcp23017 mcp23017
