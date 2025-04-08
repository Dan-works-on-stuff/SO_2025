#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 5) {
        perror("Wrong number of arguments");
        return 1;
    }
    int input_fd;
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("Error opening file");
        return 2;
    }
    int output_fd = open(argv[2], O_WRONLY);
    if (output_fd == -1) {
        perror("Error opening file");
        return 3;
    }
    char *buffer = (char *) malloc(BUFSIZE), chr1, chr2;
    if (buffer == NULL) {
        perror("Memory allocation failed");
        close(input_fd);
        close(output_fd);
        return 4;
    }
    chr1 = argv[3][0];
    chr2 = argv[4][0];
    ssize_t bytes_read;
    while ((bytes_read = read(input_fd, buffer, BUFSIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == chr1) {
                buffer[i] = chr2;
            }
        }
        write(output_fd, buffer, bytes_read);
    }
    free(buffer);
    close(input_fd);
    close(output_fd);
    return 0;
}
