#!/bin/bash

DRY_RUN=false
if [[ "$1" == "--dry-run" ]]; then
    DRY_RUN=true
    echo "💡 Dry-Run aktiviert – es werden keine Änderungen vorgenommen."
fi

# Nur mit Root-Rechten ausführbar
if [[ $EUID -ne 0 ]]; then
    echo "❌ Dieses Skript muss mit sudo ausgeführt werden."
    exit 1
fi

# Prüfen, ob mit sudo aufgerufen wurde
if [ -z "$SUDO_USER" ]; then
    echo "❌ Bitte das Skript mit sudo aufrufen, nicht direkt als root."
    exit 1
fi

# Home-Verzeichnis des aufrufenden Users ermitteln
USER_HOME=$(eval echo "~$SUDO_USER")
CONFIG_FILE="$USER_HOME/.mysql_energiebox.cfg"
# Datenbank User Name
ENERGIEBOX_USER="energiebox"

echo "ℹ️ MySQL-Konfigurationsdatei wird in $CONFIG_FILE gespeichert"

# Prüfen, ob mysql CLI vorhanden ist
if ! which mysql >/dev/null 2>&1; then
    echo "❌ Der MySQL-Client 'mysql' ist nicht installiert."
    read -p "📦 Möchtest du ihn jetzt mit apt installieren? [j/N] " install_mysql
    if [[ "$install_mysql" =~ ^[Jj]$ ]]; then
        apt update && apt install -y mysql-client || { echo "❌ Installation fehlgeschlagen."; exit 1; }
    else
        echo "⚠️ Abgebrochen. Bitte installiere den MySQL-Client manuell mit 'sudo apt install mysql-client'"
        exit 1
    fi
fi

# Prüfen, ob mysqladmin vorhanden ist
if ! which mysqladmin >/dev/null 2>&1; then
    echo "❌ 'mysqladmin' ist nicht installiert."
    read -p "📦 Möchtest du es jetzt mit apt installieren? [j/N] " install_admin
    if [[ "$install_admin" =~ ^[Jj]$ ]]; then
        apt update && apt install -y mysql-client || { echo "❌ Installation fehlgeschlagen."; exit 1; }
    else
        echo "⚠️ Abgebrochen. Bitte installiere mysqladmin mit 'sudo apt install mysql-client'"
        exit 1
    fi
fi

# Eingabe: Root-Passwort für MySQL
read -p "🔐 Bitte gib das MySQL-Root-Passwort ein: " -s MYSQL_ROOT_PW
echo

# Prüfen, ob MySQL-Server erreichbar ist
echo "🔍 Überprüfe Verbindung zum MySQL-Server..."
if ! mysqladmin ping -u root -p"$MYSQL_ROOT_PW" --silent; then
    echo "❌ Fehler: MySQL-Server ist nicht erreichbar oder das Root-Passwort ist falsch."
    exit 1
fi

# Eingabe: Passwort für neuen Benutzer
read -p "🔐 Bitte gib das gewünschte Passwort für den neuen Benutzer 'energiebox' ein: " -s ENERGIEBOX_PW
echo

# Dry-Run-Modus
if [ "$DRY_RUN" = true ]; then
    echo "=== Geplante Aktionen (Dry-Run) ==="
    echo "  - Datenbank 'energiebox' löschen (falls vorhanden)"
    echo "  - Benutzer '$ENERGIEBOX_USER' löschen (falls vorhanden)"
    echo "  - Benutzer '$ENERGIEBOX_USER' mit Passwort neu anlegen"
    echo "  - Datenbank 'energiebox' neu anlegen"
    echo "  - Zugriffsrechte für Benutzer '$ENERGIEBOX_USER' auf Datenbank gewähren"
    echo "  - Tabelle 'messwerte' mit Spalten anlegen"
    echo "  - Konfigurationsdatei '$CONFIG_FILE' erzeugen mit Berechtigung 640 und Besitzer root:energiebox"
    echo "==============================="
    echo "Kein Datenbankbefehl wurde ausgeführt."
    exit 0
fi

# MySQL-Befehle ausführen
echo "⚙️ Erstelle Datenbank, Benutzer und Tabelle..."

mysql -u root -p"$MYSQL_ROOT_PW" <<EOF
DROP DATABASE IF EXISTS energiebox;
DROP USER IF EXISTS '$ENERGIEBOX_USER'@'localhost';
CREATE USER '$ENERGIEBOX_USER'@'localhost' IDENTIFIED BY '$ENERGIEBOX_PW';
CREATE DATABASE energiebox;
GRANT ALL PRIVILEGES ON energiebox.* TO '$ENERGIEBOX_USER'@'localhost';
FLUSH PRIVILEGES;
USE energiebox;
CREATE TABLE messwerte (
 id int(10) unsigned NOT NULL AUTO_INCREMENT,
 timestamp int(10) unsigned NOT NULL,
 pv_volt float unsigned DEFAULT NULL,
 pv_ampere float unsigned DEFAULT NULL,
 pv_power float unsigned DEFAULT NULL,
 batt_volt float unsigned DEFAULT NULL,
 batt_ampere float unsigned DEFAULT NULL,
 batt_power float unsigned DEFAULT NULL,
 batt_soc int(10) unsigned DEFAULT NULL,
 generated_power float unsigned DEFAULT NULL,
 grid_load_active int(11) DEFAULT NULL,
 PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=5395 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci
EOF

# Konfigurationsdatei schreiben
echo "📁 Erstelle Konfigurationsdatei: $CONFIG_FILE"
cat <<EOC > "$CONFIG_FILE"
[client]
user=$ENERGIEBOX_USER
password=$ENERGIEBOX_PW
host=localhost
database=energiebox
port=3306
EOC

chown root:energiebox "$CONFIG_FILE"
chmod 640 "$CONFIG_FILE"

echo "✅ Datenbank-Setup abgeschlossen."
echo "🔑 Zugang über: $CONFIG_FILE (Besitzer: root, Gruppe: energiebox, Modus: 640)"
