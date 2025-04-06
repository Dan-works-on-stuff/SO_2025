#!/bin/bash
### EditCompileRun.sh : Script pentru automatizarea procesului de dezvoltare de programe C ###

if [ $# -eq 0 ]
then
    read -p "Dati numele fisierului sursa .c : " sursa
else
    sursa=$1
fi

while true
do
    code $sursa  #deschid vscode

    read -p "Doriti compilarea programului? (y/n) " answer
    if [ $answer = "y" ]
    then
        executabil=${sursa%.c}        # Construim noul nume pentru executabil, cu tot cu calea până la el!
                                      # Atenție: extensia .exe nu este obligatorie, d.p.d.v. al SO-ului Linux !

        gcc $sursa -o $executabil -Wall  # Compilarea fișierului sursă
        if [ $? -eq 0 ]               # S-a compilat fara erori?
        then
            read -p "Doriti executarea programului? (y/n) " answer
            if [ $answer = "y" ]
            then
                ./$executabil         # Aici puteți customiza linia de apel a executabilului în funcție de programul pe care-l dezvoltați (e.g., să-l apelați cu niște argumente, etc.)
            fi
    break
        else                          # Au fost erori la compilare
            read -p "Apasa <Enter> pentru a continua cu editarea erorilor..." answer   # Acest read are doar rol de pauza, pentru a avea timp să citiți erorile afișate de compilator
            continue
        fi
    else
        break
    fi
done

exit 0  # Succes!

