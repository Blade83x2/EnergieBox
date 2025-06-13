#include <iostream>
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>

class PVStatus {
    float voltage = 0.0f;
    float current = 0.0f;
    float power = 0.0f;

    static constexpr int WIDTH_LABEL = 12;
    static constexpr int WIDTH_VALUE = 10;
    static constexpr int WIDTH_BAR = 22;
    static constexpr int TOTAL_WIDTH = WIDTH_LABEL + WIDTH_VALUE + 1 + WIDTH_BAR;

    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* WHITE = "\033[1;37m";
    static constexpr const char* BLUEBG = "\033[1;44;97m";

    void clearScreen() const {
        std::cout << "\033[2J\033[H"; // Clear screen + cursor home
    }

    void printBar(float value, float maxValue) const {
        int width = WIDTH_BAR - 2;
        int percent = static_cast<int>((value / maxValue) * 100);
        if (percent > 100) percent = 100;
        if (percent < 0) percent = 0;
        int filled = (percent * width) / 100;
        std::cout << "[";
        for (int i = 0; i < width; ++i) {
            std::cout << (i < filled ? "█" : "░");
        }
        std::cout << "]";
    }

    void printFloat(float value, const char* unit) const {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f%s", value, unit);
        int len = strlen(buffer);
        int padding = WIDTH_VALUE - len;
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << WHITE << buffer << RESET;
    }

    float extractValue(const char* line) {
        float val = 0.0f;
        sscanf(line, "%*[^=]= %f", &val);
        return val;
    }

public:
    bool update() {
        FILE* fp = popen("python3 /Energiebox/Tracer/readall.py 2>&1", "r");
        if (!fp) {
            perror("Fehler beim Ausführen von readall.py");
            return false;
        }
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "PV Array: Aktuelle Spannung in Volt =")) {
                voltage = extractValue(line);
            } else if (strstr(line, "PV Array: Aktueller Strom in Ampere =")) {
                current = extractValue(line);
            } else if (strstr(line, "PV Array: Aktuelle Leistung in Watt =")) {
                power = extractValue(line);
            }
        }
        pclose(fp);
        return true;
    }

    void draw() const {
        clearScreen();

        // Header zentriert
        std::cout << BLUEBG;
        int leftPad = (TOTAL_WIDTH - 9) / 2; // "PV Status"
        int rightPad = TOTAL_WIDTH - leftPad - 9;
        for (int i = 0; i < leftPad; ++i) std::cout << " ";
        std::cout << "PV Status";
        for (int i = 0; i < rightPad; ++i) std::cout << " ";
        std::cout << RESET << "\n\n";

        std::cout.width(WIDTH_LABEL);
        std::cout << std::left << "Spannung:" << " ";
        printFloat(voltage, "V");
        std::cout << " ";
        printBar(voltage, 150.0f);
        std::cout << "\n";

        std::cout.width(WIDTH_LABEL);
        std::cout << std::left << "Strom:" << " ";
        printFloat(current, "A");
        std::cout << " ";
        printBar(current, 100.0f);
        std::cout << "\n";

        std::cout.width(WIDTH_LABEL);
        std::cout << std::left << "Leistung:" << " ";
        printFloat(power, "W");
        std::cout << " ";
        printBar(power, 5000.0f);
        std::cout << "\n\n";
    }
};

int main() {
    PVStatus pv;

    while (true) {
        if (!pv.update()) {
            std::cerr << "Fehler beim Aktualisieren der PV-Daten\n";
            return 1;
        }
        pv.draw();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
