#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Utilizare: $0 <argument>" >&2
    exit 2
fi
script_dir="$(dirname "$(realpath "$0")")"
PROCESATOR_SCRIPT="$(dirname "$script_dir")/validators/default.sh"
if [ ! -f "$PROCESATOR_SCRIPT" ]; then
    echo "Eroare: Scriptul 'default.sh' nu există în 'validators'." >&2
    exit 1
fi

if [ ! -x "$PROCESATOR_SCRIPT" ]; then
    echo "Eroare: Scriptul 'default.sh' nu are permisiunea de citire." >&2
    exit 1
fi

TARGET_DIR="$1"

# Verifică dacă argumentul este un director accesibil (citire și execuție)
if [ ! -d "$TARGET_DIR" ] || [ ! -r "$TARGET_DIR" ] || [ ! -x "$TARGET_DIR" ]; then
    echo "Eroare: '$TARGET_DIR' nu este un director sau nu are permisiuni de citire/executare." >&2
    exit 3
fi

# Parcurge toate intrările DIRECTE din director
for entry in "$TARGET_DIR"/*; do
    # Verifică dacă intrarea există (pentru a evita erori când nu există fișiere/directoare)
    [ -e "$entry" ] || continue

    if [ -d "$entry" ]; then
        # Apel recursiv pe subdirector
        "$0" "$entry"
    elif [ -f "$entry" ]; then
        # Apel către default.sh pentru fișiere, cu redirectare în results.txt (append)
        "$VALIDATOR_SCRIPT" "$entry" >> results.txt 2>> results.txt
    fi
done

