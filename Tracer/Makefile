#
# Makefile für trace
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatische Build-Info
# - automatisierte Code-Formatierung
# - Installations- und Deinstallationsziel
#

C_COMPILER   = gcc
CPP_COMPILER = g++
GROUP        = energiebox

CFLAGS       = -std=gnu99 -Wall -lwiringPi -lmysqlclient
CPPFLAGS     = -std=c++17 -Wall

# Build-Info
BUILD_DATE := $(shell date +%Y-%m-%d)
BUILD_VERSION := 1.0
CFLAGS   += -DBUILD_DATE=\"$(BUILD_DATE)\"
CFLAGS   += -DBUILD_VERSION=\"$(BUILD_VERSION)\"
CPPFLAGS += -DBUILD_DATE=\"$(BUILD_DATE)\"
CPPFLAGS += -DBUILD_VERSION=\"$(BUILD_VERSION)\"

# Objektdateien
C_OBJ       = mymcp23017.o iniparse.o
EPEVER_OBJ  = epever.o PVStatus.o BatteryStatus.o StatusBlock.o
TARGETS     = trace epever

PREFIX      = /usr/local
BINDIR      = $(PREFIX)/bin

all: $(TARGETS)

# Link-Regeln mit Rechte- und Gruppensetzung
trace: trace.o $(C_OBJ)
	$(CPP_COMPILER) -o $@ trace.o $(C_OBJ) $(CPPFLAGS) -lwiringPi -lmysqlclient
	chmod 550 $@
	chgrp $(GROUP) $@

epever: $(EPEVER_OBJ)
	$(CPP_COMPILER) -o $@ $(EPEVER_OBJ) $(CPPFLAGS)
	chmod 550 $@
	chgrp $(GROUP) $@

# Kompiliere C-Objekte
%.o: %.c
	$(C_COMPILER) -c $< $(CFLAGS)
	chmod 600 $@
	chgrp $(GROUP) $@

# Kompiliere C++-Objekte
%.o: %.cpp
	$(CPP_COMPILER) -c $< $(CPPFLAGS)
	chmod 600 $@
	chgrp $(GROUP) $@

# Installationsziel
install: all
	install -d $(BINDIR)
	install -m 550 -g $(GROUP) trace $(BINDIR)/
	install -m 550 -g $(GROUP) epever $(BINDIR)/

# Deinstallationsziel
uninstall:
	rm -f $(BINDIR)/trace $(BINDIR)/epever

# Clean
clean:
	rm -f *.o $(TARGETS) .clang-format

# Code-Formatierung
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)

.clang-format:
	@echo "# https://clang.llvm.org/docs/ClangFormatStyleOptions.html"   > .clang-format
	@echo "# Stil-Basis: LLVM Google Chromium Mozilla WebKit Microsoft GNU"   >> .clang-format
	@echo "BasedOnStyle: GNU"                           >> .clang-format
	@echo "IndentWidth: 4      # Einrückung mit 4 Leerzeichen" >> .clang-format
	@echo "TabWidth: 4         # Tabulatoren zählen als 4 Leerzeichen" >> .clang-format
	@echo "UseTab: Never       # Never|Always " >> .clang-format
	@echo "BreakBeforeBraces: Stroustrup   # Allman / Attach / Linux / Stroustrup / Mozilla " >> .clang-format
	@echo "ColumnLimit: 0    # 80, 100, 0 (kein Limit)	Maximale Zeilenlänge" >> .clang-format
	@echo "AllowShortIfStatementsOnASingleLine: true   # if/else immer mit Blockklammern umbrechen" >> .clang-format
	@echo "AllowShortLoopsOnASingleLine: true          # true | false Einzeilige for/while Schleifen" >> .clang-format
	@echo "AllowShortFunctionsOnASingleLine: Inline     # None / Inline / All  sehr kurze Funktionen dürfen einzeilig bleiben" >> .clang-format
	@echo "KeepEmptyLinesAtTheStartOfBlocks: false       # keine leeren Zeilen direkt nach {" >> .clang-format
	@echo "SpacesBeforeTrailingComments: 2               # 2 Anzahl Leerzeichen vor Endkommentaren //" >> .clang-format
	@echo "SpaceAfterCStyleCast: false                    # true | false Leerzeichen nach C-Cast: (int) x" >> .clang-format
	@echo "SpaceBeforeParens: Never          # Never | ControlStatement  Leerzeichen vor ( bei if, while, for" >> .clang-format
	@echo "SpacesInParentheses: false          # true/false	Leerzeichen innerhalb von ()" >> .clang-format
	@echo "BinPackArguments: true  # true/false	Argumente in Funktionsaufrufen auf eine Zeile packen" >> .clang-format
	@echo "BinPackParameters: true # true/false	Gleiches für Funktionsdefinitionen" >> .clang-format
	@echo "SortIncludes: false       # true/false	Automatisch #includes sortieren" >> .clang-format
