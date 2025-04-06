#!/bin/bash

compute_power() {
    local k=$1
    local current=$2
    local n=$3

    # afisare pe stderr
    echo "$current" >&2

    # ecursiv până la n
    if [ $k -lt $n ]; then
        compute_power $((k + 1)) $((current * 2)) "$n"
    fi
}

# Obține valoarea lui n din argument sau prin citire
if [ $# -ge 1 ]; then
    n=$1
else
    read n
fi

# Apel recursiv
compute_power 0 1 "$n"