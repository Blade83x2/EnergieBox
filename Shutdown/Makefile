mcp_shutdown:mcp_shutdown.o mymcp23017.o
	gcc -Wall -o mcp_shutdown mcp_shutdown.o mymcp23017.o -lwiringPi -std=gnu99
mcp_shutdown.o: mcp_shutdown.c mymcp23017.h
	gcc -Wall -c mcp_shutdown.c -lwiringPi -std=gnu99
mymcp23017.o: mymcp23017.c mymcp23017.h
	gcc -Wall -c mymcp23017.c -lwiringPi -std=gnu99
clean:
	rm mcp_shutdown.o mymcp23017.o mcp_shutdown
