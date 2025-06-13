#
# Makefile für tracer
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/

CC      = gcc
CFLAGS  = -std=gnu99
LDFLAGS = -lwiringPi

# Dateien und Objekte für tracer
TRACER_SRC   = tracer.c mymcp23017.c iniparse.c
TRACER_OBJ   = $(TRACER_SRC:.c=.o)

# Datei für batterie
BATTERIE_SRC = batterie.c
BATTERIE_OBJ = $(BATTERIE_SRC:.c=.o)

.PHONY: all clean

all: tracer batterie

# Linken der Binaries
tracer: $(TRACER_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

batterie: $(BATTERIE_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Kompilieren der .o Dateien (Pattern-Regel)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f tracer batterie $(TRACER_OBJ) $(BATTERIE_OBJ)
