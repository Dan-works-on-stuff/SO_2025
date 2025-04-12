/*
   Iată o soluție incompletă, în sensul că s-a omis codul efectiv de punere și de scoatere a lacătului pentru acces exclusiv la fișierul de date partajat,
   precum și tratarea anumitor excepții (i.e., atunci când se încearcă extragerea unei cantități mai mari decât cea disponibilă în rezervorul stației PECO).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void initializare_fisier_date(char *filename) {
	int fd= creat(filename, 0600);
	if (fd==-1) {
		perror("creat failed");
	}
	srandom( getpid() );
	float qty=random()%100;
	if (-1== write (fd, &qty, sizeof(int))) {
		perror("write failed");
	}
	close(fd);
}
/*
*  Pentru început, avem nevoie de o rutină ajutătoare, care să creeze conținutul inițial al fișierului de date partajat.
*  Se va crea fișierul și va conține un număr real, stocat în format binar, reprezentând cantitatea inițială de combustibil din rezervor.
*  Vom apela programul cu opțiunea -i pentru a invoca această rutină de inițializare.
*/

void afisare_fisier_date(char *filename) {
	float qty;
	int bytes_read;
	int fd = open(filename, O_RDONLY);
	struct flock lock;
	if (fd==-1) {
		perror("open failed");
		exit(1);
	}
	lock.l_type   = F_RDLCK;
	lock.l_whence = SEEK_CUR;
	lock.l_start  = SEEK_SET;
	lock.l_len    = SEEK_END;
	if(read(fd, &qty, sizeof(int)) == -1){
		perror("read failed");
		exit(2);
	}
	printf("%f\n", qty);
	close(fd);
}
/*
*  Similar, la finalul tuturor actualizărilor, vom avea nevoie de o altă rutină ajutătoare, care să ne afișeze conținutul final din rezervor.
*  Vom apela programul cu opțiunea -o pentru a invoca această rutină de afișare finală.
*/

void secventa_de_actualizari(int n, char* nr[]) ;
/*
*  Rutina principală a programului, care va executa secvența de actualizări primită în linia de comandă, în manieră concurentă,
*  conform celor specificate în enunțul problemei.
*  Observație: rutina principală nu este implementată în întregime mai jos, ci vă rămâne dvs. sarcina să completați "bucățile" de cod
*  care lipsesc (indicate prin comentarii TODO).
*/

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "Eroare: programul trebuie apelat cu optiunile: -i, -o, sau cu o secventa de numere reale...\n");  return 1;
	}

	/* Cazul de inițializare a fișierului de date. */
	if( strcmp(argv[1],"-i") == 0 )
	{
		initializare_fisier_date("peco.bin");
		return 0;
	}

	/* Cazul de afișare a fișierului de date. */
	if( strcmp(argv[1],"-o") == 0 )
	{
		afisare_fisier_date("peco.bin");
		return 0;
	}

	/* Cazul general: actualizarea fișierului de date. */
	secventa_de_actualizari(argc, argv);
	return 0;
}


void secventa_de_actualizari(int n, char* nr[])
{ /* Funcționalitate: realizarea secvenței de operații de actualizare a fișierului de date. */

	float valoare, stoc;
	int fd,i;

	if ( -1 == (fd = open("peco.bin", O_RDWR) ) )
	{
		perror("Eroare la deschiderea pentru actualizare a fisierului de date...");
		exit(6);
	}

	srandom( getpid() ); //  Inițializarea generatorului de numere aleatoare.

	for(i = 1; i < n; i++) // Iterăm prin secvența de numere reale specificată în linia de comandă.
	{
		if(1 != sscanf(nr[i], "%f", &valoare))  // Facem conversie de la string la float.
		{
			fprintf(stderr, "[PID: %d] Eroare: ati specificat o valoare invalida drept argument de tip numar real : %s !\n", getpid(), nr[i] );
			exit(7);
		}

		/* Începutul secțiunii critice! */
		struct flock lock;
		lock.l_type=F_WRLCK;
		lock.l_whence=SEEK_CUR;
		lock.l_start=SEEK_SET;
		lock.l_len= SEEK_END;

		if(-1 == lseek(fd, 0, SEEK_SET) ) // (Re)poziționăm cursorul la începutul fișierului.
		{
			perror("Eroare la repozitionarea in fisierul de date, pentru citire...");  exit(8);
		}
		if (-1 == read(fd, &stoc, sizeof(float) ) )
		{
			perror("Eroare la citirea valorii din fisierul de date...");  exit(9);
		}

		printf("[PID: %d] Se adauga/extrage in/din rezervor cantitatea de %f litri de combustibil.\n", getpid(), valoare );

		stoc = stoc + valoare;
		if (stoc < 0) {
			float surplus = stoc;
			stoc=0;
			printf("Cantitatea ceruta nu a putut fi supplied, alimentat doar %f", valoare+surplus);
		}
		// Varianta 2: realizarea unei extrageri parțiale, i.e. se extrage doar cât e în stoc, nu cât s-a cerut, cu un mesaj informativ în acest sens.

		if(-1 == lseek(fd, 0, SEEK_SET) ) // Repoziționăm cursorul la începutul fișierului.
		{
			perror("Eroare la repozitionarea in fisierul de date, pentru scriere...");  exit(10);
		}
		if (-1 == write(fd, &stoc, sizeof(float) ) )
		{
			perror("Eroare la scrierea valorii actualizate in fisierul de date..."); exit(11);
		}

		/* Sfârșitul secțiunii critice! */
		lock.l_type=F_UNLCK;
		if (fcntl(fd, F_SETLK, &lock) == -1) {
			perror("deblocare");
		}
		sleep( random() % 5 ); // Facem o pauză aleatoare, de maxim 4 secunde, înainte de a continua cu următoarea iterație.
	}

	close(fd);
	printf("[PID: %d] Am terminat de procesat secventa de actualizari.", getpid());
}
