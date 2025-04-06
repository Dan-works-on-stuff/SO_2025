#!/bin/bash
### My id command. ###

if [ $# -eq 0 ]
then
    echo "Eroare: parametri insuficienti! Nu ati specificat UID-ul."
    exit 1  # Fail!
fi

cuv=":$1:"
linie=$(grep $cuv /etc/passwd)
if [ $? -eq 0 ]
then
    nume_real=$(echo $linie | cut -d: -f5 | cut -d, -f1)
    if [ -n "$nume_real" ]
    then 
        echo Numele real: $nume_real 
    else 
        echo "Nu exista informatii despre numele real." 
    fi

    nume_cont=$(echo $linie | cut -d: -f1)
    echo Numele de cont: $nume_cont

    gid_principal=`echo $linie | cut -d: -f4`
    grup_principal=`grep :$gid_principal: /etc/group | cut -d: -f1`
    echo Grupul principal din care face parte: $grup_principal

    grupuri_secundare=`grep -w $nume_cont /etc/group | cut -d: -f1`
    if [ -n "$grupuri_secundare" ] ; then
        echo Grupurile secundare de care apartine: $grupuri_secundare
    else
        echo "Nu exista informatii despre grupurile secundare din care face parte." 
    fi
else
    echo "Eroare: nu exista nici un utilizator avand $1 drept UID."
    exit 2  # Fail!
fi

exit 0  # Succes!

