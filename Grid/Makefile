#
# Makefile für grid 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/

OBJ 		= grid.o mymcp23017.o iniparse.o
COMPILER 	= gcc

grid:		$(OBJ)
			$(COMPILER) -o $@ $(OBJ) -lwiringPi -std=gnu99
			chmod 550 $@

grid.o: 		grid.c
			$(COMPILER) -c grid.c -lwiringPi -std=gnu99
			chmod 600 $@

mymcp23017.o: 	mymcp23017.c
			$(COMPILER) -c mymcp23017.c -lwiringPi -std=gnu99
			chmod 600 $@

iniparse.o: 	iniparse.c
			$(COMPILER) -c iniparse.c -std=gnu99
			chmod 600 $@

clean:
		rm -f grid $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
