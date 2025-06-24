#
# Makefile für mcp_startup
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatische Build-Info
# - automatisierte Code-Formatierung
#

# ==== Compiler-Konfiguration ====
COMPILER    = gcc
CFLAGS      = -std=gnu99 -Wall
LIBS        = -lwiringPi
GROUP       = energiebox

# ==== Build-Info ====
# in der main() printf("Programmversion: %s\n", BUILD_VERSION);
# in der main() printf("Kompiliert am:    %s\n", BUILD_DATE);
BUILD_DATE := $(shell date +%Y-%m-%d)
BUILD_VERSION := 1.0
CFLAGS     += -DBUILD_DATE=\"$(BUILD_DATE)\"
CFLAGS     += -DBUILD_VERSION=\"$(BUILD_VERSION)\"

# ==== Quell- und Ziel-Dateien ====
OBJ         = mcp_startup.o mymcp23017.o iniparse.o
TARGET      = mcp_startup

# ==== Standardziel ====
all: $(TARGET)

# ==== Build-Regeln ====
$(TARGET): $(OBJ)
	$(COMPILER) -o $@ $(OBJ) $(LIBS) $(CFLAGS)
	chmod 550 $@
	chgrp $(GROUP) $@

%.o: %.c
	$(COMPILER) -c $< $(CFLAGS) $(LIBS)
	chmod 600 $@
	chgrp $(GROUP) $@

# ==== Clean ====
clean:
	rm -f $(TARGET) $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
