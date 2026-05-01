/*
 * JARVIS Accoustic Listener Adapter
 * Vendor: Johannes Krämer
 * Builddate: 01.05.2026
 * 
 * Beschreibung: Dieses Konsolenprogramm dient zur Akustischen Kommunikation
 * mit der Energiebox. Dabei werden für alle angeschlossenen Geräte hinter 
 * den Relais Synonyme definiert die bei der Spracherkennung berücksichtigt werden.
 * Ein Gerät welches als "TV" eingetragen ist, bekommt Synonyme wie z.B. "Fernseher"
 * oder "Glotze". Diese Aussprache wird dann mit weiteren Worten wie "an", 
 * "einschalten", "aktivieren", "anmachen" usw. verglichen und somit lässt sich 
 * ein Schaltplan erstellen.
 * 
 * 
 * TODO
 * + *.wav löschen beim beenden
 * + areccord Return string pipen in whisper (keine physikalische Datei schreiben)
 * + setup für wakeword, remote SSH data OR localrun, microphone input select
 * + yaml zuende bauen 230V & 12V
 * + piper & whisper.cpp konfiguration dokumentieren
 * 
 * 
*/
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <csignal>
#include <algorithm>
#include <map>
#include "SimpleYAMLParser.h"
#include "RuleEngine.h"

RuleEngine* engine;
bool runLoop = true;
std::map<std::string, std::string> config;

// Beendet Progrtamm bei SIGINT
void signalHandler(int) {
    runLoop = false;
    std::cout << "\n[Jarvis]  beende Programm...\n";
}

// Konfigurationsdatei auslesen
void loadConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "\n[Jarvis] Config Datei \"jarvis.config\" kann nicht geladen werden! " << path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        config[key] = value;
    }
}

// Führt Befehl lokal aus
int exec(const std::string& cmd) {
    return std::system(cmd.c_str());
}

// TextToSpeech Funktion für Programmstatus
void speak(const std::string& text) {
    std::string model = config.count("piper_model") ? config["piper_model"] : "de_DE-thorsten-medium.onnx";
    std::string piperPath = config.count("piperpath") ? config["piperpath"] + "/piper" : "./piper";
    std::string cmd = "echo \"" + text + "\" | " + piperPath + " --quiet --noise_w 0.8 --length_scale 0.9 --model " + model + " --output-raw | aplay -f S16_LE -r 22050 -c 1 2>/dev/null";
    int result = std::system(cmd.c_str());
    (void)result; // verhindert warning (optional)
}

// Übersetzt gesprochenes in Text
std::string transcribe(const std::string& audio) {
    std::string model = config["model"];
    std::string whisper = config["whisperpath"];
    std::string cmd = whisper + "/whisper-cli -l de -m " + model + " -f " + audio + " -otxt -of result 2>/dev/null";
    exec(cmd);
    std::ifstream file("result.txt");
    if (!file) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Methode zum Aufzeichnen des Streams vom Gerät config["mic"]
void recordAudio(const std::string& file) {
    std::string mic = config.count("mic") ? config["mic"] : "default";
    std::string duration = config.count("record_duration") ? config["record_duration"] : "4";
    std::string cmd = "arecord -D " + mic + " -f S16_LE -r 16000 -c 1 -d " + duration + " " + file + " 2>/dev/null";
    exec(cmd);
}

// Verbindet zu RemoteServer und setzt Befehl ab
void remoteCommand(const std::string& host, const std::string& cmd) {
    std::string full = "ssh " + host + " \"" + cmd + "\"";
    exec(full);
    return;
    int result = std::system(full.c_str());
    if (result != 0) {
        std::cerr << "\n[Jarvis] remoteCommand fehlgeschlagen: " << full << std::endl;
    }
}

// Tastet Microphone jede x Sekunden ab und vergleicht auf Wakeword
void waitForWakeword() {
    std::string wakeword = config.count("wakeword") ? config["wakeword"] : "jarvis";
    std::cout << "\n[Jarvis] Warte auf Wakeword: " << wakeword << "\n";
    do {
        // TODO ausgabe pipen in transcribe()
        recordAudio("wake.wav");
        std::string text = transcribe("wake.wav");
        
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        if (text.find(wakeword) != std::string::npos) {
            std::cout << "\n[Jarvis] Wakeword erkannt! Warte auf Befehle" << "\n";
            recordAudio("cmd.wav");
            std::string cmd = transcribe("cmd.wav");
            // Whitespace & newline & tabs entfernen
            cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
            cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
            cmd.erase(std::remove(cmd.begin(), cmd.end(), '\t'), cmd.end());
            // String zu LOWERCASE machen
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
            engine->process(cmd);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (runLoop);
}

// Programmstart
int main() {
    signal(SIGINT, signalHandler);
    loadConfig("jarvis.config");
    SimpleYAMLParser parser;
    auto rules = parser.parse("energieboxCommands.yaml");
    engine = new RuleEngine(rules, config["ssh_remote_addr"]);
    speak("     ");
    speak("[Jarvis] Programm wurde gestartet!");
    waitForWakeword();
    speak("     ");
    speak("[Jarvis] Programm wurde beendet!");
    return 0;
}
