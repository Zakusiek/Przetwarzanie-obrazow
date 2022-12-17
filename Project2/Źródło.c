#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <locale.h>

struct Obraz 
{

	char nazwa[30];
	char typ[3];
	int szerokosc;
	int wysokosc;
	int glebia;
	int** tablica;
};

bool wczytaj(struct Obraz* tab_obrazow, int ilosc)	//do zmiany
{

	char nazwa[30];
	printf("Podaj nazwe pliku zawierajacego obraz:\n");
	scanf(" %s", nazwa);
	FILE* plik = fopen(nazwa, "r");			//"r" oznacza, ze plik do odczytu

	char ch[3];
	int liczba;
	if (plik != NULL) {

		strcpy(tab_obrazow[ilosc].nazwa, nazwa);

		//pobieranie typu
		fscanf(plik, "%s", ch);
		if (ch[0] != 'P' && ch[1] != '2') {
			printf("Nie jest to typ P2");
			return false;
		}
		else {
			strcpy(tab_obrazow[ilosc].typ, strcat(ch, "\0"));
		}

		bool pobrano = false;
		//pobieranie szerokosci
		while (pobrano == false) {
			if (fscanf(plik, "%d", &liczba) == 0) {
				while (fgetc(plik) != '\n') {			//fgetc zwraca kolejny znak w pliku oraz przesuwa wskaznik w pliku o 1 w prawo

				}

			}
			else {
				tab_obrazow[ilosc].szerokosc = liczba;
				pobrano = true;
			}
		}


		//pobieranie wysokosci
		fscanf(plik, "%d", &liczba);
		tab_obrazow[ilosc].wysokosc = liczba;


		pobrano = false;
		//pobnieranie glebi szarosci
		while (pobrano == false) {
			if (fscanf(plik, "%d", &liczba) == 0) {
				while (fgetc(plik) != '\n') {

				}
			}
			else {
				tab_obrazow[ilosc].glebia = liczba;
				pobrano = true;
			}
		}


		//alokacja pamieci
		tab_obrazow[ilosc].tablica = calloc(tab_obrazow[ilosc].wysokosc, sizeof(int*));

		for (int i = 0; i < tab_obrazow[ilosc].wysokosc; i++) {

			tab_obrazow[ilosc].tablica[i] = calloc(tab_obrazow[ilosc].szerokosc, sizeof(int));


			if (tab_obrazow[ilosc].tablica[i] == NULL) {			//zabezpieczenie przed wyciekiem w razie nieudanej alokacji
				for (int j = 0; j <= i; j++) {
					free(tab_obrazow[ilosc].tablica[j]);
				}
				free(tab_obrazow[ilosc].tablica);
				return false;
			}
		}

		int i = 0;
		int j = -1;
		bool liczby_w_linii = false;

		while (fgetc(plik) != EOF) {
			fseek(plik, -1, SEEK_CUR);								//cofam sie o 1 poniewaz fgetc() przesunelo go o 1 w prawo
			if (liczby_w_linii == true) {
				i++;
			}
			if (i == tab_obrazow[ilosc].wysokosc) {
				break;
			}
			liczby_w_linii = false;
			j = -1;
			while (fgetc(plik) != '\n' && fgetc(plik) != EOF && i < tab_obrazow[ilosc].wysokosc && j < tab_obrazow[ilosc].szerokosc) {
				fseek(plik, -2, SEEK_CUR);
				j++;

				if (fscanf(plik, "%d", &liczba) != 0) {

					liczby_w_linii = true;
					tab_obrazow[ilosc].tablica[i][j] = liczba;

					//polykanie spacji gdy jest na koncu linii
					if (fgetc(plik) == ' ') {
						if (fgetc(plik) == '\n') {
							break;
						}
						else {
							fseek(plik, -2, SEEK_CUR);
						}
					}
					else {
						fseek(plik, -2, SEEK_CUR);
						if (fgetc(plik) == '\n') {
							//fseek(plik, -1, SEEK_CUR);
							break;
						}
					}
				}
				else {
					if (liczby_w_linii == false) {
						j = -1;
					}
					while (fgetc(plik) != '\n' && fgetc(plik) != EOF) {
						fseek(plik, -1, SEEK_CUR);
					}
					fseek(plik, -2, SEEK_CUR);
				}

			}

		}
		fclose(plik);

	}
	else {
		printf("Blad otwarcia pliku\n");
		return false;
	}

	return true;
}
void zapisz(struct Obraz* tab_obrazow, int ilosc, int aktywny_obraz)	//dziala lepiej pozmieniac cos
{

	char nazwa[250];
	printf("Podaj nazwe nowego pliku: ");
	scanf("%s", nazwa);

	FILE* plik = fopen(nazwa, "w");				

	fprintf(plik, "%s\n", tab_obrazow[aktywny_obraz].typ);
	fprintf(plik, "%d %d\n", tab_obrazow[aktywny_obraz].szerokosc, tab_obrazow[aktywny_obraz].wysokosc);
	fprintf(plik, "%d\n", tab_obrazow[aktywny_obraz].glebia);


	for (int i = 0; i < tab_obrazow[aktywny_obraz].wysokosc; i++) 
	{
		for (int j = 0; j < tab_obrazow[aktywny_obraz].szerokosc; j++) 
		{
			fprintf(plik, "%d ", tab_obrazow[aktywny_obraz].tablica[i][j]);
		}
		if (i != tab_obrazow[aktywny_obraz].wysokosc - 1) 
		{
			fprintf(plik, "\n");
		}
	}
	fclose(plik);
}
void lista(struct Obraz* tab_obrazow, int* ilosc)	//dziala
{

	printf("Lista obrazow w bazie: \n");
	for (int i = 0; i < (*ilosc); i++) 
	{
		printf("%d. %s\n", i + 1, tab_obrazow[i].nazwa);
	}
	printf("\n");
}
void usun(struct Obraz* tab_obrazow, int* ilosc, int* aktywny_obraz)	//dziala
{
	for (int i = 0; i < tab_obrazow[*aktywny_obraz].wysokosc; i++)
	{
		free(tab_obrazow[*aktywny_obraz].tablica[i]);
	}
	free(tab_obrazow[*aktywny_obraz].tablica);
	for (int i = *aktywny_obraz; i < (*ilosc) - 1; i++)
	{
		tab_obrazow[i] = tab_obrazow[i + 1];
	}
	*ilosc -= 1;
	*aktywny_obraz = -1;

	printf("\nPomyslnie usunieto aktywny obraz\n");
}
void wybierz_aktywny_obraz(int ilosc, int* aktywny_obraz)	//dziala moze zmienic sposob wybierania aktywnego obrazu
{
	int temp;

	printf("Ktory obraz ustawic jako aktywny?: ");
	scanf("%d", &temp);
	if (temp < 1 || temp > ilosc) 
	{
		printf("Wybrano zly obraz\n");
	}
	else 
	{
		*aktywny_obraz = temp - 1;
	}
}
void negatyw(struct Obraz* tab_obrazow, int aktywny_obraz)	//dziala ewentuala zmiana
{
	for (int i = 0; i < tab_obrazow[aktywny_obraz].wysokosc; i++) 
	{
		for (int j = 0; j < tab_obrazow[aktywny_obraz].szerokosc; j++) 
		{
			tab_obrazow[aktywny_obraz].tablica[i][j] = tab_obrazow[aktywny_obraz].glebia - tab_obrazow[aktywny_obraz].tablica[i][j];
		}
	}
}
void pieprz_sol(struct Obraz* tab_obrazow, int aktywny_obraz) 
{
	int ilosc_pikseli = tab_obrazow[aktywny_obraz].szerokosc * tab_obrazow[aktywny_obraz].wysokosc / 10;
	srand(time(NULL));

	for (int n = 0; n < ilosc_pikseli; n++) 
	{
		int i = rand() % tab_obrazow[aktywny_obraz].wysokosc;
		int j = rand() % tab_obrazow[aktywny_obraz].szerokosc;

		if (n % 5 == 0) 
		{											
			tab_obrazow[aktywny_obraz].tablica[i][j] = 0;
		}
		else 
		{
			tab_obrazow[aktywny_obraz].tablica[i][j] = tab_obrazow[aktywny_obraz].glebia;
		}
	}
}
void histogram(struct Obraz* tab_obrazow, int aktywny_obraz)	//git raczej
{

	int* temp_tab = calloc(tab_obrazow[aktywny_obraz].glebia + 1, sizeof(int));
	int temp = 0;

	for (int i = 0; i < tab_obrazow[aktywny_obraz].wysokosc; i++) 
	{
		for (int j = 0; j < tab_obrazow[aktywny_obraz].szerokosc; j++) 
		{
			temp = tab_obrazow[aktywny_obraz].tablica[i][j];
			temp_tab[temp]++;
		}
	}

	char nazwa[30];
	printf("Podaj nazwe pliku do ktorego zapisac histogram: ");
	scanf("%s", nazwa);
	FILE* plik = fopen(nazwa, "w");						

	for (int i = 0; i < tab_obrazow[aktywny_obraz].glebia + 1; i++) 
	{
		fprintf(plik, "%d; %d\n", i, temp_tab[i]);
	}

	fclose(plik);
	free(temp_tab);
}
void obrot(struct Obraz* tab_obrazow, int aktywny_obraz)		//nie dziala
{

}   
bool sprawdz(struct Obraz* tab_obrazow, int *aktywny_obraz, int *ilosc)
{
	if (*aktywny_obraz >= 0 && *aktywny_obraz < *ilosc)
	{
		printf("Aktywny obraz: %d. %s\n", *(aktywny_obraz)+1, tab_obrazow[*aktywny_obraz].nazwa);
		return true;
	}
	else
	{
		printf("Nie wybrano aktywnego obrazu\n\n");
		return false;
	}
}
void gaussFilter(struct Obraz* img)
{
	int** tablicaFiltr = (int**)malloc(img->szerokosc * sizeof(int*));
	for (int i = 0; i < img->szerokosc; i++)
		tablicaFiltr[i] = (int*)malloc(img->wysokosc * sizeof(int));

	// 2D gauss filter
	int mask[3][3] = { {1, 2, 1}, {2, 4, 2}, {1, 2, 1} };
	int sum = 0;
	int i, j, m, n;
	int max = 0;

	for (i = 0; i < img->szerokosc; i++) {
		for (j = 0; j < img->wysokosc; j++) {

			sum = 0;
			for (m = 0; m < 3; m++) {
				for (n = 0; n < 3; n++) {
					int x = i + m - 1;
					int y = j + n - 1;

					if (x >= 0 && x < img->szerokosc && y >= 0 && y < img->wysokosc)
						sum += img->tablica[x][y] * mask[m][n];
				}
			}

			tablicaFiltr[i][j] = sum / 16;
			if (tablicaFiltr[i][j] > max)
				max = tablicaFiltr[i][j];
		}
	}

	for (i = 0; i < img->szerokosc; i++) {
		for (j = 0; j < img->wysokosc; j++) {
			img->tablica[i][j] = tablicaFiltr[i][j] * 255 / max;
		}
	}

	free(tablicaFiltr);
}

