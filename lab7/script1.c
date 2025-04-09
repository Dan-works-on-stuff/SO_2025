/*Implementați problema secțiunii critice prezentată în cursul teoretic #5, în scenariul următor:
Se consideră, drept resursă partajabilă de mai multe procese, un fișier binar cu baza de date folosită pentru gestiunea produselor dintr-un magazin.
Înregistrările din acest fișier reprezintă perechi de forma: (cod_produs,stoc), unde cod_produs este un număr unic (i.e., apare o singură dată în baza de date) de tipul int, iar
stoc este un număr de tipul float, reprezentând cantitatea din acel produs (exprimată în unitatea de măsură specifică pentru acel tip de produs, e.g. kilograme, litri, etc.),
disponibilă în acel magazin. Atenție: perechile de numere întregi și reale sunt reprezentate binar, nu textual, în fișierul respectiv!
Asupra acestei baze de date se vor efectua operațiuni de actualizare a stocurilor de produse, conform celor descrise mai jos.
Scrieți un program C care să efectueze diverse operații de vânzare/cumpărare de produse, la intervale variate de timp, operațiile fiind specificate, într-un fișier text cu instrucțiuni,
prin secvențe de forma:
    cod_produs +cantitate   și/sau   cod_produs -cantitate ,
reprezentând cumpărarea și respectiv vânzarea cantității specificate din produsul având codul cod_produs specificat.
Pentru fiecare instrucțiune de cumpărare/vânzare din fișierul de instrucțiuni, programul va căuta în fișierul resursă (i.e., baza de date) specificat înregistrarea cu codul cod_produs
specificat în instrucțiunea respectivă, iar dacă există o astfel de înregistrare, atunci va actualiza valoarea stocului acelui produs în mod corespunzător, dar NUMAI dacă această operație
NU conduce la obținerea unei valori negative pentru stoc, altfel va afișa un mesaj de eroare corespunzător.
Dacă nu există codul cod_produs specificat în instrucțiunea respectivă, iar operația propusă este -cantitate, programul va afișa un mesaj de eroare corespunzător și se va opri din procesarea
fișierului de instrucțiuni. Iar dacă nu există codul cod_produs specificat în instrucțiunea respectivă, însă operația propusă este +cantitate, atunci programul va adăuga o nouă înregistrare,
cu valoarea: (cod_produs,cantitate), în baza de date respectivă. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 40000

int look_for_product(int fd, const char *product_id) {
  char buffer[BUFSIZE];
  int bytes_read;
  char *pos;
  int product_id_len = strlen(product_id);

  while ((bytes_read = read(fd, buffer, BUFSIZE - 1)) > 0) {
    buffer[bytes_read] = '\0'; // Null-terminate the buffer

    // Search within the buffer
    pos = strstr(buffer, product_id);
    while (pos != NULL) {
      if ((pos == buffer || *(pos - 1) == '(') &&
          (pos[product_id_len] == ',')) {
        return 1; // Product ID found
          }
      // Continue searching from the next position after the current match
      pos = strstr(pos + 1, product_id);
    }
  }

  if (bytes_read == -1) {
    perror("read"); // Handle read error
    return -1;
  }
  return 0; // Product ID not found
}


int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  int bd_fd = open(argv[1], O_RDONLY);
  if (bd_fd == -1) {
    perror("open");
  }
  char buf[BUFSIZE];



}