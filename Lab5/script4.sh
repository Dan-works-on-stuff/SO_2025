#!/bin/bash
### Afișarea comentariilor din script-uri. ###

# Validarea datelor de intrare

if [ $# -eq 0 ]
then
    read -p "Dați directorul de start:" dir
    if [ -z "$dir" ]
    then
        dir="."
    fi  # Dacă utilizatorul introduce cuvântul vid la read (i.e., apasă direct ENTER), vom procesa directorul curent de lucru!
else
    dir=$1
fi

if [ ! -d $dir ] || [ ! -r $dir ] #-d = director , -r = readable
then
    echo "Argumentul dat, $dir, nu este director, sau nu aveti drept de citire a lui!"
    exit 1  # Fail!
fi

# Parcurgere recursivă implicită, obținută aici cu ajutorul comenzii find
lista=$(find $dir -type f -name "*.sh")  # -type f iseamna file

for fis in $lista
do
    echo $fis
    grep -n '#' $fis | grep -v '$#' | grep -v '#!' # | (pipe) este modul de a redirectiona outputul comenzii din stanga ca input pt comanda din dreapta
done

exit 0  # Succes!
