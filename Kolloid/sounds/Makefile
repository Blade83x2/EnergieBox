CC = gcc
CFLAGS = -Wall -O2
LIBS = -lwiringPi

# Targets
TARGETS = disableTrafo enableTrafo finishedTrafo

# Quell-Dateien
SRC_disableTrafo = disableTrafo.c
SRC_enableTrafo  = enableTrafo.c
SRC_finishedTrafo = finishedTrafo.c

# Objekt-Dateien
OBJ_disableTrafo = disableTrafo.o
OBJ_enableTrafo  = enableTrafo.o
OBJ_finishedTrafo = finishedTrafo.o

all: $(TARGETS)

disableTrafo: $(OBJ_disableTrafo)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod 550 $@

enableTrafo: $(OBJ_enableTrafo)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod 550 $@

finishedTrafo: $(OBJ_finishedTrafo)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod 550 $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	chmod 600 $@

clean:
	rm -f $(TARGETS) *.o
