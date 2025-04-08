#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINES 10
#define BUFFER_SIZE 1024

void print_usage() {
    printf("Usage: ./myHead [-n lines] [-c bytes] [file...]\n");
}

void print_head(int fd, int mode, int count) {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int lines = 0;
    int bytes = 0;
    int newline_flag = 0;

    if (mode == 'n') {
        while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
            for (int i = 0; i < bytes_read; i++) {
                putchar(buffer[i]);
                if (buffer[i] == '\n') {
                    if (++lines >= count) {
                        newline_flag = 1;
                        break;
                    }
                }
            }
            if (newline_flag) break;
        }
        if (lines < count && bytes_read > 0) {
            putchar('\n');
        }
    } else if (mode == 'c') {
        while (1) {
            int bytes_remaining = count - bytes;
            int bytes_to_read = (bytes_remaining < BUFFER_SIZE) ? bytes_remaining : BUFFER_SIZE;

            bytes_read = read(fd, buffer, bytes_to_read);
            if (bytes_read <= 0) break;

            write(STDOUT_FILENO, buffer, bytes_read);
            bytes += bytes_read;
            if (bytes >= count) break;
        }
    }

    if (bytes_read == -1) {
        perror("read");
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int mode = 'n';
    int count = DEFAULT_LINES;
    int file_count = 0;

    while ((opt = getopt(argc, argv, "n:c:")) != -1) {
        switch (opt) {
            case 'n':
                mode = 'n';
                count = atoi(optarg);
                break;
            case 'c':
                mode = 'c';
                count = atoi(optarg);
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    file_count = argc - optind;

    if (file_count == 0) {
        print_usage();
        fprintf(stderr, "EROARE: Trebuie să specifici cel puțin un fișier!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = optind; i < argc; i++) {
        if (file_count > 1) {
            printf("\n==> %s <==\n", argv[i]);
        }

        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            perror("open");
            continue;
        }

        print_head(fd, mode, count);
        putchar('\n');
        close(fd);
    }
    return 0;
}
