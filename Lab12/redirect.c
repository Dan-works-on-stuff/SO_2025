//
// Created by maxcox on 6/7/25.
//
/*
*  Program: redirect.c
*/
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char *str1 = "Primul mesaj; va apare pe ecran.\n";
    write(STDOUT_FILENO, str1, strlen(str1));

    // Redirect stdout to "fis.txt" (overwrite).
    int fd;
    if(-1 == (fd = open("fis.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600)) ) {
        perror("Deschiderea fisierului a esuat. Motivul:");
        exit(1);
    }
    // Note: For append mode, replace O_TRUNC with O_APPEND in the open call.

    // dup2 will close STDOUT_FILENO if it's open, so explicit close is not strictly needed.
    if(-1 == dup2(fd,STDOUT_FILENO)) /* Duplicate fd to STDOUT_FILENO (1). */
    // Now, entry 1 in the process's file descriptor table refers to the same
    // open file description as fd (i.e., "fis.txt").
    {
        perror("Redirectarea a esuat. Motivul:");
        exit(2);
    }
    close(fd); // Close original descriptor fd, no longer needed.
               // STDOUT_FILENO still refers to the open file description.

    // This message will go to "fis.txt".
    char *str2 = "Al doilea mesaj scris pe stdout; va apare nu pe ecran, ci in fisier.\n";
    write(STDOUT_FILENO, str2, strlen(str2));


    // Restore stdout to the terminal.
    int fd1;
    if(-1 == (fd1 = open("/dev/tty",O_WRONLY)) ) {
        perror("Deschiderea terminalului a esuat. Motivul:");
        exit(3);
    }

    // Again, dup2 handles closing STDOUT_FILENO (currently fis.txt).
    if(-1 == dup2(fd1,STDOUT_FILENO))   /* Duplicate fd1 (terminal) to STDOUT_FILENO (1). */
    {
        perror("Re-redirectarea a esuat. Motivul:");
        exit(4);
    }
    close(fd1); // Close original descriptor fd1, no longer needed.
                // STDOUT_FILENO now refers to the terminal.

    char *str3 = "Al treilea mesaj; va apare pe ecran.\n";
    write(STDOUT_FILENO, str3, strlen(str3));

    // Adding a small delay to help ensure the last message is visible
    // before the program exits and the shell prompt potentially overwrites it.
    // If the message still doesn't appear, the issue is likely with how
    // the terminal/environment handles /dev/tty or these redirections.
    // sleep(1);

    return 0;
}

