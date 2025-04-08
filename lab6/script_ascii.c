#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define BUFFER_MAX 1024
#define ASCII_MAX 256
int fr[ASCII_MAX];
int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Usage: script_ascii <input file>\n");
  }
  char *input_file = argv[1];
  int input_fd = open(input_file, O_RDONLY);
  if(input_fd == -1){
    perror("open");
  }
  char buffer[BUFFER_MAX];
  ssize_t bytes_read;
  while((bytes_read = read(input_fd, buffer, BUFFER_MAX)) > 0){
    for(int i = 0; i < bytes_read; i++){
      fr[buffer[i]]++;
    }
  }
  for(int i = 0; i < ASCII_MAX; i++){
    if(fr[i] > 0){
      printf("%c%d\n", i, fr[i]);
    }
  }
}