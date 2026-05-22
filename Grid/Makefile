#
# Makefile für grid
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 22.05.2026
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/
#

OBJ         = grid.o mymcp23017.o iniparse.o mysql_wrapper.o
COMPILER    = gcc

# Libraries
LIBS        = -lwiringPi -lmysqlclient

# Compiler Flags
CFLAGS      = -std=gnu99

grid: $(OBJ)
	$(COMPILER) -o $@ $(OBJ) $(LIBS) $(CFLAGS)
	chmod 550 $@

grid.o: grid.c
	$(COMPILER) -c grid.c $(LIBS) $(CFLAGS)
	chmod 600 $@

mymcp23017.o: mymcp23017.c
	$(COMPILER) -c mymcp23017.c $(LIBS) $(CFLAGS)
	chmod 600 $@

iniparse.o: iniparse.c
	$(COMPILER) -c iniparse.c $(CFLAGS)
	chmod 600 $@
	
mysql_wrapper.o: mysql_wrapper.c mysql_wrapper.h
	$(COMPILER) -c mysql_wrapper.c $(CFLAGS)
	chmod 600 $@

clean:
	rm -f grid $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
