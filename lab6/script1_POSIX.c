/*
Basic cp file copy program. POSIX implementation.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <errno.h>

#define BUF_SIZE 4096  /* !!! exact dimensiunea paginii de memorie, din motive de eficiență a operațiilor cu discul !!! */

int main (int argc, char *argv []) {
    int input_fd, output_fd;
    ssize_t bytes_in, bytes_out;
    char buffer[BUF_SIZE];
    if (argc != 3) {
        printf("Usage: cp file-src file-dest\n");  return 1;
    }
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror(argv[1]);  return 2; //cerr
    }
    output_fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0600); //O_TRUNC = empty the file
    if (output_fd == -1) {
        perror(argv[2]);  return 3;
    }
    /* Process the input file a record at atime. */
    while ((bytes_in = read(input_fd, buffer, BUF_SIZE)) > 0) {
        bytes_out = write(output_fd, buffer, bytes_in);
        if (bytes_out != bytes_in) {
            fprintf(stderr,"Fatal write error!\n");  return 4;
        }
    }
    close(input_fd);   close(output_fd);
    return 0;
}

