#
# Makefile für gui 
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#

PROGRAMMNAME = gui
SRC = $(PROGRAMMNAME).cpp
OBJ = $(PROGRAMMNAME).o
CC = gcc

CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0)


all: $(PROGRAMMNAME)

$(PROGRAMMNAME): $(OBJ)
	$(CC) $(OBJ) -o $(PROGRAMMNAME) $(LIBS)

$(OBJ): $(SRC)
	$(CC) -c $(SRC) -o $(OBJ) $(CFLAGS)

clean:
	rm -f $(PROGRAMMNAME) $(OBJ)

.PHONY: all clean
