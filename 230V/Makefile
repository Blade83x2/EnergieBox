#
# Makefile für 230V 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#
OBJ         = 230V.o mymcp23017.o iniparse.o mysql_wrapper.o
COMPILER    = gcc
CFLAGS      = -std=gnu99
LIBS        = -lwiringPi -lmysqlclient

230V: $(OBJ)
	$(COMPILER) -o $@ $(OBJ) $(LIBS) $(CFLAGS)
	chmod 550 $@

230V.o: 230V.c
	$(COMPILER) -c 230V.c $(CFLAGS)
	chmod 600 $@

mymcp23017.o: mymcp23017.c
	$(COMPILER) -c mymcp23017.c $(CFLAGS)
	chmod 600 $@

iniparse.o: iniparse.c
	$(COMPILER) -c iniparse.c $(CFLAGS)
	chmod 600 $@

mysql_wrapper.o: mysql_wrapper.c mysql_wrapper.h
	$(COMPILER) -c mysql_wrapper.c $(CFLAGS)
	chmod 600 $@

clean:
	rm -f 230V $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
