#include "PVStatus.hpp"
#include "BatteryStatus.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    PVStatus pv;
    BatteryStatus battery;

    while (true) {
        if (!StatusBlock::loadTraceFile("/Energiebox/Tracer/trace.txt")) {
            std::cerr << "Fehler: trace.txt konnte nicht geladen werden\n";
            return 1;
        }

        if (!pv.update() || !battery.update()) {
            std::cerr << "Fehler beim Verarbeiten der Daten\n";
            return 1;
        }

        StatusBlock::clearScreen();
        pv.draw();
        battery.draw();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
