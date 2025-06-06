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

void print_first_n_lines(const char *map, size_t length, int n);
void print_first_n_chars(const char *map, size_t length, int n);
int count_lines(const char *map, size_t length);
void process_file(const char *filename, const char *option, int limit);
void default_head(const char *filename);

void print_first_n_lines(const char *map, size_t length, int n) {
    int printed = 0;
    for (size_t i = 0; i < length; i++) {
        putchar(map[i]);
        if (map[i] == '\n' && ++printed >= n) break;
    }
}

void print_first_n_chars(const char *map, size_t length, int n) {
    for (int i = 0; i < n && i < (int)length; i++) {
        putchar(map[i]);
    }
}

int count_lines(const char *map, size_t length) {
    int total = 0;
    for (size_t i = 0; i < length; i++) {
        if (map[i] == '\n')
            total++;
    }
    return total;
}

void process_file(const char *filename, const char *option, int limit) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) handle_error("Error opening input file");

    struct stat sb;
    if (fstat(fd, &sb) == -1) handle_error("Error getting input file size");

    size_t length = sb.st_size;
    if (length == 0) { close(fd); return; } // empty file

    char *map = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) handle_error("Error mapping input file");

    if (strcmp(option, "-n") == 0) {
        int total_lines = count_lines(map, length);
        int effective = (limit >= 0) ? limit : total_lines + limit;
        if (effective < 0) effective = 0;
        print_first_n_lines(map, length, effective);
    } else if (strcmp(option, "-c") == 0) {
        int effective = (limit >= 0) ? limit : (int)length + limit;
        if (effective < 0) effective = 0;
        print_first_n_chars(map, length, effective);
    } else {
        fprintf(stderr, "Invalid option %s\n", option);
    }

    munmap(map, length);
    close(fd);
}

void default_head(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) handle_error("Error opening input file");

    struct stat sb;
    if (fstat(fd, &sb) == -1) handle_error("Error getting input file size");

    size_t length = sb.st_size;
    if (length == 0) { close(fd); return; }

    char *map = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) handle_error("Error mapping input file");

    print_first_n_lines(map, length, 10);

    munmap(map, length);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        default_head(argv[1]);
    } else if (argc == 4) {
        const char *option = argv[1];
        int limit = atoi(argv[2]);
        process_file(argv[3], option, limit);
    } else {
        fprintf(stderr, "Usage: %s [-n|-c] <nr> <input_file>\n", argv[0]);
        fprintf(stderr, "   or: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}
