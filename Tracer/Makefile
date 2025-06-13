#
# Makefile für tracer
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# https://www.c-howto.de/tutorial/makefiles/
# https://www.c-howto.de/tutorial/makefiles/erweiterungen/
C_COMPILER   = gcc
CPP_COMPILER = g++
CFLAGS       = -std=gnu99 -lwiringPi
CPPFLAGS     = -std=c++17

C_OBJ   = mymcp23017.o iniparse.o
EPEVER_OBJ = epever.o PVStatus.o BatteryStatus.o StatusBlock.o

all: trace epever

trace: trace.o $(C_OBJ)
	$(CPP_COMPILER) -o $@ trace.o $(C_OBJ) $(CFLAGS)

epever: $(EPEVER_OBJ)
	$(CPP_COMPILER) -o $@ $(EPEVER_OBJ)

mymcp23017.o: mymcp23017.c
	$(C_COMPILER) -c $< $(CFLAGS)

iniparse.o: iniparse.c
	$(C_COMPILER) -c $< -std=gnu99

epever.o: epever.cpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)

PVStatus.o: PVStatus.cpp PVStatus.hpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)

BatteryStatus.o: BatteryStatus.cpp BatteryStatus.hpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)

StatusBlock.o: StatusBlock.cpp StatusBlock.hpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)

trace.o: trace.cpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)

clean:
	rm -f *.o trace epever
