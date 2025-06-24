

OBJ            = h2o.o mymcp23017.o iniparse.o
COMPILER       = gcc
TARGET         = h2o
INSTALL_DIR    = /usr/local/bin
INSTALL_GROUP  = energiebox

GREEN  	       = \033[1;32m
RED    	       = \033[1;31m
NC     	       = \033[0m




all: $(TARGET)

$(TARGET): $(OBJ)
	$(COMPILER) -o $@ $(OBJ) -lwiringPi -std=gnu99
	chmod 550 $@
	@echo "$(GREEN)Target $@ erstellt mit Rechten 550.$(NC)"

%.o: %.c
	$(COMPILER) -c $< -lwiringPi -std=gnu99
	chmod 600 $@
	@echo "$(GREEN)Object-File $@ erstellt mit Rechten 600.$(NC)"

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
	@echo "$(GREEN)Build-Dateien gelöscht.$(NC)"
