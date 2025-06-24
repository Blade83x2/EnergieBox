#
# Makefile für mcp_shutdown
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/

OBJ 		= mcp_shutdown.o mymcp23017.o iniparse.o
COMPILER 	= gcc

mcp_shutdown:		$(OBJ)
			$(COMPILER) -o $@ $(OBJ)	-lwiringPi -std=gnu99

mcp_shutdown.o: 		mcp_shutdown.c
			$(COMPILER) -c mcp_shutdown.c 						-lwiringPi -std=gnu99
	
mymcp23017.o: 	mymcp23017.c
			$(COMPILER) -c mymcp23017.c						-lwiringPi -std=gnu99
			
iniparse.o: 	iniparse.c
			$(COMPILER) -c iniparse.c						-std=gnu99
			
clean:
		rm -f $* $(OBJ)
