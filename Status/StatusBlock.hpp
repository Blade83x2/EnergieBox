#pragma once
#include <string>

class StatusBlock {
   protected:
    float extractValue(const std::string &line);

   public:
    virtual StatusBlock &update() = 0;
    virtual void draw() const = 0;

    // Hilfsfunktionen für Ausgabe und Terminalsteuerung
    static void clearScreen();
    static void printHeader(const std::string &header);
    static void printFloat(float value, const char *unit, bool showPercent = false, bool showSpace = true);
    static void printBar(float value, float maxValue);

    // Breite für Labels (für Ausrichtung)
    static constexpr int WIDTH_LABEL = 18;
};
