#!/bin/bash
### Calculeaza recursiv numarul de fisiere fifo si subdirectoare dintr-un director dat. ###
### Afiseaza numarul total de fisiere fifo si subdirectoare continute de acesta.        ###

if [ $# -eq 0 ]
then
    read -p "Dati numele directorului de start: " numedir
    [ -z $numedir ] && numedir="."   # Dacă utilizatorul introduce cuvântul vid la read (i.e., apasă direct ENTER), vom procesa directorul curent de lucru!
else
    numedir=$1
fi

nr_fif=0
nr_dir=0

function parcurgere_director ()
{
    arg_dir=$1  # argumentul de apel
    for fis in $arg_dir/* $arg_dir/.*
    do
        # Prin modul de evaluare a șabloanelor de specificare multiplă de fișiere, valoarea $fis conține inclusiv calea până la fișierul curent, i.e. calea specificată prin $arg_dir
        cale=$fis   # Este suficient doar atât, căci argumentul primului apel nu va putea fi cuvântul vid niciodată! (Motivul: a se vedea primul if din script)
		
        # Procesăm calea respectivă în funcție de tipul ei, i.e. subdirector/fișier fifo.
        if [ -d $cale ] ; then
            let nr_dir++
            parcurgere_director $cale    # apelul recursiv
        elif [ -p $cale ] ; then
            let nr_fif++
        fi
    done
}
# Observație: mai sus ar fi trebuit eliminat cazurile de excepție când nu există niciun fișier cu numele de forma * sau,
# respectiv, de forma .* în directorul $arg_dir, căci altfel ar fi dat eroare 'file not found' în comenzile ce ar fi putut urma în bucla do-done.
# Însă, aici, această filtrare a celor două cazuri de excepție se realizează implicit prin cele două teste [ -d $cale ]
# și, respectiv [ -p $cale ] de mai sus, care testează existența unui fișier de tip director, respectiv fifo, cu acel nume !!!


# Observație: bucla for din funcția de mai sus se poate scrie echivalent și în forma următoare (dar această formă este mai ineficientă la execuție):
#
# for fis in $(ls -A $arg_dir) ; do
#    # Construim calea până la intrarea $fis din directorul $arg_dir
#    cale=$arg_dir/$fis
#
#    ... restul e identic ca mai sus ...
# done
#


parcurgere_director $numedir             # apelul initial

echo "Directorul $numedir contine $nr_dir directoare si $nr_fif fisiere fifo."
