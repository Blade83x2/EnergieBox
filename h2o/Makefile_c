#
# Makefile für h2o 
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/

OBJ 		= h2o.o mymcp23017.o iniparse.o
COMPILER 	= gcc

h2o:		$(OBJ)
			$(COMPILER) -o $@ $(OBJ)	-lwiringPi -std=gnu99

h2o.o: 		h2o.c
			$(COMPILER) -c h2o.c 					-lwiringPi -std=gnu99
	
mymcp23017.o: 	mymcp23017.c
			$(COMPILER) -c mymcp23017.c				-lwiringPi -std=gnu99
			
iniparse.o: 	iniparse.c
			$(COMPILER) -c iniparse.c				-std=gnu99
			
clean:
		rm -f $* $(OBJ)
