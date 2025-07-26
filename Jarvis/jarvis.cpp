// jarvis_main.cpp – Alltagstauglicher Sprachassistent mit Whisper.cpp und Bash

#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <csignal>
#include <algorithm> // <--- Wichtig für std::transform
#include <map>

bool runLoop = true;
std::map<std::string, std::string> config;

void signalHandler(int signum) {
    runLoop = false;
    std::cout << "\n[Jarvis] Beende Sprachassistent...\n";
}

void loadConfig(const std::string& configPath) {
    std::ifstream infile(configPath);
    if (!infile) {
        std::cerr << "[Fehler] Konnte Konfigurationsdatei nicht laden: " << configPath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            config[key] = value;
        }
    }
}

void speak(const std::string& text) {
    std::string tts_cmd = config.count("tts") ? config["tts"] : "espeak-ng \"" + text + "\" 2>/dev/null";
    if (config.count("tts")) {
        std::string command = config["tts"];
        size_t pos = command.find("%s");
        if (pos != std::string::npos) {
            command.replace(pos, 2, text);
        }
        system(command.c_str());
    } else {
        system(tts_cmd.c_str());
    }
}

std::string transcribeWhisper(const std::string& audioPath) {
    std::string model = config.count("model") ? config["model"] : "models/ggml-base.en.bin";
    std::string command = "./main -m " + model + " -f " + audioPath + " -otxt -of result";
    system(command.c_str());
    std::ifstream inFile("result.txt");
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}

void runCommand(const std::string& cmd) {
    std::cout << "[Befehl] " << cmd << std::endl;
    system(cmd.c_str());
}

void parseCommand(const std::string& input) {
    if (input.find("licht") != std::string::npos && input.find("an") != std::string::npos) {
        speak("Ich schalte das Licht ein.");
        runCommand("bash /Energiebox/relais.sh licht_an");
    } else if (input.find("licht") != std::string::npos && input.find("aus") != std::string::npos) {
        speak("Ich schalte das Licht aus.");
        runCommand("230V 15 1");
    } else {
        speak("Das habe ich leider nicht verstanden.");
    }
}

void recordAudio(const std::string& filePath) {
    std::string mic = config.count("mic") ? config["mic"] : "plughw:1,0";
    std::string command = "arecord -D " + mic + " -f S16_LE -r 16000 -c 1 -d 4 " + filePath + " 2>/dev/null";
    system(command.c_str());
}

void waitForWakeword() {
    std::string wakeword = config.count("wakeword") ? config["wakeword"] : "jarvis";
    std::cout << "[Jarvis] Warte auf Wakeword '" << wakeword << "'...\n";
    while (runLoop) {
        recordAudio("wakeword.wav");
        std::string wakeText = transcribeWhisper("wakeword.wav");
        std::transform(wakeText.begin(), wakeText.end(), wakeText.begin(), ::tolower);
        if (wakeText.find(wakeword) != std::string::npos) {
            speak("Ja? Ich bin bereit.");
            recordAudio("command.wav");
            std::string commandText = transcribeWhisper("command.wav");
            parseCommand(commandText);
        }
    }
}

int main() {
    signal(SIGINT, signalHandler);
    loadConfig("jarvis.config");
    speak("Starte Jarvis Sprachassistent.");
    waitForWakeword();
    speak("Jarvis wurde beendet.");
    return 0;
}
