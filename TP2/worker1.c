//1) În funcția main, va face inițializările necesare pentru a putea primi informații de la procesul supervisor printr-un
//canal anonim (prin comunicații unu-la-unu). De asemenea, tot în funcția main, va face inițializările necesare
//pentru a putea trimite informații către procesul worker2, printr-un canal fifo (prin comunicații unu-la-unu).
//ii) Într-o funcție separată, apelată din funcția main, programul va citi din acel canal anonim, folosind apeluri
//POSIX, fiecare număr transmis lui de către procesul supervisor (prin mesaje de lungime constantă, folosind
//reprezentarea binară a numerelor întregi), și va verifica dacă acesta este număr nedivizibil cu 3, caz în care îl
//înmulțește cu 3, sau este număr divizibil cu 3, caz în care îl păstrează neschimbat; în ambele cazuri, valoarea
//astfel obținută va fi transmisă mai departe către procesul worker2, prin acel canal canal fifo (se va păstra
//reprezentarea în binar pentru numărul respectiv). De asemenea, programul va contoriza într-o variabilă
//contor_inmultiri câte operații de înmulțire cu 3 a efectuat pe parcursul procesării tuturor numerelor primite de la
//procesul supervisor și va transmite la final această valoare către procesul worker2, prin canalul fifo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define handle_error(msg) \
{ perror(msg); exit(EXIT_FAILURE); }

#define FIFO_PATH "/home/maxcox/Documents/GitHub/SO_2025/TP2/worker1_worker2.fifo"

void process_numbers(int in_fd, int out_fd) {
    int n, count_mul = 0;
    while (read(in_fd, &n, sizeof(n)) == sizeof(n)) {
        if (n % 3 != 0) {
            n *= 3;
            count_mul++;
        }

        write(out_fd, &n, sizeof(n)); // -----> worker2
    }
    write(out_fd, &count_mul, sizeof(count_mul));
}

int main(int argc, char *argv[]) {
    int fd_fifo;

    mkfifo(FIFO_PATH, 0666);
    fd_fifo = open(FIFO_PATH, O_WRONLY);
    if (fd_fifo == -1) {
        handle_error("open fifo");
    }

    process_numbers(STDIN_FILENO, fd_fifo);

    close(fd_fifo);
    return 0;
}
