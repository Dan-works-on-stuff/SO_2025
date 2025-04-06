#!/bin/bash

# Check if there are command-line arguments for numbers
if [ $# -ge 1 ]; then
    numbers=("$@")
else
    # Read numbers from stdin
    read -a numbers
fi

# Read p from stdin
read p

# Filter numbers <= p
filtered=()
for num in "${numbers[@]}"; do
    if (( num <= p )); then
        filtered+=("$num")
    fi
done

# Output the filtered numbers separated by spaces
echo "${filtered[@]}"