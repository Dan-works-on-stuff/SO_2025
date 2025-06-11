// Created by maxcox on 6/10/25.
// Să se scrie un program C care să simuleze apelul funcției system, folosind doar apeluri fork, wait și exec
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
    char **arguments;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <command> [args...] <directory>\n", argv[0]);
        exit(1);
    }
    int arg_count = argc -1;
    arguments = malloc((arg_count + 1) * sizeof(char*));
    if (!arguments) { perror("malloc"); exit(1); }
    for (int i = 0; i < arg_count; i++) {
        arguments[i] = argv[i + 1];
    }
    arguments[arg_count] = NULL;
// Pasul #1: obtinerea numelui directorului de prelucrat
    char nume_comanda[PATH_MAX];
    strcpy(nume_comanda, arguments[0]);
    strcpy(nume_dir, argv[argc - 1]);
    if (chdir(nume_dir) == -1) { perror("chdir"); exit(1); }

    switch (fork()) {
        case -1: perror("Eroare la primul fork");
        exit(1);
        case 0: execvp(nume_comanda, arguments);
        perror("Eroare la primul exec");
        exit(10);

        default: wait(&status), free(arguments);
        if (WIFEXITED(status))
            printf("Comanda a rulat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status));
        else
            printf("Comanda a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status));
    }
    return 0;
}