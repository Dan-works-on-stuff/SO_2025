/*
*  Program: com-0.c
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    int fd;
    fd = open("fis.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(-1 == fd )  { perror("Eroare la crearea fisierului"); abort(); }
    // Notă: dacă dorim o redirectare cu append în loc de rewrite, este suficient să înlocuim flagul O_TRUNC
    // cu O_APPEND, în apelul open de mai sus.

    /* Redirectăm fluxul stdout (cu rewrite) în fișierul fis.txt */
    close(1);  /* Închidem descriptorul 1, "asociat" fluxului stdout. */
    dup(fd);   /* Duplicăm fd cu 1 (1 fiind, în acest moment, prima intrare liberă în tabelul local
    cu sesiuni de lucru cu fișiere). */
    close(fd); /* Închidem descriptorul fd. */

    /* Scriem pe fluxul stdout, adică în fișier. */
    write(1,"Salut",5);

    /* Reacoperim cu programul com-2 */
    execl("com-2","com-2",NULL);

    /* Se va executa numai dacă eșuează apelul exec ! */
    perror("Eroare la apelul execl");
    return 1;
}
