# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Zielprogramm
TARGET = epever

# Quell- und Headerdateien
SRC = epever.cpp PVStatus.cpp BatteryStatus.cpp StatusBlock.cpp
OBJ = $(SRC:.cpp=.o)

# Build-Regeln
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
