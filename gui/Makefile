#
# Makefile für gui 
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#

CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`
TARGET = gui
OBJ = gui.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)
