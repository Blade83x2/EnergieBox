#
# Makefile für gui
# Vendor: Johannes Krämer
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatisierte Code-Formatierung
# - Installations- und Deinstallationsziel
# - Autoincrement für Version
#

CXX           = g++
TARGET        = gui
INSTALL_DIR   = /usr/local/bin
INSTALL_GROUP = energiebox

GREEN         = \033[1;32m
RED           = \033[1;31m
NC            = \033[0m

# Build-Info
VERSION_FILE  = .build_version
BUILD_DATE    := $(shell date +%Y-%m-%d)

# Falls Datei nicht existiert -> initialisieren
$(shell [ -f $(VERSION_FILE) ] || echo "1.000" > $(VERSION_FILE))

# Versionsnummer erhöhen
OLD_VERSION := $(shell cat $(VERSION_FILE))
NEW_VERSION := $(shell awk "BEGIN { printf \"%.3f\", $(OLD_VERSION) + 0.001 }")
$(shell echo "$(NEW_VERSION)" > $(VERSION_FILE))
BUILD_VERSION := $(shell cat $(VERSION_FILE))

# pkg-config Pakete
PKG_CONFIG = gtkmm-3.0 vte-2.91

# Compiler Flags
CXXFLAGS  = -Wall -std=c++17
CXXFLAGS += -I../inc
CXXFLAGS += `pkg-config --cflags $(PKG_CONFIG)`
CXXFLAGS += -DBUILD_DATE=\"$(BUILD_DATE)\"
CXXFLAGS += -DBUILD_VERSION=\"$(BUILD_VERSION)\"
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1

# Linker Flags
LDFLAGS  = `pkg-config --libs $(PKG_CONFIG)`
LDFLAGS += -lmysqlclient -lX11 -lXext -lXxf86vm

OBJ = gui.o ../inc/MySQLiWrapper.o ../inc/IniReader.o

.PHONY: all clean install uninstall format deps

all: version-info $(TARGET)

version-info:
	@echo "$(GREEN)Build-Version erhöht: $(OLD_VERSION) -> $(BUILD_VERSION)$(NC)"

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
	chmod 550 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Target $@ erstellt mit Rechten 550 und Gruppe $(INSTALL_GROUP).$(NC)"

gui.o: gui.cpp ../inc/MySQLiWrapper.h ../inc/IniReader.h
	$(CXX) $(CXXFLAGS) -c -o $@ gui.cpp
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"

../inc/MySQLiWrapper.o: ../inc/MySQLiWrapper.cpp ../inc/MySQLiWrapper.h
	$(CXX) $(CXXFLAGS) -c -o $@ ../inc/MySQLiWrapper.cpp
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"

../inc/IniReader.o: ../inc/IniReader.cpp ../inc/IniReader.h
	$(CXX) $(CXXFLAGS) -c -o $@ ../inc/IniReader.cpp
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

# === Abhängigkeiten installieren ===

deps:
	sudo apt update
	sudo apt install -y \
		build-essential libxext-dev libx11-dev \
		libgtkmm-3.0-dev \
		libvte-2.91-dev \
		libmysqlclient-dev \
		clang-format \
		pkg-config

# === Code-Formatierung ===

FORMATTER = clang-format

SRC := $(shell find . ../inc -type f \( \
	-name "*.cpp" -o \
	-name "*.hpp" -o \
	-name "*.h" -o \
	-name "*.c" \
\))

format: .clang-format
	@echo "→ Formatiere Quellcode mit clang-format..."
	$(FORMATTER) -i $(SRC)
