//
// Created by maxcox on 6/4/25.
//

// Să se scrie un program C care primește de la linia de comandă numele a două fișiere și două caractere,
// cu care va face următoarea procesare: va copia conținutul fișierului de intrare în cel de ieșire, înlocuind fiecare
// apariție a primului caracter specificat ca argument cu cel de-al doilea caracter specificat ca argument.
// În caz că fișierul de ieșire deja există, se va cere confirmare de suprascriere. Respectiv, va fi creat în cazul
// în care nu există, cu drepturi de citire și scriere doar pentru proprietar. MAPARE

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
    if (argc != 5) {
        handle_error("Usage: ./myTr input_file output_file chr1 chr2");
    }
    int fd_in, fd_out;
    struct stat sb;
    char *map_in, *map_out;
    char chr1 = argv[3][0], chr2 = argv[4][0];
    size_t length;
    // Open input file
    fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        handle_error("Error opening input file");
    }
    // Get input file size
    if (fstat(fd_in, &sb) == -1) {
        handle_error("Error getting input file size");
    }
    length = sb.st_size;
    // Map input file to memory
    map_in = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (map_in == MAP_FAILED) {
        handle_error("Error mapping input file");
    }
    // Open output file
    fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd_out == -1) {
        handle_error("Error opening output file");
    }
    // Truncate output file to the size of input file
    if (ftruncate(fd_out, length) == -1) {
        handle_error("Error truncating output file");
    }
    // Map output file to memory
    map_out = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_out, 0);
    if (map_out == MAP_FAILED) {
        handle_error("Error mapping output file");
    }
    // Copy and replace characters
    for (size_t i = 0; i < length; i++) {
        if (map_in[i] == chr1) {
            map_out[i] = chr2;
        } else {
            map_out[i] = map_in[i];
        }
    }
    msync(map_out, length, MS_SYNC); // Ensure changes are written to disk
    // Unmap files
    if (munmap(map_in, length) == -1) {
        handle_error("Error unmapping input file");
    }
    if (munmap(map_out, length) == -1) {
        handle_error("Error unmapping output file");
    }
    // Close file descriptors
    if (close(fd_in) == -1) {
        handle_error("Error closing input file descriptor");
    }
    if (close(fd_out) == -1) {
        handle_error("Error closing output file descriptor");
    }
    printf("File processed successfully: '%s' -> '%s', replacing '%c' with '%c'\n", argv[1], argv[2], chr1, chr2);
    return 0;
}