#
# Makefile für mcp_startup
# Vendor: Johannes Krämer
# Version: 1.0
# Date: 22.11.2024
#
# Funktionen:
# - Build mit Rechte- und Gruppensetzung
# - automatische Build-Info
# - automatisierte Code-Formatierung
#

# ==== Compiler-Konfiguration ====
COMPILER    = gcc
CFLAGS      = -std=gnu99 -Wall
LIBS        = -lwiringPi
GROUP       = energiebox

# ==== Build-Info ====
# in der main() printf("Programmversion: %s\n", BUILD_VERSION);
# in der main() printf("Kompiliert am:    %s\n", BUILD_DATE);
BUILD_DATE := $(shell date +%Y-%m-%d)
BUILD_VERSION := 1.0
CFLAGS     += -DBUILD_DATE=\"$(BUILD_DATE)\"
CFLAGS     += -DBUILD_VERSION=\"$(BUILD_VERSION)\"

# ==== Quell- und Ziel-Dateien ====
OBJ         = mcp_startup.o mymcp23017.o iniparse.o
TARGET      = mcp_startup

# ==== Standardziel ====
all: $(TARGET)

# ==== Build-Regeln ====
$(TARGET): $(OBJ)
	$(COMPILER) -o $@ $(OBJ) $(LIBS) $(CFLAGS)
	chmod 550 $@
	chgrp $(GROUP) $@

%.o: %.c
	$(COMPILER) -c $< $(CFLAGS) $(LIBS)
	chmod 600 $@
	chgrp $(GROUP) $@

# ==== Clean ====
clean:
	rm -f $(TARGET) $(OBJ)

# ==== Code-Formatierung ====
FORMATTER = clang-format
SRC := $(shell find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

format: .clang-format
	@echo "→ Formatierung mit clang-format..."
	$(FORMATTER) -i $(SRC)

.clang-format:
	@echo "# https://clang.llvm.org/docs/ClangFormatStyleOptions.html"   > .clang-format
	@echo "# Stil-Basis: LLVM Google Chromium Mozilla WebKit Microsoft GNU"   >> .clang-format
	@echo "BasedOnStyle: LLVM"                           >> .clang-format
	@echo "IndentWidth: 4      # Einrückung mit 4 Leerzeichen" >> .clang-format
	@echo "TabWidth: 4         # Tabulatoren zählen als 4 Leerzeichen" >> .clang-format
	@echo "UseTab: Always       # Never|Always " >> .clang-format
	@echo "BreakBeforeBraces: Allman   # Allman / Attach / Linux / Stroustrup / Mozilla " >> .clang-format
	@echo "ColumnLimit: 100    # 80, 100, 0 (kein Limit)	Maximale Zeilenlänge" >> .clang-format
	@echo "AllowShortIfStatementsOnASingleLine: false   # if/else immer mit Blockklammern umbrechen" >> .clang-format
	@echo "AllowShortLoopsOnASingleLine: false          # true | false Einzeilige for/while Schleifen" >> .clang-format
	@echo "AllowShortFunctionsOnASingleLine: Inline     # None / Inline / All  sehr kurze Funktionen dürfen einzeilig bleiben" >> .clang-format
	@echo "KeepEmptyLinesAtTheStartOfBlocks: false       # keine leeren Zeilen direkt nach {" >> .clang-format
	@echo "SpacesBeforeTrailingComments: 2               # 2 Anzahl Leerzeichen vor Endkommentaren //" >> .clang-format
	@echo "SpaceAfterCStyleCast: true                    # true | false Leerzeichen nach C-Cast: (int) x" >> .clang-format
	@echo "SpaceBeforeParens: ControlStatements          # Never | ControlStatement  Leerzeichen vor ( bei if, while, for" >> .clang-format
	@echo "SpacesInParentheses: true          # true/false	Leerzeichen innerhalb von ()" >> .clang-format
	@echo "BinPackArguments: false  # true/false	Argumente in Funktionsaufrufen auf eine Zeile packen" >> .clang-format
	@echo "BinPackParameters: false # true/false	Gleiches für Funktionsdefinitionen" >> .clang-format
	@echo "SortIncludes: false       # true/false	Automatisch #includes sortieren" >> .clang-format
