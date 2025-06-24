#
# Makefile für 230V 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#

OBJ         = 230V.o mymcp23017.o iniparse.o
COMPILER    = gcc
CFLAGS      = -std=gnu99

230V: $(OBJ)
	$(COMPILER) -o $@ $(OBJ) -lwiringPi $(CFLAGS)
	chmod 550 $@  # Setze Berechtigung für Binary

230V.o: 230V.c
	$(COMPILER) -c 230V.c -lwiringPi $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

mymcp23017.o: mymcp23017.c
	$(COMPILER) -c mymcp23017.c -lwiringPi $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

iniparse.o: iniparse.c
	$(COMPILER) -c iniparse.c $(CFLAGS)
	chmod 600 $@  # Setze Berechtigung für Objektdatei

clean:
	rm -f 230V $(OBJ)


# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
