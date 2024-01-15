mcp_startup:mcp_startup.o mymcp23017.o
	gcc -Wall -o mcp_startup mcp_startup.o mymcp23017.o -lwiringPi -std=gnu99
mcp_startup.o: mcp_startup.c mymcp23017.h
	gcc -Wall -c mcp_startup.c -lwiringPi -std=gnu99
mymcp23017.o: mymcp23017.c mymcp23017.h
	gcc -Wall -c mymcp23017.c -lwiringPi -std=gnu99
clean:
	rm mcp_startup.o mymcp23017.o mcp_startup
