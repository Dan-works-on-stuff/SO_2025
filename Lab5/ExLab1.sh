#!/bin/bash
### call2find3 ###

# Validarea datelor de intrare

if [ $# -eq 0 ]
then
    read -p "Dați nivelul:" nr
    if [ -z "$nr" ]
    then
        nr='0'
    fi
else
    nr=$1
fi

# Parcurgere recursivă implicită, obținută aici cu ajutorul comenzii find
lista=$( find $HOME -maxdepth $nr -type f -mtime -28 )

nr_files=$(echo "$lista" | wc -l)
echo "Number of files found: $nr_files"

if [ $? -ne 0 ]
then
    echo "Command find has failed!"
    exit 1
else
    for fisier in $lista
    do
        if file "$fisier" | grep -q "C source"
        then
            sursa="$fisier"
            executabil="${sursa%.c}"
            # echo "Compilam fisierul $sursa si obtinem executabilul $executabil, in acelasi director cu sursa (doar dacă nu vor fi erori la compilare...)"
            gcc "$sursa" -o "$executabil" -Wall
            if [ $? -eq 0 ]
            then
                echo "$sursa"
                echo "Compilat ok"
            else
                echo "Compilat nu ok"
            fi
        else 
            echo "$(date -r "$fisier") $fisier"
        fi
    done
fi    

# for fis in $lista
# do
#     echo $fis
# done

exit 0  # Succes!
