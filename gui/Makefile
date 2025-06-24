#
# Makefile für gui 
# Vendor: Johannes Krämer
# Version: 1.1
# Date: 24.06.2025
#

CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`
TARGET = gui
OBJ = gui.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
	chmod 550 $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	chmod 600 $@

clean:
	rm -f $(OBJ) $(TARGET)
