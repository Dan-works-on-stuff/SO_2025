#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd;
    long offset;
    ssize_t bytes_in, bytes_out;
    char buffer[6], *marcaj = "ZZZZZ";

    fd = open("datafile.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }


    /* Urmează prima citire din fișier, a 5 octeți, începând cu al 11-lea octet din fișier. */
    offset = lseek(fd, 10, SEEK_SET);
    if (offset == -1) {
        perror("1st lseek");
        return 2;
    }
    fprintf(stderr, "[Debug info] The new offset after 1st lseek is: %ld\n", offset);

    bytes_in = read(fd, buffer, 5);
    if (bytes_in == -1) {
        perror("1st read");
        return 3;
    }
    if (bytes_in != 5) { fprintf(stderr, "1st read warning: insufficient information in file!\n"); }

    buffer[bytes_in] = 0;
    printf("First read from file: %s\n", buffer);


    /* Urmează suprascrierea a 3 octeți peste informația din fișier, începând cu al 11-lea octet din fișier. */
    offset = lseek(fd, 10, SEEK_SET);
    if (offset == -1) {
        perror("2nd lseek");
        return 4;
    }
    fprintf(stderr, "[Debug info] The new offset after 2nd lseek is: %ld\n", offset);

    bytes_out = write(fd, marcaj, 4);
    if (bytes_out == -1) {
        perror("3rd lseek");
        return 5;
    }


    /* Acum vom verifica efectul acestei suprascrieri...
       Urmează a doua citire din fișier, a 5 octeți, începând cu al 11-lea octet din fișier. */

    offset = lseek(fd, 10, SEEK_SET);
    if (offset == -1) {
        perror("4th lseek");
        return 6;
    }
    fprintf(stderr, "[Debug info] The new offset after 3rd lseek is: %ld\n", offset);

    bytes_in = read(fd, buffer, 5);
    if (bytes_in == -1) {
        perror("5th lseek");
        return 7;
    }

    buffer[bytes_in] = 0;
    printf("Second read from file: %s\n", buffer);

    close(fd);
    return 0;
}
