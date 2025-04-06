#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[], char* env[])
{
   printf("Hello world!\n");
   sleep(3);  // O pauză de 3 secunde, pentru a observa "întârzierea" afișării textului pe ecran, introdusă de bufferul de la nivelul bibliotecii stdio!
   return 0;
}
