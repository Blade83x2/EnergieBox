/*
 * Grafisches Programm zur Steuerung der Relais
 * Vendor: Johannes Krämer
 * Version: 1.4
 * Date: 19.06.2025
 * 
 * Um das Programm auf dem Raspberry zu beenden, kann folgender Befehl verwendet werden:
 * kill -9 $(pidof gui)
*/
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/main.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <ctime>
#include <iomanip>

// Debug-Modus aktivieren/deaktivieren
bool debug = true;

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

void debugPrint(const std::string& strMsg, LogLevel level = LogLevel::DEBUG) {
    if (debug || level != LogLevel::DEBUG) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* tm_time = std::localtime(&now_time);

        std::ostringstream logPrefix;
        logPrefix << std::put_time(tm_time, "%Y-%m-%d %H:%M:%S") << " ";

        switch (level) {
            case LogLevel::DEBUG: logPrefix << "[DEBUG] "; break;
            case LogLevel::INFO:  logPrefix << "[INFO ] "; break;
            case LogLevel::WARN:  logPrefix << "[WARN ] "; break;
            case LogLevel::ERROR: logPrefix << "[ERROR] "; break;
        }
        std::cout << logPrefix.str() << strMsg << std::endl;
    }
}


// Struktur für Informationen zu einem einzelnen Relais
struct RelaisInfo {
    int nummer;                // Relaisnummer
    std::string name;         // Anzeigename
    bool aktiv;               // Aktueller Zustand (ein/aus)
    Gtk::Button* button;      // Zeiger auf zugehörigen Button
    sigc::connection handler; // Signal-Handler für Klick-Ereignis
};

// Einfache INI-Datei-Parser-Klasse
class IniReader {
public:
    explicit IniReader(const std::string& path) {
        std::ifstream file(path);
        
        
        
        
        
        debugPrint("Lese Datei: " + path, LogLevel::INFO);
        std::cout << "[GUI] Lese Datei: " << path << std::endl;
        if (!file) {
            std::cerr << "Fehler beim Öffnen der INI: " << path << std::endl;
            return;
        }
        std::string line;
        std::string currentSection;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;
            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size()-2);
                continue;
            }
            auto eqpos = line.find('=');
            if (eqpos == std::string::npos) continue;
            std::string key = line.substr(0, eqpos);
            std::string value = line.substr(eqpos + 1);
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            data_[currentSection + "/" + key] = value;
        }
    }

    std::string get(const std::string& sectionKey, const std::string& defaultVal="") const {
        auto it = data_.find(sectionKey);
        if (it != data_.end()) return it->second;
        return defaultVal;
    }

private:
    std::map<std::string, std::string> data_; // Schlüssel: "Sektion/Key" -> Wert
};

// Hauptklasse der GUI-Anwendung
class GUI : public Gtk::Window {
public:
    GUI() {
        set_title("Energiebox GUI");           // Fenstertitel setzen
        set_default_size(800, 480);            // Größe für Touchdisplay
        fullscreen();                           // Vollbildmodus aktivieren
        // Ränder für das Notebook (Tabs) setzen
        notebook_.set_margin_top(25);
        notebook_.set_margin_bottom(25);
        notebook_.set_margin_start(25);
        notebook_.set_margin_end(25);
        // Tabs hinzufügen: Energiebox, 12V, 230V
        notebook_.append_page(*Gtk::manage(new Gtk::Box()), "Energiebox");
        notebook_.append_page(build_tab_12v(), "12V");
        notebook_.append_page(build_tab_230v(), "230V");
        add(notebook_); // Notebook dem Fenster hinzufügen
        apply_css(); // Schriftgröße etc. anpassen
        // Event-Handler für Tab-Wechsel, um Timer zurückzusetzen
        notebook_.signal_switch_page().connect(sigc::mem_fun(*this, &GUI::on_tab_switched));
        // Timer: nach 30 Sekunden Inaktivität automatisch auf Info-Tab wechseln
        last_interaction_time_ = std::time(nullptr);
        Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::check_idle_time), 10);
        // Erstmal Energiebox-Tab direkt aktualisieren und füllen
        //update_energiebox_tab();
        show_all_children();
    }

