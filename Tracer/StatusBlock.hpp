#pragma once
#include <string>

class StatusBlock
{
  protected:
	float extractValue( const std::string &line );

  public:
	virtual bool update() = 0;
	virtual void draw() const = 0;

	// Datei lesen (nur einmal pro Schleife)
	static bool loadTraceFile( const std::string &filename );

	// Gemeinsamer Speicher für trace.txt Inhalt
	static std::string traceData;

	// Hilfsfunktionen für Ausgabe und Terminalsteuerung
	static void clearScreen();
	static void printHeader( const std::string &header );
	static void
	printFloat( float value, const char *unit, bool showPercent = false, bool showSpace = true );
	static void printBar( float value, float maxValue );

	// Breite für Labels (für Ausrichtung)
	static constexpr int WIDTH_LABEL = 18;
};
