# Makefile für BatteryController
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 24.06.2025
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatische Build-Info
# - Code-Formatierung via clang-format
# - Install und Uninstall

CXX            = g++
CC             = gcc
CXXFLAGS       = -Wall -std=c++17 -lmysqlclient -lwiringPi
CFLAGS         = -Wall -std=gnu99 -lwiringPi
TARGET         = trace
OBJ            = trace.o iniparse.o
INSTALL_DIR    = /usr/local/bin
INSTALL_GROUP  = energiebox

# Farben für Ausgaben
GREEN          = \033[1;32m
RED            = \033[1;31m
NC             = \033[0m

# Build-Info
BUILD_DATE     := $(shell date +%Y-%m-%d)
BUILD_VERSION  := 1.0
CXXFLAGS       += -DBUILD_DATE=\"$(BUILD_DATE)\" -DBUILD_VERSION=\"$(BUILD_VERSION)\"
CFLAGS         += -DBUILD_DATE=\"$(BUILD_DATE)\" -DBUILD_VERSION=\"$(BUILD_VERSION)\"

# Default-Ziel
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)
	chmod 550 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Binary $@ erstellt mit Rechten 550 und Gruppe $(INSTALL_GROUP).$(NC)"

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"

install: $(TARGET)
	@if getent group $(INSTALL_GROUP) > /dev/null; then \
		echo "$(GREEN)Gruppe '$(INSTALL_GROUP)' existiert.$(NC)"; \
	else \
		echo "$(RED)Gruppe '$(INSTALL_GROUP)' fehlt!$(NC)"; \
		echo "$(RED)Bitte anlegen mit: sudo groupadd $(INSTALL_GROUP)$(NC)"; \
		exit 1; \
	fi
	@echo "Installiere $@ nach $(INSTALL_DIR)..."
	install -g $(INSTALL_GROUP) -m 550 $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@echo "$(GREEN)Installiert als $(INSTALL_DIR)/$(TARGET).$(NC)"

uninstall:
	@if [ -f $(INSTALL_DIR)/$(TARGET) ]; then \
		rm -f $(INSTALL_DIR)/$(TARGET); \
		echo "$(GREEN)Erfolgreich entfernt.$(NC)"; \
	else \
		echo "$(RED)Keine Datei zum Entfernen gefunden.$(NC)"; \
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

.clang-format:
	@echo "BasedOnStyle: LLVM"                         > .clang-format
	@echo "IndentWidth: 4"                            >> .clang-format
	@echo "TabWidth: 4"                               >> .clang-format
	@echo "UseTab: Always"                            >> .clang-format
	@echo "BreakBeforeBraces: Attach"                >> .clang-format
	@echo "BinPackParameters: true"                  >> .clang-format
	@echo "BinPackArguments: true"                   >> .clang-format
	@echo "ColumnLimit: 100"                         >> .clang-format
	@echo "AllowShortFunctionsOnASingleLine: Inline" >> .clang-format
	@echo "SortIncludes: false"                      >> .clang-format
