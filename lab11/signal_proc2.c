#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t got_input = 0;
volatile sig_atomic_t interval_count = 0;
volatile sig_atomic_t timed_out = 0;
volatile sig_atomic_t child_exited = 0;
volatile pid_t global_child_pid;
volatile int global_N;

static char *child_filename = NULL;

/**
 * @brief Signal handler for SIGALRM.
 *
 * This function is called when an alarm signal (SIGALRM) is received.
 * It checks if any input has been received in the last 'global_N' seconds.
 * If no input was received, it prints a warning message to STDERR and increments 'interval_count'.
 * It resets 'got_input' to 0.
 * If 'interval_count' is less than 5, it schedules another alarm for 'global_N' seconds.
 * Otherwise, it sets 'timed_out' to 1, indicating that the program has timed out due to inactivity.
 *
 * @param sig The signal number (SIGALRM).
 */
void alrm_handler(int sig) {
    if (got_input == 0) {
        char msg[100];
        int len = snprintf(msg, sizeof(msg), "Warning: no input in the last %d seconds\n", global_N);
        write(STDERR_FILENO, msg, len);
        interval_count++;
    }
    got_input = 0;

    if (interval_count < 5) {
        alarm(global_N);
    } else {
        timed_out = 1;
    }
}

/**
 * @brief Signal handler for SIGCHLD.
 *
 * This function is called when a child process changes state (e.g., terminates).
 * It waits for the specific child process identified by 'global_child_pid'.
 * If the waitpid call confirms that the global child process has exited,
 * it sets the 'child_exited' flag to 1.
 *
 * @param sig The signal number (SIGCHLD).
 */
void chld_handler(int sig) {
    pid_t pid = waitpid(global_child_pid, NULL, 0);
    if (pid == global_child_pid) {
        child_exited = 1;
    }
}

/**
 * @brief Signal handler for SIGUSR2 in the child process.
 *
 * This function is called when the child process receives a SIGUSR2 signal.
 * It prints a message to STDOUT indicating that SIGUSR2 was received.
 * It then opens the file specified by 'child_filename' in read-only mode.
 * If opening the file fails, it prints an error and exits.
 * It reads the content of the file in chunks and writes it to STDOUT.
 * If reading or writing fails, it prints an error, closes the file, and exits.
 * After successfully printing the file content, it closes the file,
 * prints a final message to STDOUT, and exits the child process with status 0.
 *
 * @param sig The signal number (SIGUSR2).
 */
void child_usr2_handler(int sig) {
    char msg1[] = "Fiul: am primit USR2!\n";
    write(STDOUT_FILENO, msg1, strlen(msg1));

    int fd = open(child_filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char buf[1024];
    ssize_t n;
    // Read the file in chunks: loop while read() returns > 0
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, n) < 0) {
            perror("write");
            close(fd);
            exit(1);
        }
    }
    if (n < 0) {  // read() error after loop
        perror("read");
        close(fd);
        exit(1);
    }
    close(fd);

    char msg2[] = "Fiul: sfarsit executie!\n";
    write(STDOUT_FILENO, msg2, strlen(msg2));
    exit(0);
}

/**
 * @brief Main function of the program.
 *
 * This program takes a filename and an integer N as command-line arguments.
 * It forks a child process.
 *
 * Parent process:
 * - Sets up signal handlers for SIGALRM (using alrm_handler) and SIGCHLD (using chld_handler).
 * - Opens the specified file for writing (truncating if it exists, creating if not).
 * - Enters a loop reading characters from STDIN and writing them to the opened file.
 * - An alarm is set for N seconds. If no input is received within N seconds, alrm_handler is called.
 * - This alarm mechanism repeats up to 5 times. If no input is received for 5*N seconds, the parent times out.
 * - If STDIN reaches EOF or the parent times out, it closes the output file.
 * - It then sends a SIGUSR2 signal to the child process.
 * - It waits for the child process to exit (indicated by child_exited flag set by chld_handler).
 * - Finally, it prints a message and exits (status 1 if timed out, 0 otherwise).
 *
 * Child process:
 * - Sets 'child_filename' to the filename provided as an argument.
 * - Sets up a signal handler for SIGUSR2 (using child_usr2_handler).
 * - Pauses, waiting for a signal.
 * - When SIGUSR2 is received, child_usr2_handler executes, printing the content of the file to STDOUT and then exiting.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 *             argv[0] is the program name.
 *             argv[1] is the filename.
 *             argv[2] is the integer N (timeout interval).
 * @return 0 on successful completion (if not timed out), 1 on error or timeout.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename N\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];
    int N = atoi(argv[2]);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        child_filename = filename;

        struct sigaction sa;
        sa.sa_handler = child_usr2_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGUSR2, &sa, NULL) < 0) {
            perror("sigaction");
            exit(1);
        }

        pause();
        exit(1);
    }

    global_child_pid = pid;
    global_N = N;

    int out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("open");
        kill(pid, SIGKILL);
        exit(1);
    }

    struct sigaction sa_alrm;
    sa_alrm.sa_handler = alrm_handler;
    sigemptyset(&sa_alrm.sa_mask);
    sa_alrm.sa_flags = 0;
    if (sigaction(SIGALRM, &sa_alrm, NULL) < 0) {
        perror("sigaction");
        close(out_fd);
        kill(pid, SIGKILL);
        exit(1);
    }

    struct sigaction sa_chld;
    sa_chld.sa_handler = chld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = 0;
    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        perror("sigaction");
        close(out_fd);
        kill(pid, SIGKILL);
        exit(1);
    }

    got_input = 0;
    interval_count = 0;
    timed_out = 0;
    child_exited = 0;

    alarm(N);

    char c;
    while (!timed_out) {
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n == 0) {
            break;
        } else if (n < 0) {
            if (errno == EINTR) {
                if (timed_out) {
                    break;
                }
                continue;
            } else {
                perror("read");
                close(out_fd);
                kill(pid, SIGKILL);
                exit(1);
            }
        } else {
            if (write(out_fd, &c, 1) < 0) {
                perror("write");
                close(out_fd);
                kill(pid, SIGKILL);
                exit(1);
            }
            got_input = 1;
        }
    }

    close(out_fd);

    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    sigaction(SIGALRM, &sa_ignore, NULL);

    if (kill(pid, SIGUSR2) < 0) {
        perror("kill");
    }

    while (!child_exited) {
        pause();
    }

    printf("Parinte: sfarsit executie!\n");
    if (timed_out) {
        exit(1);
    } else {
        exit(0);
    }
}
