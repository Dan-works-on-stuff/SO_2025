//
// Created by maxcox on 4/14/25.
//

#define _POSIX_C_SOURCE 200809L // Necesar pentru lstat, fcntl pe unele sisteme
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h> // Pentru PATH_MAX (optional)

#ifndef PATH_MAX
#define PATH_MAX 4096 // O valoare default dacă nu e definit
#endif

// Structura pentru înregistrările din fișierul binar
typedef struct {
    mode_t permissions;       // Masca de permisiuni (ultimii 9-12 biți din st_mode)
    unsigned long long count; // Contorul de apariții
} PermissionRecord;

// --- Funcții pentru blocaje (locking) ---

// Pune un blocaj de scriere pe tot fișierul
int lock_file(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK;    // Blocaj de scriere (exclusiv)
    lock.l_whence = SEEK_SET; // De la începutul fișierului
    lock.l_start = 0;         // Începând de la offset 0
    lock.l_len = 0;           // Până la sfârșitul fișierului (0 înseamnă EOF)
    lock.l_pid = getpid();    // (informativ, setat de sistem oricum)

    // Așteaptă până când blocajul poate fi obținut (F_SETLKW)
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Eroare la blocare fisier (fcntl F_SETLKW)");
        return -1;
    }
    return 0;
}

// Eliberează blocajul de pe fișier
int unlock_file(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;   // Eliberează blocajul
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;          // Pe aceeași regiune ca la blocare
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) { // Folosim SETLKW și pentru unlock
        perror("Eroare la deblocare fisier (fcntl F_UNLCK)");
        // Continuăm chiar dacă deblocarea eșuează, dar semnalăm
        return -1;
    }
    return 0;
}

// --- Funcția de actualizare a contorului în fișierul binar ---

int update_permission_count(int fd, mode_t perms) {
    // Extragem doar biții relevanți de permisiuni (user, group, other)
    mode_t relevant_perms = perms & 0777; // Mască octală pentru rwxrwxrwx
    PermissionRecord record;
    ssize_t bytes_read;
    off_t current_pos;
    int found = 0;
    int result = 0; // 0 = success, -1 = error

    // 1. Obține blocajul exclusiv pe tot fișierul
    if (lock_file(fd) == -1) {
        return -1; // Eroare la blocare
    }

    // 2. Poziționează la începutul fișierului pentru căutare
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("Eroare la repozitionare la inceput (lseek)");
        unlock_file(fd); // Încearcă să deblochezi
        return -1;
    }

    // 3. Caută înregistrarea existentă
    while ((bytes_read = read(fd, &record, sizeof(PermissionRecord))) > 0) {
        if (bytes_read != sizeof(PermissionRecord)) {
            fprintf(stderr, "Eroare: Citire incompleta din fisierul binar.\n");
            result = -1;
            goto cleanup_and_exit; // Mergi la deblocare și ieșire
        }

        if (record.permissions == relevant_perms) {
            // Am găsit înregistrarea
            found = 1;
            record.count++; // Incrementăm contorul

            // Ne poziționăm înapoi la începutul înregistrării curente pentru suprascriere
            current_pos = lseek(fd, 0, SEEK_CUR); // Aflăm poziția curentă
            if (current_pos == (off_t)-1) {
                 perror("Eroare la citirea pozitiei curente (lseek SEEK_CUR)");
                 result = -1;
                 goto cleanup_and_exit;
            }
            if (lseek(fd, current_pos - sizeof(PermissionRecord), SEEK_SET) == (off_t)-1) {
                perror("Eroare la repozitionare pentru scriere (lseek SEEK_SET)");
                result = -1;
                goto cleanup_and_exit;
            }

            // Suprascriem înregistrarea cu contorul actualizat
            if (write(fd, &record, sizeof(PermissionRecord)) != sizeof(PermissionRecord)) {
                perror("Eroare la scrierea inregistrarii actualizate");
                result = -1;
                goto cleanup_and_exit;
            }
            // Am terminat actualizarea, putem ieși din buclă
            break;
        }
    }

    if (bytes_read == -1) {
        perror("Eroare la citirea din fisierul binar in timpul cautarii");
        result = -1;
        goto cleanup_and_exit;
    }

    // 4. Dacă nu am găsit înregistrarea, o adăugăm la sfârșit
    if (!found) {
        // Ne asigurăm că suntem la sfârșitul fișierului (chiar dacă am citit până la EOF)
        if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
             perror("Eroare la pozitionare la sfarsit pentru adaugare (lseek SEEK_END)");
             result = -1;
             goto cleanup_and_exit;
        }

        // Pregătim noua înregistrare
        record.permissions = relevant_perms;
        record.count = 1; // Prima apariție

        // Scriem noua înregistrare
        if (write(fd, &record, sizeof(PermissionRecord)) != sizeof(PermissionRecord)) {
            perror("Eroare la adaugarea inregistrarii noi");
            result = -1;
            // Continuăm spre cleanup chiar dacă scrierea eșuează
        }
    }

