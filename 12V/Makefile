#
# Makefile für 12V 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#

OBJ         = 12V.o mymcp23017.o iniparse.o
COMPILER    = gcc
CFLAGS      = -std=gnu99

12V: $(OBJ)
	$(COMPILER) -o $@ $(OBJ) -lwiringPi $(CFLAGS)
	chmod 550 $@  # Setze Berechtigung für Binary

12V.o: 12V.c
	$(COMPILER) -c 12V.c -lwiringPi $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

mymcp23017.o: mymcp23017.c
	$(COMPILER) -c mymcp23017.c -lwiringPi $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

iniparse.o: iniparse.c
	$(COMPILER) -c iniparse.c $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

clean:
	rm -f 12V $(OBJ)
