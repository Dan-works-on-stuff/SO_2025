/*
* Program: access_v2.c
*
* Funcționalitate: versiunea 2 a programului demonstrativ de acces concurent/exclusiv a două procese la un fișier dat.
*/
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd, codRead, codBlocaj;
    unsigned char ch;
    off_t poz;
    struct flock lacat;

    /* Verificarea existenței argumentului în linia de comandă. */
    if(argv[1] == NULL)
    {
        fprintf(stderr,"Programul %s trebuie apelat cu cel putin un parametru.\n", argv[0]);  exit(1);
    }

    /* Deschiderea fișierului de date. */
    if(-1 == (fd=open("fis.dat", O_RDWR)) )
    {   /* Tratează cazul de eroare la deschidere. */
        perror("Eroare, nu pot deschide fisierul fis.dat deoarece ");
        exit(2);
    }

    /* Pregătirea lacătului pentru fișier. */
    lacat.l_type   = F_WRLCK;
    lacat.l_whence = SEEK_SET;
    lacat.l_start  = 0;
    lacat.l_len    = 1; /* Aici se poate pune orice valoare, inclusiv 0, deoarece pentru această versiune v2 a
             programului NU contează lungimea zonei blocate (practic, în v2 e ca și cum am bloca tot fișierul). */

    /* Încercări repetate de punere a lacătului până când reusește. */
    printf("\n[ProcesID:%d] Incep incercarile de punere a blocajului prin apel neblocant.\n",getpid());
    while( (-1 == (codBlocaj = fcntl(fd,F_SETLK,&lacat)) ) && ((errno == EACCES)||(errno == EAGAIN)) )
    {
        fprintf(stderr, "[ProcesID:%d] Blocaj imposibil...", getpid());
        perror("\tMotivul erorii");
        sleep(3); // O mică pauză, înainte de a reîncerca punerea lacătului.
    }
    if(codBlocaj == -1)
    {
        fprintf(stderr, "[ProcesID:%d] Eroare grava la blocaj...", getpid());
        perror("\tMotivul erorii");
        exit(3);
    }
    printf("[ProcesID:%d] Blocaj reusit!\n", getpid());


    /* Bucla de parcurgere a fișierului, citind caracter cu caracter până la EOF. */
    while(0 != (codRead = read(fd,&ch,1)) )
    {
        if (-1 == codRead) { perror("Eroare la apelul read"); exit(4); } /* Tratează cazul de eroare la citire. */

        if(ch == '#')
        {   /* Am întâlnit prima poziție cu '#' în fișierul de date. */

            if(-1 == (poz = lseek(fd,-1,SEEK_CUR)) ) { perror("Eroare la apelul lseek"); exit(5); } /* Tratează cazul de eroare la repoziționare. */

            sleep(5); // O pauză, pentru a forța anumite ordini la execuția concurentă a mai multor instanțe ale programului.

            if(-1 == write(fd,argv[1],1) ) { perror("Eroare la apelul write"); exit(6); } /* Tratează cazul de eroare la scriere. */

            printf("[ProcesID:%d] Terminat. S-a inlocuit primul '#' gasit in fisierul fis.dat, la pozitia: %ld.\n", getpid(), poz);
            return 0;
        }
    }// end bucla while

    close(fd);
    printf("[ProcesID:%d] Terminat. Nu exista '#' in fisierul fis.dat !\n", getpid());
    return 0;
}