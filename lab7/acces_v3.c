#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    int fd, codRead, openfd;
    unsigned char ch;
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

    /* Pregătirea unui lacăt pentru fișier, pus DOAR pe octetul/caracterul de la poziția curentă. */
    lacat.l_type   = F_WRLCK;
    lacat.l_whence = SEEK_CUR;
    lacat.l_start  = 0;
    lacat.l_len    = 1; /* Blocăm DOAR octetul/caracterul de la poziția curentă. */
    int poz=0;
    char buff[BUF_SIZE];
    /* Bucla de parcurgere a fișierului, citind caracter cu caracter până la EOF. */
    while(0 != (codRead = read(fd,&ch,1)) )
    {
        if (-1 == codRead) { perror("Eroare la apelul read"); exit(3); } /* Tratează cazul de eroare la citire. */

        if(ch == '#')
        {   /* Am întâlnit prima poziție cu '#' în fișierul de date. */

            if(-1 == (poz = lseek(fd,-1L,SEEK_CUR) )) { perror("Eroare la apelul lseek"); exit(4); } /* Tratează cazul de eroare la repoziționare. */

            /* O singură încercare de punere a lacătului în mod blocant  (i.e., apelul intră în așteptare până când reusește). */
            printf("\n[ProcesID:%d] Incep apelul de punere a blocajului in mod blocant, pe pozitia cu primul '#' deja gasit în fisier.\n",getpid());

            if(-1 == fcntl(fd,F_SETLKW,&lacat) )
            {
                if(errno == EINTR)
                    fprintf(stderr,"[ProcesID:%d] Eroare, apelul fcntl a fost intrerupt de un semnal...", getpid());
                else
                    fprintf(stderr,"[ProcesID:%d] Eroare la blocaj...", getpid());

                perror("\tMotivul erorii");
                exit(5);
            }
            printf("[ProcesID:%d] Blocaj reusit!\n", getpid());
            openfd=open("fis.dat", O_RDONLY);
            int bytes_read = read(openfd, buff, BUF_SIZE);
            write(STDOUT_FILENO, buff, bytes_read);
            sleep(5); // O pauză, pentru a forța anumite ordini la execuția concurentă a mai multor instanțe ale programului.

            if(-1 == write(fd,argv[1],1) ) { perror("Eroare la apelul write"); exit(6); } /* Tratează cazul de eroare la scriere. */
            printf("[ProcesID:%d] Terminat. S-a inlocuit primul '#' gasit in fisierul fis.dat la index %d !\n", getpid(), poz);
            write(STDOUT_FILENO, buff, bytes_read);
            return 0;
        }
    }// end bucla while

    close(fd);
    printf("[ProcesID:%d] Terminat. Nu exista '#' in fisierul fis.dat !\n", getpid());
    return 0;
}