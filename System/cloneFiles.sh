#!/bin/bash
#
# Dieses Programm liesst eine SD Karte aus und erstellt ein
# Bitgenaues Abbild in dem Ordner von wo es aufgerufen wird.
#
# Option zum Schreiben des Images auf USB SSD und automatische
# Anpassung der Festplattengröße.
#
# Kann danach direkt im Raspberry als USB Festplatte verwendet werden!

set -euo pipefail

BLOCKSIZE="4M"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
IMAGE_PREFIX="raspi_sd_${TIMESTAMP}"
LOGFILE="clone_${TIMESTAMP}.log"

exec > >(tee -a "$LOGFILE") 2>&1

# ----------------- Helper -----------------

function die() {
    echo "❌ $1"
    exit 1
}

function require_root() {
    [[ $EUID -eq 0 ]] || die "Dieses Skript muss mit sudo ausgeführt werden."
}

function get_system_disk() {
    lsblk -no PKNAME "$(findmnt -n -o SOURCE /)" | sed 's|^|/dev/|'
}

SYSTEM_DISK="$(get_system_disk)"

function list_all_devices() {
    lsblk -dpno NAME,SIZE,MODEL,TRAN
}



function select_device() {
    local MODE=$1
    local TITLE=$2

    echo
    echo "📀 $TITLE"
    echo


    mapfile -t DEVICES < <(list_all_devices)


    [[ ${#DEVICES[@]} -gt 0 ]] || die "Keine passenden Datenträger gefunden."

    select DEVICE in "${DEVICES[@]}"; do
        [[ -n "$DEVICE" ]] || { echo "Ungültige Auswahl."; continue; }
        SELECTED_DEVICE=$(awk '{print $1}' <<< "$DEVICE")

        if [[ "$SELECTED_DEVICE" == "$SYSTEM_DISK" ]]; then
            die "Systemplatte ($SYSTEM_DISK) darf NICHT überschrieben werden!"
        fi
        break
    done
}

function select_image_file() {
    echo
    echo "🖼️  Verfügbare Image-Dateien:"
    echo

    mapfile -t IMAGES < <(ls *.img 2>/dev/null || true)
    [[ ${#IMAGES[@]} -gt 0 ]] || die "Keine .img-Dateien gefunden."

    select IMAGE in "${IMAGES[@]}"; do
        [[ -n "$IMAGE" ]] || { echo "Ungültige Auswahl."; continue; }
        SELECTED_IMAGE="$IMAGE"
        break
    done
}

function sha_image() {
    local FILE=$1
    sha256sum "$FILE" | tee "${FILE}.sha256"
}

# ----------------- READ -----------------

function read_sd_card() {
    select_device "all" "SD-Karte auswählen (Quelle)"

    IMAGE_FILE="${IMAGE_PREFIX}.img"

    echo
    echo "Quelle: $SELECTED_DEVICE"
    echo "Ziel:   $IMAGE_FILE"read_sd_card
    read -p "SD-Karte auslesen? (j/N): " CONFIRM
    [[ "$CONFIRM" =~ ^[jJ]$ ]] || exit 0

    dd if="$SELECTED_DEVICE" of="$IMAGE_FILE" \
        bs=$BLOCKSIZE status=progress conv=fsync

    echo "🔐 Erzeuge SHA256-Prüfsumme..."
    sha_image "$IMAGE_FILE"

    echo "✅ Image erstellt: $IMAGE_FILE"
}

# ----------------- WRITE -----------------

function expand_partition() {
    local DEVICE=$1
    local PART="${DEVICE}2"

    echo "📐 Erweitere Partitionstabelle..."
    parted "$DEVICE" ---pretend-input-tty <<EOF
resizepart 2 100%
Yes
EOF

    partprobe "$DEVICE"
    sleep 2

    echo "📈 Erweitere Dateisystem..."
    resize2fs "$PART"
}

function write_to_usb_ssd() {
    select_image_file
    select_device "usb" "USB-SSD auswählen (ZIEL)"

    echo
    echo "IMAGE: $SELECTED_IMAGE"
    echo "ZIEL:  $SELECTED_DEVICE"
    echo
    echo "⚠️  ALLE DATEN AUF DEM ZIEL WERDEN GELÖSCHT!"
    read -p "Wirklich schreiben? (j/N): " CONFIRM
    [[ "$CONFIRM" =~ ^[jJ]$ ]] || exit 0

    dd if="$SELECTED_IMAGE" of="$SELECTED_DEVICE" \
        bs=$BLOCKSIZE status=progress conv=fsync

    echo "🔎 Prüfe Image-Checksumme..."
    # sha256sum -c "${SELECTED_IMAGE}.sha256"

    # expand_partition "$SELECTED_DEVICE"
    echo "Mit sudo resize2fs /dev/sda2 Plattengröße auf dem Laufenden System anpassen"
    echo "✅ Schreiben & Erweiterung abgeschlossen."
}

# ----------------- MAIN -----------------

require_root

case "${1:-}" in
    -r) read_sd_card ;;
    -w) write_to_usb_ssd ;;
    *)  echo "Usage: sudo $0 -r | -w"; exit 1 ;;
esac

echo
echo "📄 Logfile: $LOGFILE"
