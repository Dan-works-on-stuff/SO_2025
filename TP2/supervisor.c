// Programul va primi un argument în linia de comandă, ce reprezintă calea (absolută sau relativă) către un fișier
// existent pe disc, numit “input_data.txt”. Acest fișier conține o secvență de numere întregi, în format textual.
// i) În funcția principală a programului, acesta va testa existența unui argument primit în linia de comandă și va
// face inițializările necesare pentru a putea trimite informații procesului worker1 printr-un canal anonim (prin
// comunicații unu-la-unu) și, respectiv, pentru a putea primi rezultate de la procesul worker3 printr-un obiect de
// memorie partajată – o mapare nepersistentă cu nume, creată cu funcția shm_open. De asemenea, tot în funcția
// principală a programului, programul va crea un proces fiu, iar în fiul creat va starta, printr-un apel exec adecvat,
// programul worker1.
// ii) Într-o funcție separată, apelată din funcția main, programul va citi pe rând, unul câte unul, fiecare număr din
// acel fișier “input_data.txt” și-l va converti la reprezentarea în format binar a numerelor întregi, iar rezultatul
// conversiei îl va transmite către procesul worker1 prin acel canal anonim, folosind apeluri POSIX (așadar,
// folosind reprezentarea binară a numerelor întregi, le veți transmite prin mesaje de lungime constantă).
// iii) Într-o altă funcție separată, apelată din funcția main, programul va citi, folosind apeluri POSIX, numerele
// transmise lui de către procesul worker3 (descris mai jos) prin intermediul acelei mapări nepersistente cu nume și
// le va aduna. Rezultatul acestei adunări va fi convertit la reprezentarea textuală în baza 10 a numerele întregi și
// apoi se va calcula suma cifrelor acestei reprezentări, afișând-o la final pe ecran.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define handle_error(msg) \
{ perror(msg); exit(EXIT_FAILURE); }

#define SHM_NAME "/tp2_shm_supervisor_worker3"
#define MAX_NUMBERS 100
#define SHM_SIZE ((MAX_NUMBERS + 1) * sizeof(int))

void send_data_to_worker1(const char *input_file_path, int pipe_write_fd) {
    FILE *file = fopen(input_file_path, "r");
    if (!file) {
        handle_error("fopen input_data.txt");
    }

    int num;
    while (fscanf(file, "%d", &num) == 1) {
        if (write(pipe_write_fd, &num, sizeof(int)) != sizeof(int)) {
            perror("write to pipe for worker1");
        }
    }

    if (ferror(file)) {
        perror("fscanf from input_data.txt");
    }

    fclose(file);
    close(pipe_write_fd); // EOF --> WORKER1
}

void receive_and_process_from_worker3() {
    int shm_fd;
    int *shm_ptr;
    int total_sum = 0;

    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        handle_error("shm_open (supervisor read)");
    }

    shm_ptr = (int *)mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        close(shm_fd);
        handle_error("mmap (supervisor)");
    }
    close(shm_fd);

    int count = shm_ptr[0];
    printf("Supervisor: Worker3 sent %d numbers.\n", count); // testing

    for (int i = 0; i < count; ++i) {
        total_sum += shm_ptr[i + 1]; // Numbers start from index 1
        printf("Supervisor: Read %d from shm.\n", shm_ptr[i+1]); // testing
    }

    printf("Supervisor: Total sum from worker3: %ld\n", total_sum); // testing

    char sum_str[15];
    sprintf(sum_str, "%ld", total_sum);

    int digit_sum = 0;
    for (int i = 0; sum_str[i] != '\0'; ++i) {
        if (sum_str[i] >= '0' && sum_str[i] <= '9') {
            digit_sum += sum_str[i] - '0';
        }
    }

    printf("Suma cifrelor rezultatului final: %d\n", digit_sum);

    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("munmap (supervisor)");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <cale_fisier_input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *input_file = argv[1];

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        handle_error("pipe");
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        handle_error("shm_open (supervisor create)");
    }
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        close(shm_fd);
        handle_error("ftruncate");
    }
    close(shm_fd);

    pid_t pid = fork();
    if (pid == -1) {
        handle_error("fork");
    }

    if (pid == 0) { // copil (worker1)
        close(pipe_fd[1]);
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            handle_error("dup2 stdin for worker1");
        }
        close(pipe_fd[0]);

        execl("./worker1", "worker1", (char *)NULL);
        handle_error("execl worker1");
    }
    // Parent
    close(pipe_fd[0]);

    send_data_to_worker1(input_file, pipe_fd[1]);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        handle_error("waitpid for worker1 chain");
    }

    receive_and_process_from_worker3();

    return 0;
}
