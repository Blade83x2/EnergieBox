/*
 * JARVIS Accoustic Listener Adapter
 * Project: Energiebox
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
 * + piper & whisper.cpp konfiguration dokumentieren
 * + model und vadmodel in beschreibung download
 *

 *
 */
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <limits.h>

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

// Gibt den aktuellen absoluten Pfad vom Projekt zurück
std::string getProjectPath() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        std::string path(buffer);
        // Letzten 7 Zeichen (/jarvis) entfernen
        if (path.length() >= 7) {
            path.erase(path.length() - 7);
        }
        return path;
    }
    return "";
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

// Verbindet zu RemoteServer und setzt Befehl ab
void remoteCommand(const std::string& host, const std::string& cmd) {
    std::string sshHost = host;
    std::string port = "22";  // Standardport
    // Prüfen ob ":PORT" vorhanden ist
    size_t pos = host.rfind(':');
    if (pos != std::string::npos) {
        std::string possiblePort = host.substr(pos + 1);
        // Prüfen ob hinter ":" nur Zahlen stehen
        bool isNumber = !possiblePort.empty() && std::all_of(possiblePort.begin(), possiblePort.end(), ::isdigit);
        if (isNumber) {
            port = possiblePort;
            sshHost = host.substr(0, pos);
        }
    }
    std::string full = "ssh -p " + port + " " + sshHost + " \"" + cmd + "\"";
    int result = std::system(full.c_str());
    if (result != 0) {
        std::cerr << "\n[Jarvis] remoteCommand fehlgeschlagen: " << full << std::endl;
    }
    (void)result;  // verhindert warning
}

void localCommand(const std::string& cmd) {
    int result = std::system(cmd.c_str());
    if (result != 0) {
        std::cerr << "\n[Jarvis] localCommand fehlgeschlagen: " << cmd << std::endl;
    }
    (void)result;  // verhindert warning (optional)
}

// TextToSpeech Funktion für Programmstatus
void speak(const std::string& text) {
    std::string projectPath = getProjectPath();
    std::string piperPath = projectPath + "/Jarvis/piper";
    std::string pipermodel = piperPath + "/" + config["piper_model"];
    std::string cmd = "echo \"" + text + "\" | " + piperPath + "/piper --quiet --noise_w 0.8 --length_scale 0.9 --model " + pipermodel +
                      " --output-raw | aplay -f S16_LE -r 22050 -c 1 2>/dev/null";
    int result = std::system(cmd.c_str());
    (void)result;  // verhindert warning (optional)
}

// Übersetzt gesprochenes in Text
std::string transcribe(const std::string& audiofile) {
    std::string model = config["model"];
    std::string vadmodel = config["vadmodel"];
    std::string projectPath = getProjectPath();
    std::string whisper = projectPath + "/whisper.cpp";
    std::string cmd = whisper + "/build/bin/whisper-cli --vad --vad-model " + whisper + vadmodel + " --language de --threads 4 --model " + whisper + model + " --file " +
                      audiofile + " -otxt -of result 2>/dev/null";

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

// Tastet Microphone jede x Sekunden ab und vergleicht auf Wakeword
void waitForWakeword() {
    std::string wakeword = config.count("wakeword") ? config["wakeword"] : "jarvis";
    std::cout << "\n[Jarvis] Warte auf Wakeword: " << wakeword << "\n";
    do {
        // TODO ausgabe pipen in transcribe()

        //   arecord -D " + mic + "  -f S16_LE -c 1 -r 16000 -d 5 |  ./whisper-cli -m models/ggml-base.bin -f - --no-timestamps

        recordAudio("wake.wav");
        std::string text = transcribe("wake.wav");

        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        if (text.find(wakeword) != std::string::npos) {
            std::cout << "\n[Jarvis] Wakeword erkannt! Warte auf Befehle"
                      << "\n";
            speak("               ");
            speak("   Dann lass mal hören!");
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
    engine = new RuleEngine(rules, config);
    speak("[Jarvis] Programm wurde gestartet!");

    waitForWakeword();
    speak("     ");
    speak("[Jarvis] Programm wurde beendet!");
    return 0;
}
