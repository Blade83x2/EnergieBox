/*
 * Grafisches Programm zur Steuerung der Relais
 * Vendor: Johannes Krämer
 * Version: 1.5
 * Date: 21.06.2025
 *
 * Um das Programm auf dem Raspberry zu beenden, kann folgender Befehl verwendet werden:
 * kill -9 $(pidof gui)
 *
 */
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/progressbar.h>
#include <glibmm/main.h>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <glibmm/optioncontext.h>
#include <glibmm/optiongroup.h>
#include "MySQLiWrapper.h"

// Debug-Modus aktivieren/deaktivieren
bool debug = false;
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

void debugPrint(const std::string &strMsg, LogLevel level = LogLevel::DEBUG) {
    if (debug) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *tm_time = std::localtime(&now_time);
        std::ostringstream logPrefix;
        logPrefix << std::put_time(tm_time, "%Y-%m-%d %H:%M:%S") << " ";
        switch (level) {
            case LogLevel::DEBUG:
                logPrefix << "[DEBUG] ";
                break;
            case LogLevel::INFO:
                logPrefix << "[INFO ] ";
                break;
            case LogLevel::WARN:
                logPrefix << "[WARN ] ";
                break;
            case LogLevel::ERROR:
                logPrefix << "[ERROR] ";
                break;
        }
        std::cout << logPrefix.str() << strMsg << std::endl;
    }
}

// Struktur für Informationen zu einem einzelnen Relais
struct RelaisInfo {
    int nummer;                // Relaisnummer
    std::string name;          // Anzeigename
    bool aktiv;                // Aktueller Zustand (ein/aus)
    Gtk::Button *button;       // Zeiger auf zugehörigen Button
    sigc::connection handler;  // Signal-Handler für Klick-Ereignis
};

