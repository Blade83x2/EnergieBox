#
# Makefile für tracer
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/

OBJ 		= tracer.o mymcp23017.o iniparse.o
COMPILER 	= gcc

tracer:		$(OBJ)
			$(COMPILER) -o $@ $(OBJ)	-lwiringPi -std=gnu99

tracer.o: 		tracer.c
			$(COMPILER) -c tracer.c 				-lwiringPi -std=gnu99
	
mymcp23017.o: 	mymcp23017.c
			$(COMPILER) -c mymcp23017.c				-lwiringPi -std=gnu99
			
iniparse.o: 	iniparse.c
			$(COMPILER) -c iniparse.c				-std=gnu99
			
clean:
		rm -f $* $(OBJ)
