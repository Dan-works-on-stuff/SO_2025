#!/bin/bash

# Verifică dacă a fost primit exact un argument
if [ $# -ne 1 ]; then
    echo "Utilizare: $0 <argument>" >&2
    exit 1
fi

# Calea către scriptul curent (main.sh)
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Calea către subdirectorul și scriptul de verificat
PROCESATOR_DIR="$SCRIPT_DIR/xprocesator"
PROCESATOR_SCRIPT="$PROCESATOR_DIR/procesator.sh"

# Verificare existență director și script + permisiune execuție
if [ ! -d "$PROCESATOR_DIR" ]; then
    echo "Eroare: Directorul 'xprocesator' nu există în directorul scriptului." >&2
    exit 1
fi

if [ ! -f "$PROCESATOR_SCRIPT" ]; then
    echo "Eroare: Scriptul 'procesator.sh' nu există în 'xprocesator'." >&2
    exit 1
fi

if [ ! -x "$PROCESATOR_SCRIPT" ]; then
    echo "Eroare: Scriptul 'procesator.sh' nu are permisiunea de execuție." >&2
    exit 1
fi


# Calea către subdirectorul și scriptul de verificat
PROCESATOR2_DIR="$SCRIPT_DIR/validators"
PROCESATOR2_SCRIPT="$PROCESATOR2_DIR/validatorASCII.c"

# Verificare existență director și script + permisiune execuție
if [ ! -d "$PROCESATOR2_DIR" ]; then
    echo "Eroare: Directorul 'validators' nu există în directorul scriptului." >&2
    exit 2
fi

if [ ! -f "$PROCESATOR2_SCRIPT" ]; then
    echo "Eroare: Scriptul 'validatorASCII.c' nu există în 'validators'." >&2
    exit 2
fi

if [ ! -r "$PROCESATOR2_SCRIPT" ]; then
    echo "Eroare: Scriptul 'validatorASCII.c' nu are permisiunea de citire." >&2
    exit 2
fi

TARGET_DIR="$1"

# Verifică dacă argumentul este un director accesibil (citire și execuție)
if [ ! -d "$TARGET_DIR" ] || [ ! -r "$TARGET_DIR" ] ; then
    echo "Eroare: '$TARGET_DIR' nu este un director sau nu are permisiuni de citire." >&2
    exit 3
fi
./procesator.sh $0
echo $?
# Dacă a trecut toate verificările, scriptul poate continua (de exemplu, apelarea procesator.sh)
# "$PROCESATOR_SCRIPT" "$1"