cleanup_and_exit:
    // 5. Eliberăm blocajul, indiferent de rezultat
    if (unlock_file(fd) == -1) {
       // Eroarea la deblocare a fost deja semnalată în unlock_file
       result = -1; // Marcăm că a apărut o problemă (chiar dacă operația principală a reușit)
    }

    return result;
}

// --- Funcția recursivă de parcurgere a directoarelor ---

int process_path(const char *path, int binary_fd) {
    struct stat statbuf;

    // 1. Obține informații despre intrare (fișier/director/link)
    // Folosim lstat pentru a NU urmări linkurile simbolice
    if (lstat(path, &statbuf) == -1) {
        fprintf(stderr, "Eroare lstat pentru '%s': %s\n", path, strerror(errno));
        return -1; // Nu putem procesa această intrare
    }

    // 2. Actualizează contorul pentru permisiunile acestei intrări
    // fprintf(stdout, "Procesez: %s (Perms: %o)\n", path, statbuf.st_mode & 0777); // Debug
    if (update_permission_count(binary_fd, statbuf.st_mode) == -1) {
        fprintf(stderr, "Eroare la actualizarea contorului pentru '%s'\n", path);
        // Continuăm parcurgerea, dar semnalăm eroarea la final? Sau oprim?
        // Decidem să continuăm pentru a procesa cât mai mult posibil.
    }

    // 3. Dacă este un director, parcurge-l recursiv
    if (S_ISDIR(statbuf.st_mode)) {
        DIR *dirp;
        struct dirent *entry;
        char full_path[PATH_MAX];

        if ((dirp = opendir(path)) == NULL) {
            fprintf(stderr, "Eroare la deschiderea directorului '%s': %s\n", path, strerror(errno));
            return -1; // Nu putem parcurge acest director
        }

        while ((entry = readdir(dirp)) != NULL) {
            // Ignoră intrările "." și ".." pentru a evita recursivitatea infinită
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // Construiește calea completă pentru intrarea din director
            int len = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            if (len < 0 || (size_t)len >= sizeof(full_path)) {
                fprintf(stderr, "Eroare: Calea '%s/%s' este prea lunga.\n", path, entry->d_name);
                continue; // Treci la următoarea intrare
            }

            // Procesează recursiv intrarea (fișier sau subdirector)
            process_path(full_path, binary_fd);
        }

        if (errno != 0 && entry == NULL) {
             perror("Eroare la citirea directorului");
             // errno este setat de readdir în caz de eroare
        }


        if (closedir(dirp) == -1) {
             perror("Eroare la inchiderea directorului");
             // Continuăm, dar semnalăm.
        }
    }
    // Dacă nu e director, nu mai facem nimic (am procesat deja permisiunile)

    return 0; // Succes pentru procesarea acestei căi (chiar dacă au fost erori în sub-apeluri)
}

// --- Funcția principală ---

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Utilizare: %s <director_start> <fisier_binar_output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *start_dir = argv[1];
    const char *binary_file = argv[2];
    int binary_fd;

    // Deschide (sau creează) fișierul binar pentru citire și scriere
    // O_RDWR: Citire/Scriere
    // O_CREAT: Creează dacă nu există
    // O_SYNC: (Opțional) Forțează scrierile să fie fizice imediat - poate încetini, dar e mai sigur în caz de crash.
    //          Alternativ, se poate folosi fsync() după scrieri importante dacă O_SYNC nu e folosit.
    //          Nu este strict necesar pentru corectitudinea sincronizării cu fcntl, dar ajută la persistență.
    // 0666: Permisiunile default dacă fișierul este creat (poate fi modificat de umask)
    binary_fd = open(binary_file, O_RDWR | O_CREAT /*| O_SYNC*/, 0666);
    if (binary_fd == -1) {
        perror("Nu s-a putut deschide/crea fisierul binar");
        return EXIT_FAILURE;
    }

    // Începe procesarea recursivă din directorul specificat
    printf("Pornesc scanarea directorului: %s\n", start_dir);
    printf("Salvez rezultatele in: %s\n", binary_file);

    if (process_path(start_dir, binary_fd) != 0) {
         fprintf(stderr, "Scanarea s-a terminat cu erori.\n");
         // Nu e neaparat o eroare fatală, unele fișiere/directoare pot fi inaccesibile
    } else {
         printf("Scanare finalizata.\n");
    }


    // Închide fișierul binar
    if (close(binary_fd) == -1) {
        perror("Eroare la inchiderea fisierului binar");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}