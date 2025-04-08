#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int input_fd;
    long offset;
    ssize_t bytes_in;
    char buffer[6];

    input_fd = open("datafile.txt", O_RDONLY);
    if (input_fd == -1) {
        perror("open");
        return 1;
    }

    /* Urmează prima citire din fișier, a 5 octeți, începând cu al 11-lea octet din fișier. */
    offset = lseek(input_fd, 10, SEEK_SET);
    if (offset == -1) {
        perror("1st lseek");
        return 2;
    }
    fprintf(stderr, "[Debug info] The new offset after 1st lseek is: %ld\n", offset);

    bytes_in = read(input_fd, buffer, 5);
    if (bytes_in == -1) {
        perror("1st read");
        return 3;
    }
    /* if (bytes_in != 5)  {  fprintf(stderr,"1st read : error incomplete read!\n");  return 4;  } */
    if (bytes_in != 5) { fprintf(stderr, "1st read warning: insufficient information in file!\n"); }

    buffer[bytes_in] = 0;
    printf("First read from file: %s\n", buffer);


    /* Urmează a doua citire din fișier, tot a 5 octeți, începând cu al 10-lea octet dinspre finalul fișierului. */
    offset = lseek(input_fd, -10, SEEK_END);
    if (offset == -1) {
        perror("2nd lseek");
        return 4;
    }
    fprintf(stderr, "[Debug info] The new offset after 2nd lseek is: %ld\n", offset);

    bytes_in = read(input_fd, buffer, 5);
    if (bytes_in == -1) {
        perror("2nd read");
        return 5;
    }

    buffer[bytes_in] = 0;
    buffer[bytes_in] = 0;
    printf("Second read from file: %s\n", buffer);

    close(input_fd);
    return 0;
}
