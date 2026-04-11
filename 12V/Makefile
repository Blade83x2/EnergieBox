#
# Makefile für 12V 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#

OBJ         = 12V.o mymcp23017.o iniparse.o mysql_wrapper.o

COMPILER    = gcc
CFLAGS      = -std=gnu99
LIBS        = -lwiringPi -lmysqlclient

12V: $(OBJ)
	$(COMPILER) -o $@ $(OBJ) $(LIBS) $(CFLAGS)
	chmod 550 $@

12V.o: 12V.c
	$(COMPILER) -c 12V.c $(CFLAGS)
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
	rm -f 12V $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