private:
    sigc::connection relais12V_timer_connection_;
    sigc::connection relais230V_timer_connection_;
    sigc::connection energiebox_timer_connection_;  
    // Stilanpassung (z. B. Schriftgröße)
    void apply_css() {
        auto css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data(
            "button { font-size: 12pt; min-height: 30px; }"
            "label { font-size: 14pt; }"
            "treeview, treeview.view { font-size: 14pt; }"
        );
        auto screen = Gdk::Screen::get_default();
        Gtk::StyleContext::add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    // 12V Tab erstellen (mit Leistungsprüfung)
    Gtk::Widget& build_tab_12v() {
        return build_tab("/Energiebox/12V/config.ini", "/Energiebox/12V/12V", true);
    }

    // 230V Tab erstellen (ohne Controller-Verbrauch)
    Gtk::Widget& build_tab_230v() {
        return build_tab("/Energiebox/230V/config.ini", "/Energiebox/230V/230V", false);
    }

    // Generische Funktion für Relais-Tabs
    Gtk::Grid& build_tab(const std::string& configPath, const std::string& binaryPath, bool checkPower) {
        Gtk::Grid* grid = Gtk::manage(new Gtk::Grid());
        grid->set_row_spacing(7);
        grid->set_column_spacing(7);
        grid->set_margin_top(10);
        grid->set_margin_left(10);
        grid->set_margin_right(10);
        IniReader ini(configPath);
        int anzahl = std::stoi(ini.get("mcp/numberOfRelaisActive", "0"));
        const int spalten = 2;
        const int max_zeilen = 8;
        if (anzahl > spalten * max_zeilen) anzahl = spalten * max_zeilen;
        for (int i = 1; i <= anzahl; ++i) {
            std::string sektion = "Relais " + std::to_string(i);
            std::string name = ini.get(sektion + "/name", "Relais " + std::to_string(i));
            bool isAktiv = ini.get(sektion + "/eltakoState", "0") == "1";
            Gtk::Button* btn = Gtk::manage(new Gtk::Button(name));
            btn->set_hexpand(true);
            btn->set_size_request(0, 32);
            btn->override_background_color(Gdk::RGBA(isAktiv ? "lightgreen" : "lightcoral"));
            if (checkPower) {
                relais12v_buttons_[i] = btn;
                relais12v_status_[i] = isAktiv;
            } else {
                relais230v_buttons_[i] = btn;
                relais230v_status_[i] = isAktiv;
            }
            auto* relais = new RelaisInfo{i, name, isAktiv, btn};
            relais->handler = btn->signal_clicked().connect([=]() mutable {
                last_interaction_time_ = std::time(nullptr); // Timer zurücksetzen bei Klick
                debugPrint("Button gedrückt: Relais: " + std::to_string(i) + " (" + name + ")", LogLevel::INFO);
                IniReader ini(configPath);
                std::string canStart = ini.get(sektion + "/canStartFromGui", "1");
                if (canStart != "1") {
                    Gtk::MessageDialog dialog(*this, "Schaltung nicht erlaubt", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
                    dialog.set_secondary_text("Dieses Relais kann nicht über die GUI geschaltet werden.");
                    dialog.run();
                    debugPrint("Zugriff verweigert: " + name, LogLevel::INFO);
                    return;
                }
                bool neu = !relais->aktiv;
                // Leistungsprüfung
                if (neu && checkPower) {
                    int maxPower = std::stoi(ini.get("mcp/maxPConverter", "0")) - std::stoi(ini.get("mcp/maxPMicroController", "0"));
                    int usedPower = 0;
                    for (int j = 1; j <= anzahl; ++j) {
                        if (j == i) continue;
                        if (ini.get("Relais " + std::to_string(j) + "/eltakoState", "0") == "1")
                            usedPower += std::stoi(ini.get("Relais " + std::to_string(j) + "/pMax", "0"));
                    }
                    int needed = std::stoi(ini.get(sektion + "/pMax", "0"));
                    if (needed > (maxPower - usedPower)) {
                        Gtk::MessageDialog dialog(*this, "Nicht genug Leistung", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
                        dialog.set_secondary_text("Dieses Relais kann nicht eingeschaltet werden, da nicht genug Leistung verfügbar ist.");
                        dialog.run();
                        debugPrint("Zuwenig Leistung für: Relais " + std::to_string(i) + " (" + name + ")", LogLevel::INFO);
                        return;
                    }
                } else if (neu && !checkPower) {
                    int maxPower = std::stoi(ini.get("mcp/maxOutputPower", "0"));
                    int usedPower = 0;
                    for (int j = 1; j <= anzahl; ++j) {
                        if (j == i) continue;
                        if (ini.get("Relais " + std::to_string(j) + "/eltakoState", "0") == "1")
                            usedPower += std::stoi(ini.get("Relais " + std::to_string(j) + "/pMax", "0"));
                    }
                    int needed = std::stoi(ini.get(sektion + "/pMax", "0"));
                    if (needed > (maxPower - usedPower)) {
                        Gtk::MessageDialog dialog(*this, "Nicht genug Leistung", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
                        dialog.set_secondary_text("Dieses Relais kann nicht eingeschaltet werden, da nicht genug Leistung verfügbar ist.");
                        dialog.run();
                        debugPrint("Zuwenig Leistung für: Relais " + std::to_string(i) + " (" + name + ")", LogLevel::INFO);
                        return;
                    }
                }
                // Schaltbefehl ausführen
                std::string cmd = binaryPath + " " + std::to_string(i) + " " + std::to_string(neu ? 1 : 0) + " 0";
                std::system(cmd.c_str());
               
                debugPrint("Schalte " + name + " auf " + (neu ? "AN" : "AUS"), LogLevel::INFO);
                
                btn->override_background_color(Gdk::RGBA(neu ? "lightgreen" : "lightcoral"));
                relais->aktiv = neu;
                if (checkPower) relais12v_status_[i] = neu;
                else relais230v_status_[i] = neu;
            });

            int zeile = (i - 1) / spalten;
            int spalte = (i - 1) % spalten;
            grid->attach(*btn, spalte, zeile, 1, 1);
        }
        return *grid;
    }

    // Event-Handler für Tab-Wechsel
    void on_tab_switched(Gtk::Widget* page, guint page_num) {
        last_interaction_time_ = std::time(nullptr);
        Gtk::Widget* tab_label_widget = notebook_.get_tab_label(*page);
        Gtk::Label* tab_label = dynamic_cast<Gtk::Label*>(tab_label_widget);
        std::string tabName = tab_label ? tab_label->get_text() : "<unbekannter Tab>";
        debugPrint("Tab gewechselt zu " + tabName, LogLevel::INFO);
        if (tabName == "12V") {
            if (!relais12V_timer_connection_.connected()) {
                relais12V_timer_connection_ = Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::refresh_relais12V_status), 10);
                debugPrint("Relais12V-Timer gestartet", LogLevel::INFO);
            }
        } else {
            if (relais12V_timer_connection_.connected()) {
                relais12V_timer_connection_.disconnect();
                debugPrint("Relais12V-Timer gestoppt", LogLevel::INFO);
            }
        }
        if (tabName == "230V") {
            if (!relais230V_timer_connection_.connected()) {
                relais230V_timer_connection_ = Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::refresh_relais230V_status), 10);
                debugPrint("Relais230V-Timer gestartet", LogLevel::INFO);
            }
        } else {
            if (relais230V_timer_connection_.connected()) {
                relais230V_timer_connection_.disconnect();
                debugPrint("Relais230V-Timer gestoppt", LogLevel::INFO);
            }
        }
        if (tabName == "Energiebox") {
            if (!energiebox_timer_connection_.connected()) {
                energiebox_timer_connection_ =  Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::update_energiebox_tab), 180);
                debugPrint("Energiebox-Timer gestartet", LogLevel::INFO);
                update_energiebox_tab();
            }
        } else {
            if (energiebox_timer_connection_.connected()) {
                energiebox_timer_connection_.disconnect();
                debugPrint("Energiebox-Timer gestoppt", LogLevel::INFO);
            }
        }
    }
    // 12V config.ini auslesen und eventuelle Relais Zustandsänderungen in der GUI updaten
    bool refresh_relais12V_status() {
        IniReader ini12v("/Energiebox/12V/config.ini");
        for (auto& [nr, btn] : relais12v_buttons_) {
            bool isAktiv = ini12v.get("Relais " + std::to_string(nr) + "/eltakoState", "0") == "1";
            std::string neuerName = ini12v.get("Relais " + std::to_string(nr) + "/name", "Relais " + std::to_string(nr));
            if (isAktiv != relais12v_status_[nr]) {
                relais12v_status_[nr] = isAktiv;
                btn->override_background_color(Gdk::RGBA(isAktiv ? "lightgreen" : "lightcoral"));
            }
            // Button-Beschriftung ggf. aktualisieren
            if (btn->get_label() != neuerName) {
                btn->set_label(neuerName);
            }
        }
        return true;
    }
    // 230V config.ini auslesen und eventuelle Relais Zustandsänderungen in der GUI updaten
    bool refresh_relais230V_status() {
        IniReader ini230v("/Energiebox/230V/config.ini");
        for (auto& [nr, btn] : relais230v_buttons_) {
            bool isAktiv = ini230v.get("Relais " + std::to_string(nr) + "/eltakoState", "0") == "1";
            std::string neuerName = ini230v.get("Relais " + std::to_string(nr) + "/name", "Relais " + std::to_string(nr));
            if (isAktiv != relais230v_status_[nr]) {
                relais230v_status_[nr] = isAktiv;
                btn->override_background_color(Gdk::RGBA(isAktiv ? "lightgreen" : "lightcoral"));
            }
            // Button-Beschriftung ggf. aktualisieren
            if (btn->get_label() != neuerName) {
                btn->set_label(neuerName);
            }
        }
        return true;
    }
    
    // Prüft ob seit letzter Aktion 3 Minuten vergangen sind und wechselt ggf. zum Info-Tab
    bool check_idle_time() {
        time_t now = std::time(nullptr);
        if (now - last_interaction_time_ >= 180) {
            if (notebook_.get_current_page() != 0) { // Wenn nicht schon Info-Tab
                notebook_.set_current_page(0);
                debugPrint("180 Sekunden Inaktivität – wechsle zum Info-Tab", LogLevel::INFO);
            }
        }
        
        return true; // Timer wiederholen
    }

    // Hilfsfunktion: Erzeugt eine Tabelle (Gtk::Grid) mit Beschriftungen und Werten, rechtsbündig
    Gtk::Grid* create_data_table(const std::vector<std::pair<std::string, std::string>>& data, const std::string& headline) {
        Gtk::Grid* grid = Gtk::manage(new Gtk::Grid());
        grid->set_row_spacing(8);
        grid->set_column_spacing(50);
        // Überschrift als fettes Label (links ausgerichtet, größer)
        auto* title = Gtk::manage(new Gtk::Label("<b>" + headline + "</b>"));
        title->set_use_markup(true);
        title->set_halign(Gtk::ALIGN_START);
        title->set_margin_bottom(10);
        grid->attach(*title, 0, 0, 2, 1);
        int row = 1;
        for (const auto& [label, value] : data) {
            auto* label_widget = Gtk::manage(new Gtk::Label(label));
            label_widget->set_halign(Gtk::ALIGN_START);
            auto* value_widget = Gtk::manage(new Gtk::Label(value));
            value_widget->set_halign(Gtk::ALIGN_END);
            grid->attach(*label_widget, 0, row, 1, 1);
            grid->attach(*value_widget, 1, row, 1, 1);
            ++row;
        }
        return grid;
    }

    // Liest die Datei /Energiebox/Tracer/trace.txt aus, parsed die Werte und aktualisiert den Info-Tab
    bool update_energiebox_tab() {
        std::ifstream file("/Energiebox/Tracer/trace.txt");
        debugPrint("Lese /Energiebox/Tracer/trace.txt", LogLevel::INFO);
        if (!file) {
            std::cerr << "Fehler beim Öffnen der Datei /Energiebox/Tracer/trace.txt" << std::endl;
            return true; // Timer soll weitermachen
        }
        // Map zum Speichern der ausgelesenen Werte (Schlüssel: String davor, Wert inkl. Einheit dahinter)
        std::map<std::string, std::string> values;
        std::string line;
        while (std::getline(file, line)) {
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            // Entferne führende und folgende Leerzeichen bei key und val
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            val.erase(0, val.find_first_not_of(" \t\r\n"));
            val.erase(val.find_last_not_of(" \t\r\n") + 1);
            values[key] = val;
        }
        // Daten für PV Tabelle
        std::vector<std::pair<std::string, std::string>> pv_data = {
            {"Spannung", values["PV Array: Aktuelle Spannung in Volt"]},
            {"Ampere", values["PV Array: Aktueller Strom in Ampere"]},
            {"Watt", values["PV Array: Aktuelle Leistung in Watt"]},
            {"Heute generiert", values["PV Array: Generierte Energie heute"]}
        };
        // Daten für Batterie Tabelle
        std::vector<std::pair<std::string, std::string>> battery_data = {
            {"Spannung", values["Batterie: Aktuelle Spannung in Volt"]},
            {"Ampere", values["Batterie: Derzeitiger Ladestrom in Ampere"]},
            {"Watt", values["Batterie: Derzeitige Ladeleistung in Watt"]},
            {"SOC", values["Batterie: Ladezustand in Prozent"]}
        };
        // Alten Inhalt entfernen falls vorhanden
        if (info_content_) {
            auto* tab_content = dynamic_cast<Gtk::Container*>(notebook_.get_nth_page(0));
            if (tab_content) {
                tab_content->remove(*info_content_);
            }
        }
        // Neues Widget erstellen und Tabellen nebeneinander packen
        auto* hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 70));
        hbox->set_halign(Gtk::ALIGN_CENTER);
        hbox->set_valign(Gtk::ALIGN_CENTER);
        auto* pv_table = create_data_table(pv_data, "PV Eingang:");
        auto* battery_table = create_data_table(battery_data, "Batterie Ladung:");
        pv_table->set_hexpand(true);
        battery_table->set_hexpand(true);
        hbox->pack_start(*pv_table, Gtk::PACK_SHRINK);
        hbox->pack_start(*battery_table, Gtk::PACK_SHRINK);
        info_content_ = hbox;
        auto* tab_content = dynamic_cast<Gtk::Container*>(notebook_.get_nth_page(0));
        if (tab_content) {
            tab_content->add(*info_content_);
        }
        info_content_->show_all();
        return true; // Timer soll weiterlaufen
    }

    Gtk::Notebook notebook_;       // Tab-Widget
    Gtk::Widget* info_content_ = nullptr; // Aktueller Inhalt im Info-Tab (für ersetzen)
    time_t last_interaction_time_; // Zeitstempel letzter Nutzer-Interaktion
    std::map<int, Gtk::Button*> relais12v_buttons_;
    std::map<int, bool> relais12v_status_;
    std::map<int, Gtk::Button*> relais230v_buttons_;
    std::map<int, bool> relais230v_status_;
};

// Einstiegspunkt der Anwendung
int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "de.cplusplus-development.gui");
    GUI window;
    return app->run(window);
    debugPrint("Programm gestartet", LogLevel::INFO);
}
