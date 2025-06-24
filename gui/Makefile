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

	
#
# Makefile für gui 
# Vendor: Johannes Krämer
# Version: 1.2
# Date: 24.06.2025
#

CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`
TARGET = gui
OBJ = gui.o
INSTALL_DIR = /usr/local/bin
INSTALL_GROUP = energiebox

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
	chmod 550 $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	chmod 600 $@

#install: $(TARGET)
#	@echo "Installiere $(TARGET) nach $(INSTALL_DIR)..."
#	install -g $(INSTALL_GROUP) -m 660 $(TARGET) $(INSTALL_DIR)/$(TARGET)
#	@echo "Fertig: $(INSTALL_DIR)/$(TARGET)"

clean:
	rm -f $(OBJ) $(TARGET)
