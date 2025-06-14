#!/bin/bash
relais="$1"
state="$2"
config="/Energiebox/230V/config.ini"
if ! [[ "$relais" =~ ^([1-9]|1[0-6])$ ]]; then
  echo "Ungültige Relaisnummer: $relais (erlaubt 1-16)"
  exit 1
fi
if ! [[ "$state" =~ ^[01]$ ]]; then
  echo "Ungültiger Zustand: $state (erlaubt 0 oder 1)"
  exit 1
fi
sed -i "/^\[Relais $relais]$/,/^\[/ s/^eltakoState = [01]/eltakoState = $state/" "$config"
