#
# Makefile für gui
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 24.06.2025
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatische Build-Info (BUILD_DATE, BUILD_VERSION)
# - automatisierte Code-Formatierung
# - Installations- und Deinstallationsziel
#

CXX           = g++
CXXFLAGS      = -Wall -std=c++17 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS       = `pkg-config gtkmm-3.0 --libs` -lmysqlclient
TARGET        = gui
OBJ           = gui.o MySQLiWrapper.o
INSTALL_DIR   = /usr/local/bin
INSTALL_GROUP = energiebox

GREEN         = \033[1;32m
RED           = \033[1;31m
NC            = \033[0m

# Build-Info
BUILD_DATE    := $(shell date +%Y-%m-%d)
BUILD_VERSION := 1.0

CXXFLAGS     += -DBUILD_DATE=\"$(BUILD_DATE)\" -DBUILD_VERSION=\"$(BUILD_VERSION)\"

CXXFLAGS     += -D_GLIBCXX_USE_CXX11_ABI=1

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
	chmod 550 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Target $@ erstellt mit Rechten 550 und Gruppe $(INSTALL_GROUP).$(NC)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"
	
MySQLiWrapper.o: MySQLiWrapper.cpp MySQLiWrapper.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"


install: $(TARGET)
	@if getent group $(INSTALL_GROUP) > /dev/null; then \
		echo "$(GREEN)Gruppe '$(INSTALL_GROUP)' existiert.$(NC)"; \
	else \
		echo "$(RED)WARNUNG: Gruppe '$(INSTALL_GROUP)' existiert nicht!$(NC)"; \
		echo "$(RED)Bitte erstellen mit: sudo groupadd $(INSTALL_GROUP)$(NC)"; \
		exit 1; \
	fi
	@echo "Installiere $(TARGET) nach $(INSTALL_DIR)..."
	install -g $(INSTALL_GROUP) -m 550 $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@echo "$(GREEN)Erfolgreich installiert als $(INSTALL_DIR)/$(TARGET) mit Gruppe $(INSTALL_GROUP) und Rechten 550.$(NC)"

uninstall:
	@echo "Entferne $(INSTALL_DIR)/$(TARGET)..."
	@if [ -f $(INSTALL_DIR)/$(TARGET) ]; then \
		rm -f $(INSTALL_DIR)/$(TARGET); \
		echo "$(GREEN)Erfolgreich entfernt.$(NC)"; \
	else \
		echo "$(RED)Datei $(INSTALL_DIR)/$(TARGET) existiert nicht.$(NC)"; \
	fi

clean:
	rm -f $(OBJ) $(TARGET)
	@echo "$(GREEN)Build-Dateien entfernt.$(NC)"

# === Code-Formatierung ===
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \))

format: .clang-format
	@echo "→ Formatiere Quellcode mit clang-format..."
	$(FORMATTER) -i $(SRC)

