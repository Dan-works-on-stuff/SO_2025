//
// Created by maxcox on 4/9/25.
//
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int argument = 0;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
    }
    if (argc < 2 || argv[1][0] == '.' && argv[1][1] == '\0') {
        argument = 0;
    } else if (argv[1][0] == '-') {
        switch (argv[1][1]) {
            case 'A':
                argument = 1;
            break;
            case 'l':
                argument = 2;
            break;
            case 'a':
                argument = 3;
            break;
            default:
                printf("Unknown option\n");
            break;
        }
    } else {
        if (strcmp(argv[1], "..") == 0) {
            closedir(dir); // Close the initial current directory
            dir = opendir("..");
            if (dir == NULL) {
                perror("opendir");
            }
        }
    }

    struct dirent *entry;

    if (argument == 0) {
        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, ".",1) == 0) {
                continue;
            }
            printf("%s\n", entry->d_name);
        }
    } else if (argument == 1) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            printf("%s\n", entry->d_name);
        }
    } else if (argument == 2) {
        while ((entry = readdir(dir)) != NULL) {

        }
    } else if (argument == 3) {
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
    return 0;
}
