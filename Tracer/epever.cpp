#include "PVStatus.hpp"
#include "BatteryStatus.hpp"
#include <thread>
#include <chrono>

int main() {
    PVStatus pv;
    BatteryStatus battery;

    while (true) {
        if (!pv.update() || !battery.update()) {
            std::cerr << "Fehler beim Lesen der Daten\n";
            return 1;
        }

        StatusBlock::clearScreen();
        pv.draw();
        battery.draw();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