int main() 
{

	int ilosc = 0;
	int aktywny_obraz = -1;
	int kat = 0;
	int opcja = 0;
	struct Obraz* tab_obrazow = NULL;


	do {
		printf("%s", "Wybierz akcje: \n1. Wczytaj obraz\n2. Pokaz liste obrazow\n3. Wybierz aktywny obraz\n4. Usun aktywny obraz z listy\n5. Obroc aktywny obraz\n6. Negatyw aktywnego obrazu\n7. Histogram\n8. Pieprz i sol\n9. Przeprowadz filtracje Gaussa na aktywnym obrazie\n10. Zapisz aktywny obraz\nWybierz inna liczbe by zakonczyc program\n\n");
		sprawdz(tab_obrazow, &aktywny_obraz, &ilosc);

		scanf("%d", &opcja);
		switch (opcja) 
		{
		case 1:
			ilosc++;
			tab_obrazow = realloc(tab_obrazow, ilosc * sizeof(struct Obraz));

			if (tab_obrazow == NULL) 
			{
				printf("Blad");
				return 1;
			}

			if (wczytaj(tab_obrazow, ilosc - 1) == false) 
			{
				ilosc = ilosc - 1;
				tab_obrazow = realloc(tab_obrazow, ilosc * sizeof(struct Obraz));
				printf("Blad podczas wczytywania obrazu\n");
			}
			break;

		case 2:
			lista(tab_obrazow, &ilosc);
			break;

		case 3:
			lista(tab_obrazow, &ilosc);
			wybierz_aktywny_obraz(ilosc, &aktywny_obraz);
			break;

		case 4:

			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				usun(tab_obrazow, &ilosc, &aktywny_obraz);
				tab_obrazow = realloc(tab_obrazow, ilosc * sizeof(struct Obraz));
				if (tab_obrazow == NULL && ilosc != 0) 
				{
					printf("Blad alokacji, przerwanie dzialania");
					return 1;
				}
			}
			break;

		case 5:
			printf("O ile stopni obrocic obraz w prawo? (k*90): ");
			scanf("%d", &kat);
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				for (int i = 0; i < kat / 90; i++) 
				{
					obrot(tab_obrazow, aktywny_obraz);
				}
			}
			break;

		case 6:
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				negatyw(tab_obrazow, aktywny_obraz);
			}
			break;

		case 7:
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				histogram(tab_obrazow, aktywny_obraz);
			}
			break;

		case 8:
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				pieprz_sol(tab_obrazow, aktywny_obraz);
			}
			break;
		case 9:
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				gaussFilter(aktywny_obraz);
			}
		break;
		case 10:
			if (sprawdz(tab_obrazow, &aktywny_obraz, &ilosc) == true)
			{
				zapisz(tab_obrazow, ilosc, aktywny_obraz);
			}
			break;

		default:
			printf("Koniec programu");
			for (int i = 0; i < ilosc; i++)
			{
				usun(tab_obrazow, &ilosc, &i);
			}
			free(tab_obrazow);
			return 1;
		}
	} while (1);
}