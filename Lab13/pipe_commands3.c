/*
*   Program: executarea a trei comenzi simple în paralel și înlănțuite!
*   Ierarhia celor 3 procese necesare: părinte cu un copil și un nepot.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main() {
    pid_t pid1;
    int pipe_Cmd3toCmd4[2];

    if (-1 == pipe(pipe_Cmd3toCmd4)) {
        fprintf(stderr, "Eroare: nu pot crea al treilea canal anonim, errno=%d\n",errno);
        perror("Cauza erorii");
        exit(1);
    }

    if (-1 == (pid1 = fork())) {
        perror("Eroare: nu am putut crea primul fiu. Cauza erorii");
        exit(2);
    }

    if (pid1 == 0) {
        int pipe_Cmd1toCmd2[2];
        pid_t pid2;
        // Secțiune de cod executată doar de către fiul procesului inițial.
        if (-1 == pipe(pipe_Cmd1toCmd2)) {
            fprintf(stderr, "Eroare: nu pot crea primul canal anonim, errno=%d\n",errno);
            perror("Cauza erorii");
            exit(3);
        }

        if (-1 == (pid2 = fork())) {
            perror("Eroare: nu am putut crea al doilea proces fiu. Cauza erorii");
            exit(4);
        }

        if (pid2 == 0) {
            int pipe_Cmd2toCmd3[2];
            pid_t pid3;
            if (-1 == pipe(pipe_Cmd2toCmd3)) {
                fprintf(stderr, "Eroare: nu pot crea al treilea canal anonim, errno=%d\n",errno);
                perror("Cauza erorii");
                exit(1);
            }
            if (-1 == (pid3 = fork())) {
                perror("Eroare: nu am putut crea primul fiu. Cauza erorii");
                exit(2);
            }
            if (pid3 == 0) {
                // Secțiune de cod executată doar de către "nepotul" procesului inițial.
                if (-1 == dup2(pipe_Cmd2toCmd3[1], 1)) {
                    perror("Eroare: redirectarea iesirii standard in al treilea proces fiu a esuat. Cauza erorii");
                    exit(5);
                }

                execlp("ps", "ps",
                       "-eo", "user,comm,pid",
                       "--no-headers",
                       NULL);
                perror("Eroare la execlp() in al treilea proces fiu. Cauza erorii");
                exit(6);
            }

            // configure tr to read from ps output via pipe_Cmd2toCmd3
            close(pipe_Cmd2toCmd3[1]);
            if (-1 == dup2(pipe_Cmd2toCmd3[0], 0)) {
                perror("Eroare: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");
                exit(5);
            }

            // redirect tr output to first child's pipe
            close(pipe_Cmd1toCmd2[0]);
            if (-1 == dup2(pipe_Cmd1toCmd2[1], 1)) {
                perror("Eroare: redirectarea iesirii standard in al doilea proces fiu a esuat. Cauza erorii");
                exit(5);
            }

            execlp("tr", "tr", "-s", " ",NULL);
            perror("Eroare la execlp() in al doilea proces fiu. Cauza erorii");
            exit(6);
        }
        // Secțiune de cod executată doar de către fiul procesului inițial.
        close(pipe_Cmd1toCmd2[1]);

        if (-1 == dup2(pipe_Cmd1toCmd2[0], 0)) {
            perror("Eroare: redirectarea intrarii standard in primul fiu a esuat. Cauza erorii");
            exit(7);
        }

        if (-1 == dup2(pipe_Cmd3toCmd4[1], 1)) {
            perror("Eroare: redirectarea iesirii standard in primul fiu a esuat. Cauza erorii");
            exit(8);
        }

        execlp("cut", "cut",
               "-d", " ",
               "-f1,2,3",
               "--output-delimiter=:",
               NULL);
        perror("Eroare la execlp() in primul fiu. Cauza erorii");
        exit(9);
    }
    // Secțiune de cod executată doar de către părinte (i.e., procesul inițial).
    close(pipe_Cmd3toCmd4[1]);

    if (-1 == dup2(pipe_Cmd3toCmd4[0], 0)) {
        perror("Eroare: redirectarea intrarii standard in parinte a esuat. Cauza erorii");
        exit(10);
    }

    execlp("sort", "sort",
           "-k2",
           "-t:",
           NULL);
    perror("Eroare la execlp() in parinte. Cauza erorii");
    exit(11);
}