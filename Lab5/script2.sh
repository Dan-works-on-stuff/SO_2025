#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Eroare: parametri insuficienti! Nu ati specificat numele directorului."
    exit 1  # Fail!
fi


if [ ! -d $1 ] || [ ! -r $1  ]
then
    echo "Eroare: $1 nu este director, sau nu aveti drept de citire a acestuia!"
    exit 2  # Fail!
fi


for sursa in $1/*.cpp
do
    # Construim noul nume pentru executabil, cu tot cu calea până la el!
    # executabil=$1/$(basename $sursa .cpp)  # Atentie: extensia .exe nu este obligatorie, d.p.d.v. al SO-ului Linux !

    # Notă: comanda de mai sus poate fi rescrisa echivalent (și mai performant) precum în versiunea a 3-a a scriptului, adică:
    executabil=${sursa%.cpp}

    echo "Compilam fisierul $sursa si obtinem executabilul $executabil, in acelasi director cu sursa (doar dacă nu vor fi erori la compilare...)"
    g++ $sursa -o $executabil -Wall
done


exit 0  # Succes!