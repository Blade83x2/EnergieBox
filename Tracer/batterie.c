#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RESET  "\033[0m"
#define CYAN   "\033[1;36m"
#define WHITE  "\033[1;37m"
#define BLUEBG "\033[1;44;97m"
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"

// Breiten der Spalten
#define WIDTH_LABEL 12
#define WIDTH_VALUE 6
#define WIDTH_BAR 22
#define TOTAL_WIDTH (WIDTH_LABEL + WIDTH_VALUE + 1 + WIDTH_BAR)

// Prozentbalken anzeigen
void print_bar(int percent) {
    int width = WIDTH_BAR - 2; // für [ ]
    int filled = (percent * width) / 100;
    printf("[");
    for (int i = 0; i < width; i++) {
        printf(i < filled ? "█" : "░");
    }
    printf("]");
}

// Prozent aus Zeile extrahieren
int extract_percent(const char* line) {
    int value = 0;
    sscanf(line, "%*[^=]= %d%%", &value);
    return value;
}

// Float aus Zeile extrahieren
float extract_value(const char* line) {
    float value = 0.0;
    sscanf(line, "%*[^=]= %f", &value);
    return value;
}

// Spannung (48–56V) in Prozent umrechnen
int voltage_to_percent(float voltage) {
    float min = 48.0, max = 56.0;
    if (voltage < min) voltage = min;
    if (voltage > max) voltage = max;
    return (int)(((voltage - min) / (max - min)) * 100);
}

// Farbe für Ladezustand wählen
const char* soc_color(int soc) {
    if (soc < 20)
        return RED;
    else if (soc < 50)
        return YELLOW;
    else
        return GREEN;
}

// Formatierte Zahl mit Farbe ausgeben, rechtsbündig in WIDTH_VALUE Breite
void print_colored_number(int value, const char* color) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d%%", value);
    int len = (int)strlen(buffer);
    int padding = WIDTH_VALUE - len;
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s%s%s", color, buffer, RESET);
}

// Für Spannung mit 2 Nachkommastellen
void print_colored_float(float value, const char* color) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.2fV", value);
    int len = (int)strlen(buffer);
    int padding = WIDTH_VALUE - len;
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s%s%s", color, buffer, RESET);
}

void draw(int soc, float voltage) {
    int volt_percent = voltage_to_percent(voltage);

    // Bildschirm löschen und Cursor oben links
    printf("\033[2J\033[H");

    // Überschrift zentriert mit Hintergrund, exakt TOTAL_WIDTH breit
    printf("%s", BLUEBG);
    int left_padding = (TOTAL_WIDTH - 15) / 2; // "Batterie Status" Länge=15
    int right_padding = TOTAL_WIDTH - left_padding - 15;
    for (int i = 0; i < left_padding; i++) printf(" ");
    printf("Batterie Status");
    for (int i = 0; i < right_padding; i++) printf(" ");
    printf("%s\n\n", RESET);

    // Zeile Label linksbündig
    printf("%-*s ", WIDTH_LABEL, "Ladezustand:");
    // Zahl rechtsbündig + farbig
    print_colored_number(soc, soc_color(soc));
    printf(" ");
    // Balken rechts daneben
    print_bar(soc);
    printf("\n");

    // Spannung
    printf("%-*s ", WIDTH_LABEL, "Spannung:");
    print_colored_float(voltage, WHITE);
    printf(" ");
    print_bar(volt_percent);
    printf("\n\n");

    fflush(stdout);
}

int main() {
    while (1) {
        FILE* fp = popen("python3 /Energiebox/Tracer/readall.py 2>&1", "r");
        if (!fp) {
            perror("Fehler beim Ausführen von readall.py");
            return 1;
        }

        char line[256];
        int soc = -1;
        float voltage = 0.0;

        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "Batterie: Ladezustand in Prozent ="))
                soc = extract_percent(line);
            else if (strstr(line, "Batterie: Aktuelle Spannung in Volt ="))
                voltage = extract_value(line);
        }
        pclose(fp);

        draw(soc, voltage);

        sleep(2);
    }
    return 0;
}
