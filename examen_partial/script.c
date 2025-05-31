// 604391
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096
#define PATH_SIZE 4096

typedef struct {
    int permissions;
    long count; 
} permisiuni;

int lock_file(int fd);

int unlock_file(int fd);

int update_count(int fd, mode_t perms);

int process_path(const char *path, int binary_fd);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./script <file> <path>\n");
    }
    if (argv[1] == "-o") {
        char *filename = argv[2];
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(1);
        }
        int bytes=0, num=0;
        while((bytes = read(fd, &num, sizeof(int))) > 0) {
            if (bytes != sizeof(int)) {
                fprintf(stderr, "Eroare: fișierul conține date incomplete.\n");
                exit(EXIT_FAILURE);
            }
            printf("%d ", num);
        }
    }else {
        char *filename = argv[1];
        int fd = open(filename, O_RDONLY | O_CREAT);
        if (fd == -1) {
            perror("open");
            return 1;
        }
        const char *start_dir = argv[1];
        const char *binary_file = argv[2];
        int binary_fd;

        printf("Pornesc scanarea directorului: %s\n", start_dir);
        printf("Salvez rezultatele in: %s\n", binary_file);

        if (process_path(start_dir, binary_fd) != 0) {
            fprintf(stderr, "Scanarea s-a terminat cu erori.\n");
        } else {
            printf("Scanare finalizata.\n");
        }

        if (close(binary_fd) == -1) {
            perror("Eroare la inchiderea fisierului binar");
            return EXIT_FAILURE;
        }
        return 0;
    }
    return 0;
}

int lock_file(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("(fcntl F_SETLKW)");
        return 2;
    }
    return 0;
}

int unlock_file(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("(fcntl F_UNLCK)");
        return 3;
    }
    return 0;
}

int update_count(int fd, mode_t perms) {
    int relevant_perms = perms & 0777;
    permisiuni record;
    int bytes_read;
    long current_pos;
    int found = 0;
    int result = 0;

    if (lock_file(fd) == -1) {
        return 4;
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        unlock_file(fd);
        return 5;
    }

    while ((bytes_read = read(fd, &record, sizeof(permisiuni))) > 0) {
        if (bytes_read != sizeof(permisiuni)) {
            fprintf(stderr, "Eroare: Citire incompleta din fisierul binar.\n");
            result = -1;
            unlock_file(fd);
                result = -1;
        }

        if (record.permissions == relevant_perms) {
            found = 1;
            record.count++;

            current_pos = lseek(fd, 0, SEEK_CUR);
            if (current_pos == (off_t)-1) {
                 perror("Eroare la citirea pozitiei curente (lseek SEEK_CUR)");
                 result = -1;
                unlock_file(fd);
                result = -1;
            }
            if (lseek(fd, current_pos - sizeof(permisiuni), SEEK_SET) == (off_t)-1) {
                perror("Eroare la repozitionare pentru scriere (lseek SEEK_SET)");
                result = -1;
                unlock_file(fd);
                    result = -1;

            }

            if (write(fd, &record, sizeof(permisiuni)) != sizeof(permisiuni)) {
                perror("Eroare la scrierea inregistrarii actualizate");
                unlock_file(fd);
                result = -1;
            }
            break;
        }
    }

    if (bytes_read == -1) {
        perror("read");
        result = -1;
        unlock_file(fd);


    if (!found) {
        if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
             perror("lseek");
             result = -1;
            unlock_file(fd);
                result = -1;

        }

        record.permissions = relevant_perms;
        record.count = 1;

        if (write(fd, &record, sizeof(permisiuni)) != sizeof(permisiuni)) {
            perror("write record");
            result = -1;
            unlock_file(fd);
            }
        }
    }

    // if (unlock_file(fd) == -1) {
    //     result = -1;
    // }

    return result;
}

int process_path(const char *path, int binary_fd) {
    struct stat statbuf;

    if (lstat(path, &statbuf) == -1) {
        fprintf(stderr, "Eroare lstat pentru '%s': %s\n", path, strerror(errno));
        return 6;
    }

    if (update_count(binary_fd, statbuf.st_mode) == -1) {
        fprintf(stderr, "Eroare la actualizarea contorului pentru '%s'\n", path);
    }

    if (S_ISDIR(statbuf.st_mode)) {
        DIR *dirp;
        struct dirent *entry;
        char full_path[PATH_MAX];

        if ((dirp = opendir(path)) == NULL) {
            fprintf(stderr, "Eroare la deschiderea directorului '%s': %s\n", path, strerror(errno));
            return 7;
        }

        while ((entry = readdir(dirp)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            int len = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            if (len < 0 || (size_t)len >= sizeof(full_path)) {
                fprintf(stderr, "Eroare: Calea '%s/%s' este prea lunga.\n", path, entry->d_name);
                continue;
            }
            process_path(full_path, binary_fd);
        }

        if (errno != 0 && entry == NULL) {
             perror("readdir");
        }

        if (closedir(dirp) == -1) {
             perror("close");
        }
    }
    return 0;
}