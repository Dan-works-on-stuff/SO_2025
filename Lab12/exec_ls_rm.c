/*
  Program: soluția incompletă a problemei [Exec command #3: ls ...; rm ...].
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>  // necesar pentru PATH_MAX

int main(int argc, char *argv[]) {
    char nume_dir[PATH_MAX];
    int status;
    // Pasul #1: obtinerea numelui directorului de prelucrat

    if (argc > 1) {
        strcpy(nume_dir, argv[1]);
    } else {
        printf("Introduceti numele directorului dorit:");
        scanf("%s", nume_dir);
    }

    // Pasul #2: invocarea primei comenzi: ls -l nume_dir

    switch (fork()) {
        case -1: perror("Eroare la primul fork");
            exit(1);

        case 0: execlp("ls", "ls", "-l", nume_dir, NULL);
            perror("Eroare la primul exec");
            exit(10);

        default: wait(&status);
            if (WIFEXITED(status))
                printf("Comanda ls a rulat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status));
            else
                printf("Comanda ls a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status));
    }

    // Pasul #3: invocarea celei de-a doua comenzi: rm -r -i nume_dir

    //  TODO: de implementat un switch similar ca la pasul #2, pentru invocarea comenzii rm, folosind însă un apel execv(), în loc de execlp()
    // ...
    switch (fork()) {
        case -1: perror("Eroare la primul fork");
        exit(1);

        case 0:
        {
            char *args_for_rm[] = {"rm", "-r", "-i", nume_dir, NULL};
            execv("/bin/rm", args_for_rm); //execvp("rm", args_for_rm);
            // If execv returns, an error occurred.
            perror("Eroare la al doilea exec (rm)"); // Updated error message for rm
        }

        default: wait(&status);
        if (WIFEXITED(status))
            printf("Comanda ls a rulat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status));
        else
            printf("Comanda ls a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status));
    }
}
