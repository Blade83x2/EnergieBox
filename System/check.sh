#!/bin/bash -e
# Aufruf: ./check.sh
# Prüft ob alles mit der Spannungsversorgung in Ordnung ist

   #      Bit   Meaning
   #      ────  ────────────────────────────────────
   #       0    Under-voltage detected
   #       1    Arm frequency capped
   #       2    Currently throttled
   #       3    Soft temperature limit active
   #      16    Under-voltage has occurred
   #      17    Arm frequency capping has occurred
   #      18    Throttling has occurred
   #      19    Soft temperature limit has occurred
   #      A value of zero indicates that none of the above conditions is true.

A=( [ 0]="Unterspannungswarnung am Controller"
    [ 1]="Arm Frequenz begrenzt"
    [ 2]="Derzeit gedrosselt"
    [ 3]="Zulässige Temperaturgrenze erreicht"
    [16]="Unterspannung am Controller"
    [17]="Arm Frequenz wurde begrenzt"
    [18]="Drossel wurde aktiviert"
    [19]="Drosselung wegen Temperaturgrenze aktiviert" )

for ((i=0; i<32; i++)); do
    if [[ ! ${A[i]} ]]; then
        A[i]="set, but meaning is unknown. See vcgencmd(1)."
    fi
done
eval $(vcgencmd get_throttled)
if [[ "$1" ]]; then throttled="$1"; fi
if (( throttled == 0 )); then
    echo " "
elif (( (throttled & 0xFF) == 0 )); then
    echo "Es sind Probleme aufgetreten, die derzeit jedoch nicht auftreten."
fi
for ((i=0; i<32; i++)); do
    if (( throttled & 2**i )); then
        echo "Bit $i: ${A[$i]}"
    fi
done
exit $(( throttled & 0xFF ))
