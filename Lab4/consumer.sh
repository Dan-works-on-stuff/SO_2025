#!/bin/bash

read -a nums

if [ ${#nums[@]} -eq 0 ]; then
    echo "Consumer: computed sum is NULL."
    exit 1
else
    sum=0
    for num in "${nums[@]}"; do
        cube=$(( num * num * num ))
        sum=$(( sum + cube ))
    done
    echo "Consumer: computed sum is $sum."
    exit 0
fi