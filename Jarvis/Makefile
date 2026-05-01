# Makefile für Jarvis Sprachassistent

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

SOURCES = jarvis.cpp RuleEngine.cpp SimpleYAMLParser.cpp
OBJECTS = $(SOURCES:.cpp=.o)

jarvis: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o jarvis $(OBJECTS)

clean:
	rm -f jarvis *.o *.wav result.txt

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)
