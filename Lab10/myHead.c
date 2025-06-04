//Să se scrie un program C ce implementează comanda head,
//inclusiv cu opțiunile -n și -c (și inclusiv cu valori numerice negative pentru aceste opțiuni).
//Se va permite precizarea de argumente multiple de tip nume de fișiere, în linia de comandă a programului,
//pentru procesare.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define handle_error(msg) \
{ perror(msg); exit(EXIT_FAILURE); }

int main(int argc, char *argv[]) {
    if (!(argc == 2 || argc == 4)) {
        handle_error("Usage: ./myHead [option] [nr] input_file");
    }
    if (argc != 2 && argc != 4) {
        handle_error("Usage: ./myHead [-n|-c] <nr> <input_file>");
    }

    int fd_in;
    struct stat sb;
    char *map_in;
    size_t length;
    int nr_lines = 10; // default

    if (argc == 2) {
        // Open the file from argv[1]
        fd_in = open(argv[1], O_RDONLY);
    } else {
        // Check option from argv[1], parse nr from argv[2], open file from argv[3]
        if (strcmp(argv[1], "-n") != 0 && strcmp(argv[1], "-c") != 0) {
            handle_error("Invalid arguments. Use -n <lines> or -c <chars>.");
        }
        fd_in = open(argv[3], O_RDONLY);
    }
    if (fd_in == -1) {
        handle_error("Error opening input file");
    }
    if (fstat(fd_in, &sb) == -1) {
        handle_error("Error getting input file size");
    }
    length = sb.st_size;
    map_in = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (map_in == MAP_FAILED) {
        handle_error("Error mapping input file");
    }

    if (argc == 2) {
        // Default head: first 10 lines
        int printed = 0;
        for (size_t i = 0; i < length; i++) {
            putchar(map_in[i]);
            if (map_in[i] == '\n' && ++printed >= nr_lines) break;
        }
    } else {
        int limit = atoi(argv[2]);
        if (strcmp(argv[1], "-n") == 0) {
            int total_lines = 0;
            for (size_t i = 0; i < length; i++) {
                if (map_in[i] == '\n')
                    total_lines++;
            }
            int effective = (limit >= 0) ? limit : total_lines + limit;
            if (effective < 0) effective = 0;
            int printed = 0;
            for (size_t i = 0; i < length; i++) {
                putchar(map_in[i]);
                if (map_in[i] == '\n') {
                    printed++;
                    if (printed >= effective) break;
                }
            }
        } else if (strcmp(argv[1], "-c") == 0) {
            int effective = (limit >= 0) ? limit : (int)length + limit;
            if (effective < 0) effective = 0;
            for (int i = 0; i < effective && i < (int)length; i++) {
                putchar(map_in[i]);
            }
        }
    }
    munmap(map_in, length);
    close(fd_in);
    return 0;
}