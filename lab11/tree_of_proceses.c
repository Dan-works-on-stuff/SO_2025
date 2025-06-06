// Să se scrie un program C care să creeze un arbore particular de procese, având 3 nivele, structurate astfel:
// 1) unicul proces P1,1 de pe nivelul 1 al arborelui (i.e., rădăcina arborelui) va avea 4 procese fii, și anume procesele P2,1 ,
// P2,2 , P2,3 și P2,4 de pe nivelul 2 al arborelui;
// 2) fiecare dintre cele 4 procese de pe nivelul 2 al arborelui, va avea la rândul său 3 procese fii pe nivelul 3 al arborelui,
// numerotate cu P3,1 , P3,2 , ... , P3,12 , care vor fi astfel "nepoți" ai procesului din rădăcina arborelui și, în plus, nu vor avea
// nici un fiu la rândul lor (i.e., vor fi "frunze" în arbore).
// Fiecare proces va afișa pe ecran, pe o singură linie (!), mesajul compus din următoarele informații:
// "numărul lui de ordine" în arbore, urmat de PID-ul lui, cel al părintelui său, precum și perechile de PID-uri
// și coduri de terminare corespunzătoare tuturor fiilor acestuia. (Prin "numărul lui de ordine" în arbore înțelegem perechea (i,j) ,
// ce poate avea următoarele valori: (1,1), (2,1), (2,2), (2,3), (3,1), ... , (3,12) .)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define MAX_CHILDREN 4
#define MAX_GRANDCHILDREN 3

int main() {
    unsigned int pid[MAX_CHILDREN];
    unsigned int pid_grandchild[MAX_CHILDREN][MAX_GRANDCHILDREN];
    int status;
    pid_t p1 = getpid(); // PID-ul procesului rădăcină
    printf("Rădăcina arborelui de procese: PID=%d, PARENT_PID=%d\n", p1, getppid());
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if ((pid[i] = fork()) < 0) {
            perror("Eroare la fork");
            exit(EXIT_FAILURE);
        }
        if (pid[i] == 0) { // Procesul fiu
            printf("Procesul P2,%d: PID=%d, PARENT_PID=%d\n", i + 1, getpid(), getppid());
            for (int j = 0; j < MAX_GRANDCHILDREN; j++) {
                if ((pid_grandchild[i][j] = fork()) < 0) {
                    perror("Eroare la fork");
                    exit(EXIT_FAILURE);
                }
                if (pid_grandchild[i][j] == 0) { // Procesul nepot
                    printf("Procesul P3,%d: PID=%d, PARENT_PID=%d\n", i * MAX_GRANDCHILDREN + j + 1, getpid(), getppid());
                    exit(0); // Nepotul termină execuția
                }
            }
            // Procesul fiu așteaptă toți nepoții să termine
            for (int j = 0; j < MAX_GRANDCHILDREN; j++) {
                waitpid(pid_grandchild[i][j], &status, 0);
                printf("Procesul P2,%d: Nepot PID=%d s-a terminat cu status=%d\n", i + 1, pid_grandchild[i][j], WEXITSTATUS(status));
            }
            exit(0); // Fiu termină execuția
        }
        // Procesul părinte așteaptă toți fiii să termine
        waitpid(pid[i], &status, 0);
        printf("Procesul P1: Fiul PID=%d s-a terminat cu status=%d\n", pid[i], WEXITSTATUS(status));
    }
    printf("Procesul P1: Toți fiii s-au terminat. Procesul rădăcină PID=%d se termină.\n", p1);
    return 0;
}
