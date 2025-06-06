/*
* Funcționalitate: ilustrează terminarea normală a fiului creat de procesul inițial.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid_fiu;

    if (-1 == (pid_fiu = fork())) {
        perror("Eroare la fork");
        return 1;
    }

    if (pid_fiu == 0) {
        printf("\nProcesul fiu, cu PID-ul: %d.\n", getpid());

        return 3; // Sau, echivalent:  exit(3);
    }

    /* In procesul părinte. */
    int status;
    pid_fiu = wait(&status);
    if (-1 == pid_fiu) {
        perror("Eroare la wait");
        return 2;
    }

    printf("\nProcesul tata: fiul cu PID-ul %d s-a sfarsit cu status-ul: %d.\n", pid_fiu, status);

    /* Ilustrarea folosirii macro-urilor de inspecție a valorii stocate de apelul wait() în variabila status */
    if (WIFEXITED(status))
        printf("Tatal: codul de terminare al fiului (extras cu macroul WEXITSTATUS) este: %d.\n",
               WEXITSTATUS(status));
    //else
    if (WIFSIGNALED(status))
        printf("Tatal: fiul a fost terminat fortat, cu semnalul (extras cu macroul WTERMSIG): %d.\n",
               WTERMSIG(status));
    return 0;
}

    // The names of these macros are mnemonic:
// WIFEXITED:
// W: Stands for "wait" (as these macros operate on the status returned by wait or waitpid).
// IF: Indicates a conditional check (i.e., "if").
// EXITED: Refers to the child process having exited normally.
// So, "Wait, if exited".
//
// WEXITSTATUS:
// W: Stands for "wait".
// EXITSTATUS: Refers to the exit status code of the child process.
// So, "Wait, exit status".

