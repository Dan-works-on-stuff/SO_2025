/*
*   Program: pipe_ex1.c
*
*   Funcționalitate: demonstrează modul de utilizare a unui pipe (i.e., canal anonim) pentru comunicația unul-la-unul între două procese,
*   folosind funcțiile I/O de nivel scăzut (i.e., apelurile din API-ul POSIX).
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NMAX 1000

int main(void) {
    int pid, p[2];
    char ch;
    /* Creare canal anonim. */
    if (-1 == pipe(p)) {
        perror("Eroare la crearea canalului anonim");
        exit(1);
    }

    if (-1 == (pid = fork())) {
        perror("Eroare la crearea unui proces fiu");
        exit(2);
    }

    if (pid) {/* Secvența de cod executată doar de procesul tată. */

        /* Tatăl își închide capătul de citire din canal. */
        close(p[0]);
        printf("Introduceti o secventa de caractere diverse, terminata prin CTRL+D.\n");
        while (EOF != (ch = getchar()))
            if ((ch >= 'a') && (ch <= 'z'))
                write(p[1], &ch, 1);

        close(p[1]);/* Tatăl își închide capătul de scriere în canal, pentru ca fiul să poată citi EOF din canal (!!!) */


        wait(NULL);/* Tatăl așteaptă terminarea fiului. */

    } else {/* Secvența de cod executată doar de procesul fiu. */

        char buffer[NMAX];
        int nIndex = 0;

        close(p[1]);/* Fiul își închide capătul de scriere în canal, pentru ca ulterior să poată citi EOF din canal (!!!) */


        /* Fiul citește caracterele din canal și le salvează în buffer, într-o buclă, până când detectează EOF în canal, ... */
        while (read(p[0], &ch, 1) != 0)
            if (nIndex < NMAX)
                buffer[nIndex++] = ch;

        /* ..., iar apoi afișează conținutul bufferului. */
        if (nIndex == NMAX)
            buffer[NMAX - 1] = '\0';
        else
            buffer[nIndex] = '\0';
        printf("[Procesul consumator (fiul)] Am citit din canal secventa: %s\n", buffer);

        /* Fiul își închide capătul de citire din canal. */
        close(p[0]);
    }
    return 0;
}