// Einfache INI-Datei-Parser-Klasse
class IniReader {
   public:
    explicit IniReader(const std::string &path) {
        std::ifstream file(path);
        debugPrint("Lese Datei: " + path, LogLevel::INFO);
        if (!file) {
            std::cerr << "Fehler beim Öffnen der INI: " << path << std::endl;
            return;
        }
        std::string line;
        std::string currentSection;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
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
    std::string get(const std::string &sectionKey, const std::string &defaultVal = "") const {
        auto it = data_.find(sectionKey);
        if (it != data_.end()) return it->second;
        return defaultVal;
    }

   private:
    std::map<std::string, std::string> data_;  // Schlüssel: "Sektion/Key" -> Wert
};

// Hauptklasse der GUI-Anwendung
class GUI : public Gtk::Window {
   public:
    GUI() {
        set_title("⚡ Energiebox");
        set_icon_from_file("/Energiebox/gui/icon.png");
        // Hauptcontainer für Notebook und Status-Leiste
        main_box_.set_orientation(Gtk::ORIENTATION_VERTICAL);
        // Ränder für das Notebook (Tabs) setzen
        notebook_.set_margin_top(10);
        notebook_.set_margin_bottom(10);
        notebook_.set_margin_start(10);
        notebook_.set_margin_end(10);
        // Tabs hinzufügen: Energiebox, 12V, 230V
        notebook_.append_page(build_energiebox_tab(), "💻 Energiebox");
        notebook_.append_page(build_tab_12v(), "⚡ 12V");
        notebook_.append_page(build_tab_230v(), "⚡ 230V");
        main_box_.pack_start(notebook_, Gtk::PACK_EXPAND_WIDGET);
        // Status-Leiste erstellen
        create_status_bar();
        main_box_.pack_end(status_box_, Gtk::PACK_SHRINK);
        add(main_box_);  // Hauptcontainer dem Fenster hinzufügen
        apply_css();     // Schriftgröße etc. anpassen
        // Event-Handler für Tab-Wechsel, um Timer zurückzusetzen
        notebook_.signal_switch_page().connect(sigc::mem_fun(*this, &GUI::on_tab_switched));
        // Timer: nach 30 Sekunden Inaktivität automatisch auf Info-Tab wechseln
        last_interaction_time_ = std::time(nullptr);
        Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::check_idle_time), 10);
        // Timer für Uhrzeit-Update
        Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::update_time), 1);
        // Erstmal Energiebox-Tab direkt aktualisieren und füllen
        show_all_children();
    }

   private:
    Gtk::Box main_box_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box status_box_{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Label system_status_label_;
    Gtk::Label time_label_;
    sigc::connection relais12V_timer_connection_;
    sigc::connection relais230V_timer_connection_;
    sigc::connection energiebox_timer_connection_;
    // CSS aus Datei laden
    void apply_css() {
        auto css_provider = Gtk::CssProvider::create();
        try {
            css_provider->load_from_path("/Energiebox/gui/style.css");
        } catch (const Glib::Error &ex) {
            debugPrint("Fehler beim Laden der CSS-Datei:  " + ex.what(), LogLevel::ERROR);
            return;
        }
        auto screen = Gdk::Screen::get_default();
        Gtk::StyleContext::add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    // Energiebox Tab erstellen
    Gtk::Widget &build_energiebox_tab() {
        // Main Container erstellen
        auto *main_container = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 20));
        main_container->set_margin_top(10);
        main_container->set_margin_bottom(10);
        main_container->set_margin_start(10);
        main_container->set_margin_end(10);
        // Box für Grafik und Texte erstellen
        auto *welcome_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 10));
        welcome_box->set_hexpand(true);  // maximale Breite verwenden
        welcome_box->set_valign(Gtk::ALIGN_CENTER);
        welcome_box->get_style_context()->add_class("welcome-box");
        // Bild (links in box)
        auto *image = Gtk::manage(new Gtk::Image("/Energiebox/gui/logo.png"));
        image->set_pixel_size(80);  // hübsche Größe
        image->get_style_context()->add_class("logo");
        image->set_valign(Gtk::ALIGN_CENTER);  // vertikal mittig
        // bild in welcome_box laden
        welcome_box->pack_start(*image, Gtk::PACK_SHRINK);
        // Container für Textbereich (rechts in box)
        auto *text_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 1));
        text_box->set_valign(Gtk::ALIGN_CENTER);  // der inhalt dieser box von der höhe her mittig positionieren
        // inhalt für text_box
        auto *title = Gtk::manage(new Gtk::Label("Energiebox V.5.0"));
        title->get_style_context()->add_class("welcome-title");
        title->set_halign(Gtk::ALIGN_START);  // text links ausrichten
        auto *subtitle = Gtk::manage(new Gtk::Label("© 2026 Johannes Krämer"));
        subtitle->get_style_context()->add_class("welcome-subtitle");
        subtitle->set_halign(Gtk::ALIGN_START);  // text links ausrichten
        // inhalt in die text_box laden
        text_box->pack_start(*title, Gtk::PACK_SHRINK);
        text_box->pack_start(*subtitle, Gtk::PACK_SHRINK);
        // text_box in welcome_box laden
        welcome_box->pack_start(*text_box, Gtk::PACK_EXPAND_WIDGET);
        // welcome_box in hauptcontainer laden
        main_container->pack_start(*welcome_box, Gtk::PACK_SHRINK);
        // Platzhalter für Daten-Tabellen (werden dynamisch erstellt)
        energiebox_data_container_ = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
        // und in hauptcontainer laden
        main_container->pack_start(*energiebox_data_container_, Gtk::PACK_SHRINK);
        return *main_container;
    }
    // 12V Tab erstellen (mit Leistungsprüfung)
    Gtk::Widget &build_tab_12v() { return build_tab("/Energiebox/12V/config.ini", "/Energiebox/12V/12V", true, "12V"); }
    // 230V Tab erstellen (ohne Controller-Verbrauch)
    Gtk::Widget &build_tab_230v() { return build_tab("/Energiebox/230V/config.ini", "/Energiebox/230V/230V", false, "230V"); }
    // Generische Funktion für Relais-Tabs
    Gtk::Grid &build_tab(const std::string &configPath, const std::string &binaryPath, bool checkPower, const std::string &tabType) {
        auto *main_container = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 15));
        main_container->set_margin_top(15);
        main_container->set_margin_bottom(15);
        main_container->set_margin_start(15);
        main_container->set_margin_end(15);
        // Header mit Titel und Bild
        auto *header = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 20));
        header->set_hexpand(true);  // maximale Breite verwenden
        header->set_valign(Gtk::ALIGN_CENTER);
        header->get_style_context()->add_class("schalt-boxen");
        // Container für Textbereich (rechts in box)
        auto *text_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 1));
        text_box->set_valign(Gtk::ALIGN_CENTER);  // der inhalt dieser box von der höhe her mittig positionieren
        // inhalt für text_box
        auto *title = Gtk::manage(new Gtk::Label("🔌 " + tabType + " Geräte Steuerung"));
        title->get_style_context()->add_class("data-title");
        title->set_halign(Gtk::ALIGN_CENTER);  // text links ausrichten
        text_box->pack_start(*title, Gtk::PACK_SHRINK);
        header->pack_start(*text_box, Gtk::PACK_SHRINK);
        main_container->pack_start(*header, Gtk::PACK_SHRINK);
        // Relais-Buttons Grid
        Gtk::Grid *grid = Gtk::manage(new Gtk::Grid());
        grid->set_row_spacing(10);
        grid->set_column_spacing(5);
        grid->set_halign(Gtk::ALIGN_CENTER);
        IniReader ini(configPath);
        int anzahl = std::stoi(ini.get("mcp/numberOfRelaisActive", "0"));
        const int spalten = 4;
        const int max_zeilen = 4;
        if (anzahl > spalten * max_zeilen) anzahl = spalten * max_zeilen;
        for (int i = 1; i <= anzahl; ++i) {
            std::string sektion = "Relais " + std::to_string(i);
            std::string name = ini.get(sektion + "/name", "Relais " + std::to_string(i));
            bool isAktiv = ini.get(sektion + "/eltakoState", "0") == "1";
            Gtk::Button *btn = Gtk::manage(new Gtk::Button(name));
            btn->set_hexpand(true);
            btn->set_size_request(180, 50);
            // CSS-Klassen für Styling
            btn->get_style_context()->add_class("relais-button");
            btn->get_style_context()->add_class(isAktiv ? "relais-active" : "relais-inactive");
            if (checkPower) {
                relais12v_buttons_[i] = btn;
                relais12v_status_[i] = isAktiv;
            } else {
                relais230v_buttons_[i] = btn;
                relais230v_status_[i] = isAktiv;
            }
            auto *relais = new RelaisInfo{i, name, isAktiv, btn};
            relais->handler = btn->signal_clicked().connect([=]() mutable {
                last_interaction_time_ = std::time(nullptr);  // Timer zurücksetzen bei Klick
                debugPrint("Button gedrückt: Relais: " + std::to_string(i) + " (" + name + ")", LogLevel::INFO);
                IniReader ini(configPath);
                std::string canStart = ini.get(sektion + "/canStartFromGui", "1");
                if (canStart != "1") {
                    Gtk::MessageDialog dialog(*this, "Schaltung nicht erlaubt", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
                    dialog.set_secondary_text("Dieses Relais darf nicht über die GUI geschaltet werden.");
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
                        if (ini.get("Relais " + std::to_string(j) + "/eltakoState", "0") == "1") usedPower += std::stoi(ini.get("Relais " + std::to_string(j) + "/pMax", "0"));
                    }
                    int needed = std::stoi(ini.get(sektion + "/pMax", "0"));
                    if (needed > (maxPower - usedPower)) {
                        Gtk::MessageDialog dialog(*this, "Nicht genug Leistung", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
                        dialog.set_secondary_text("Dieses Relais darf nicht eingeschaltet werden, da nicht genug Leistung verfügbar ist.");
                        dialog.run();
                        debugPrint("Zuwenig Leistung für: Relais " + std::to_string(i) + " (" + name + ")", LogLevel::INFO);
                        return;
                    }
                } else if (neu && !checkPower) {
                    int maxPower = std::stoi(ini.get("mcp/maxOutputPower", "0"));
                    int usedPower = 0;
                    for (int j = 1; j <= anzahl; ++j) {
                        if (j == i) continue;
                        if (ini.get("Relais " + std::to_string(j) + "/eltakoState", "0") == "1") usedPower += std::stoi(ini.get("Relais " + std::to_string(j) + "/pMax", "0"));
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
                system_status_label_.set_text("💬 System: Schalte " + name + " auf " + (neu ? "AN" : "AUS"));
                debugPrint("Schalte " + name + " auf " + (neu ? "AN" : "AUS"), LogLevel::INFO);
                // CSS-Klassen aktualisieren
                btn->get_style_context()->remove_class(neu ? "relais-inactive" : "relais-active");
                btn->get_style_context()->add_class(neu ? "relais-active" : "relais-inactive");
                relais->aktiv = neu;
                if (checkPower)
                    relais12v_status_[i] = neu;
                else
                    relais230v_status_[i] = neu;
            });
            int zeile = (i - 1) / spalten;
            int spalte = (i - 1) % spalten;
            grid->attach(*btn, spalte, zeile, 1, 1);
        }
        main_container->pack_start(*grid, Gtk::PACK_EXPAND_WIDGET);
        // Wrapper für Grid zurückgeben (Hack für Kompatibilität)
        auto *wrapper = Gtk::manage(new Gtk::Grid());
        wrapper->attach(*main_container, 0, 0, 1, 1);
        return *wrapper;
    }

    // Status-Leiste erstellen
    void create_status_bar() {
        status_box_.get_style_context()->add_class("status-bar");
        status_box_.set_spacing(20);
        // System Status
        system_status_label_.set_text("💬 System: Betriebsbereit");
        system_status_label_.get_style_context()->add_class("status-text");
        system_status_label_.set_halign(Gtk::ALIGN_START);
        // Trennlinie
        auto *separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL));
        // Uhrzeit
        time_label_.get_style_context()->add_class("status-text");
        time_label_.set_halign(Gtk::ALIGN_END);
        update_time();  // Initial setzen
        status_box_.pack_start(system_status_label_, Gtk::PACK_SHRINK);
        status_box_.pack_start(*separator, Gtk::PACK_SHRINK);
        status_box_.pack_end(time_label_, Gtk::PACK_SHRINK);
    }
    // Uhrzeit aktualisieren
    bool update_time() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *tm_time = std::localtime(&now_time);
        std::ostringstream timeStream;
        timeStream << std::put_time(tm_time, "%H:%M:%S | %d.%m.%Y");
        time_label_.set_text(timeStream.str());
        return true;  // Timer wiederholen
    }
    // Event-Handler für Tab-Wechsel
    void on_tab_switched(Gtk::Widget *page, guint page_num) {
        last_interaction_time_ = std::time(nullptr);
        Gtk::Widget *tab_label_widget = notebook_.get_tab_label(*page);
        Gtk::Label *tab_label = dynamic_cast<Gtk::Label *>(tab_label_widget);
        std::string tabName = tab_label ? tab_label->get_text() : "<unbekannter Tab>";
        debugPrint("Tab gewechselt zu " + tabName, LogLevel::INFO);
        // Status aktualisieren
        if (tabName.find("12V") != std::string::npos) {
            system_status_label_.set_text("💬 System: 12V Steuerung aktiv");
            refresh_relais12V_status();  // <-- sofortiger Refresh
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
        if (tabName.find("230V") != std::string::npos) {
            system_status_label_.set_text("💬 System: 230V Steuerung aktiv");
            refresh_relais230V_status();  // <-- sofortiger Refresh
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
        if (tabName.find("Energiebox") != std::string::npos) {
            system_status_label_.set_text("💬 System: Monitoring aktiv");
            if (!energiebox_timer_connection_.connected()) {
                energiebox_timer_connection_ = Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GUI::update_energiebox_tab), 60);
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
        for (auto &[nr, btn] : relais12v_buttons_) {
            bool isAktiv = ini12v.get("Relais " + std::to_string(nr) + "/eltakoState", "0") == "1";
            std::string neuerName = ini12v.get("Relais " + std::to_string(nr) + "/name", "Relais " + std::to_string(nr));
            if (isAktiv != relais12v_status_[nr]) {
                relais12v_status_[nr] = isAktiv;
                btn->get_style_context()->remove_class(isAktiv ? "relais-inactive" : "relais-active");
                btn->get_style_context()->add_class(isAktiv ? "relais-active" : "relais-inactive");
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
        for (auto &[nr, btn] : relais230v_buttons_) {
            bool isAktiv = ini230v.get("Relais " + std::to_string(nr) + "/eltakoState", "0") == "1";
            std::string neuerName = ini230v.get("Relais " + std::to_string(nr) + "/name", "Relais " + std::to_string(nr));
            if (isAktiv != relais230v_status_[nr]) {
                relais230v_status_[nr] = isAktiv;
                btn->get_style_context()->remove_class(isAktiv ? "relais-inactive" : "relais-active");
                btn->get_style_context()->add_class(isAktiv ? "relais-active" : "relais-inactive");
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
            if (notebook_.get_current_page() != 0) {  // Wenn nicht schon Info-Tab
                notebook_.set_current_page(0);
                debugPrint("180 Sekunden Inaktivität – wechsle zum Info-Tab", LogLevel::INFO);
            }
        }
        return true;  // Timer wiederholen
    }

    // Hilfsfunktion: Erzeugt eine Tabelle (Gtk::Grid) mit Beschriftungen und Werten, rechtsbündig
    Gtk::Grid *create_data_table(const std::vector<std::pair<std::string, std::string>> &data, const std::string &headline) {
        Gtk::Grid *grid = Gtk::manage(new Gtk::Grid());
        grid->set_row_spacing(8);
        grid->set_column_spacing(50);
        grid->get_style_context()->add_class("data-table");
        // Überschrift als fettes Label (links ausgerichtet, größer)
        auto *title = Gtk::manage(new Gtk::Label(headline));
        title->get_style_context()->add_class("data-title");
        title->set_halign(Gtk::ALIGN_START);
        title->set_margin_bottom(10);
        grid->attach(*title, 0, 0, 2, 1);
        std::string percent;
        std::string zeroValue;
        std::string battpercentStr;
        int battpercentInt = 0;
        int row = 1;
        for (const auto &[label, value] : data) {
            if (label == "SOC") {
                // Die SOC Ausgabe wird je nach Ladezustand Farblich ausgegeben. Ausserdem wird eine
                // Progressbar darunter angezeigt (2 colums!)
                auto *label_widget = Gtk::manage(new Gtk::Label(label + " (%)"));
                label_widget->get_style_context()->add_class("data-label");
                label_widget->set_halign(Gtk::ALIGN_START);
                auto *value_widget = Gtk::manage(new Gtk::Label(value));
                value_widget->get_style_context()->add_class("data-value");
                // Farbe für Prozentangabe SOC
                battpercentStr = value;
                battpercentStr.pop_back();
                battpercentInt = std::stoi(battpercentStr);
                if (battpercentInt >= 0 && battpercentInt <= 25) {
                    value_widget->get_style_context()->add_class("data-value-red");
                } else if (battpercentInt > 25 && battpercentInt <= 60) {
                    value_widget->get_style_context()->add_class("data-value-orange");
                } else if (battpercentInt > 60 && battpercentInt <= 100) {
                    value_widget->get_style_context()->add_class("data-value-green");
                }
                value_widget->set_halign(Gtk::ALIGN_END);
                grid->attach(*label_widget, 0, row, 1, 1);
                grid->attach(*value_widget, 1, row, 1, 1);
                // darunter progressbar anzeigen von batterie ladezustand. gradiant farbverlauf
                // verwenden
                percent = value;
                percent.pop_back();
                float soc = std::stof(percent);
                // SOC-ProgressBar erstellen
                auto *soc_bar = Gtk::manage(new Gtk::ProgressBar());
                soc_bar->set_show_text(false);
                soc_bar->set_valign(Gtk::ALIGN_CENTER);
                soc_bar->set_hexpand(true);
                soc_bar->set_margin_top(0);
                soc_bar->set_margin_bottom(0);
                soc_bar->get_style_context()->add_class("soc-bar");
                if (battpercentInt >= 0 && battpercentInt <= 25) {
                    soc_bar->get_style_context()->add_class("soc-bar-low");
                } else if (battpercentInt > 25 && battpercentInt <= 55) {
                    soc_bar->get_style_context()->add_class("soc-bar-middle");
                } else if (battpercentInt > 55 && battpercentInt <= 100) {
                    soc_bar->get_style_context()->add_class("soc-bar-high");
                }
                soc_bar->set_fraction(soc / 100.0f);
                // progressbar in 2 spalten laden
                grid->attach(*soc_bar, 0, ++row, 2, 1);
            } else {
                // normale ausgabe
                auto *label_widget = Gtk::manage(new Gtk::Label(label));
                label_widget->get_style_context()->add_class("data-label");
                label_widget->set_halign(Gtk::ALIGN_START);
                label_widget->set_hexpand(true);
                auto *value_widget = Gtk::manage(new Gtk::Label(value));
                value_widget->get_style_context()->add_class("data-value");
                // Rote Farbe bestimmen für Werte die derzeit auf 0 stehen
                zeroValue = value;
                zeroValue.pop_back();
                if (zeroValue == "0" || value == "0kWh") {
                    value_widget->get_style_context()->remove_class("data-value-green");
                    value_widget->get_style_context()->add_class("data-value-red");
                } else {
                    value_widget->get_style_context()->remove_class("data-value-red");
                    value_widget->get_style_context()->add_class("data-value-green");
                }
                value_widget->set_halign(Gtk::ALIGN_END);
                grid->attach(*label_widget, 0, row, 1, 1);
                grid->attach(*value_widget, 1, row, 1, 1);
                ++row;
            }
        }
        return grid;
    }

    // Liest messwerte aus der Datenbank aus und parst die Werte und aktualisiert den Energietab
    bool update_energiebox_tab() {
        MySQLiWrapper db("/home/box/.mysql_energiebox.cfg");
        if (db.query("SELECT pv_volt, pv_ampere, pv_power, batt_volt, batt_ampere, batt_power, batt_soc, generated_power FROM messwerte ORDER BY id DESC LIMIT 1")) {
            std::map<std::string, std::string> row = db.fetchArray();
            // PV Daten
            std::vector<std::pair<std::string, std::string>> pv_data = {{"Spannung (U)", row["pv_volt"] + "V"},
                                                                        {"Ampere (I)", row["pv_ampere"] + "A"},
                                                                        {"Leistung (P)", row["pv_power"] + "W"},
                                                                        {"Tagesertrag (P)", row["generated_power"] + "kWh"}};
            // Batterie Daten
            std::vector<std::pair<std::string, std::string>> battery_data = {{"Spannung (U)", row["batt_volt"] + "V"},
                                                                             {"Ampere (I)", row["batt_ampere"] + "A"},
                                                                             {"Leistung (P)", row["batt_power"] + "W"},
                                                                             {"SOC", row["batt_soc"] + "%"}};
            // Bestehende GUI-Widgets entfernen
            auto children = energiebox_data_container_->get_children();
            for (auto *child : children) {
                energiebox_data_container_->remove(*child);
            }
            // Neue Tabellen erstellen
            auto *pv_table = create_data_table(pv_data, "🌞 PV MPPT Status");
            auto *battery_table = create_data_table(battery_data, "🔋 Batterie Ladestatus");
            pv_table->set_margin_start(0);
            pv_table->set_margin_end(20);
            battery_table->set_margin_start(20);
            battery_table->set_margin_end(0);
            energiebox_data_container_->pack_start(*pv_table, Gtk::PACK_EXPAND_WIDGET);
            energiebox_data_container_->pack_start(*battery_table, Gtk::PACK_EXPAND_WIDGET);
            energiebox_data_container_->show_all();
        }
        return true;
    }
    Gtk::Notebook notebook_;                         // Tab-Widget
    Gtk::Box *energiebox_data_container_ = nullptr;  // Container für Energiebox-Daten
    time_t last_interaction_time_;                   // Zeitstempel letzter Nutzer-Interaktion
    std::map<int, Gtk::Button *> relais12v_buttons_;
    std::map<int, bool> relais12v_status_;
    std::map<int, Gtk::Button *> relais230v_buttons_;
    std::map<int, bool> relais230v_status_;
};

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "de.cplusplus-development.gui");
    bool window_mode = false;
    bool debug_mode = false;
    // Registriert Start Parameter + Hilfe
    Glib::OptionEntry entry_debug;
    entry_debug.set_long_name("debug");
    entry_debug.set_short_name('d');
    entry_debug.set_description("Aktiviere Debug-Modus für Ausgabe auf Konsole");

    Glib::OptionEntry entry_window;
    entry_window.set_long_name("window");
    entry_window.set_short_name('w');
    entry_window.set_description("Starte im Fenster-Modus (nicht Fullscreen)");

    Glib::OptionGroup option_group("main", "Hauptoptionen");
    option_group.add_entry(entry_debug, debug_mode);
    option_group.add_entry(entry_window, window_mode);

    Glib::OptionContext context;
    context.set_summary("Grafisches User Interface Programm der Energiebox.\nDient zur Anzeige und Steuerung der Relais- Schaltungen\ndie an 3 Portexpender angeschlossen sind.");
    context.set_main_group(option_group);
    try {
        context.parse(argc, argv);
    } catch (const Glib::Error &ex) {
        std::cerr << "Fehler beim Parsen der Optionen: " << ex.what() << std::endl;
        return 1;
    }
    debug = debug_mode;
    debugPrint("Programm gestartet", LogLevel::INFO);
    GUI window;
    if (window_mode) {
        window.set_default_size(800, 480);
    } else {
        window.fullscreen();
    }
    return app->run(window);
}
