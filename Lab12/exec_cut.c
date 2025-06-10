#include <unistd.h>     // Pentru fork, execvp
#include <stdio.h>      // Pentru printf, perror
#include <stdlib.h>     // Pentru exit, EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>  // Pentru pid_t
#include <sys/wait.h>   // Pentru wait, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG

int main(void)
{
    int status;
    pid_t pid;

    // cut --fields=1,3,4,6 -d: --output-delimiter=" -- " /etc/passwd
    char *args_cut[] = {
        "cut",                  // Numele comenzii
        "--fields=1,3,4,6",     // Optiunea --fields cu valoarea sa
        "-d",                   // Optiunea -d
        ":",                    // Delimitatorul pentru -d
        "--output-delimiter= -- ", // Optiunea --output-delimiter cu valoarea " -- "
        "/etc/passwd",          // Fisierul de intrare
        NULL                    // Terminator pentru tabloul de argumente
    };

    pid = fork();

    if (pid == -1) {
        perror("Eroare la fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // Se incearca executia comenzii 'cut'
        // execvp cauta comanda 'cut' in directoarele specificate de variabila de mediu PATH
        // si inlocuieste imaginea procesului copil cu cea a comenzii 'cut'.
        execvp(args_cut[0], args_cut);

        // Daca functia execvp returneaza, inseamna ca a aparut o eroare
        // (de ex., comanda 'cut' nu a fost gasita sau nu poate fi executata).
        perror("Eroare la execvp pentru cut");
        exit(127);
    } else {

        // Asteapta terminarea procesului copil
        if (wait(&status) == -1) {
            perror("Eroare la wait");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 127) { //child process exited badly
                printf("Comanda cut nu a putut fi executata...\n");
            } else {
                printf("Comanda cut a fost executata ...\n");
            }
        } else if (WIFSIGNALED(status)) {

            printf("Comanda cut a fost executata ...\n");
        } else {
            printf("Comanda cut nu a putut fi executata... (stare copil necunoscuta dupa wait)\n");
        }
    }

    return EXIT_SUCCESS;
}
