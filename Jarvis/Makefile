# Makefile für Jarvis Sprachassistent
# Projekt: Energiebox

CXX           = g++
CXXFLAGS      = -Wall -Wextra -std=c++17 -O2 -MMD -MP
TARGET        = jarvis
SOURCES       = jarvis.cpp RuleEngine.cpp SimpleYAMLParser.cpp
OBJ           = $(SOURCES:.cpp=.o)
DEPS          = $(OBJ:.o=.d)

INSTALL_DIR   = /usr/local/bin
INSTALL_GROUP = energiebox

GREEN         = \033[1;32m
RED           = \033[1;31m
NC            = \033[0m

# === Build-Info ===
BUILD_DATE    := $(shell date +%Y-%m-%d)
BUILD_VERSION := 1.0

CXXFLAGS     += -DBUILD_DATE=\"$(BUILD_DATE)\" -DBUILD_VERSION=\"$(BUILD_VERSION)\"
CXXFLAGS     += -D_GLIBCXX_USE_CXX11_ABI=1

# === Targets ===
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^
	chmod 550 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Target $@ erstellt mit Rechten 550 und Gruppe $(INSTALL_GROUP).$(NC)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	chmod 600 $@
	chgrp $(INSTALL_GROUP) $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600 und Gruppe $(INSTALL_GROUP).$(NC)"

# === Dependencies einbinden ===
-include $(DEPS)

# === Installation ===
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
	@echo "$(GREEN)Erfolgreich installiert als $(INSTALL_DIR)/$(TARGET).$(NC)"

uninstall:
	@echo "Entferne $(INSTALL_DIR)/$(TARGET)..."
	@if [ -f $(INSTALL_DIR)/$(TARGET) ]; then \
		rm -f $(INSTALL_DIR)/$(TARGET); \
		echo "$(GREEN)Erfolgreich entfernt.$(NC)"; \
	else \
		echo "$(RED)Datei $(INSTALL_DIR)/$(TARGET) existiert nicht.$(NC)"; \
	fi

# === Clean ===
clean:
	rm -f $(OBJ) $(DEPS) $(TARGET) *.wav result.txt
	@echo "$(GREEN)Build-Dateien entfernt.$(NC)"

# === Code-Formatierung ===
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \))

format: .clang-format
	@echo "→ Formatiere Quellcode mit clang-format..."
	$(FORMATTER) -i $(SRC)
