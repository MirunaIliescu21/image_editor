// Copyright Iliescu Miruna-Elena 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define VALMAX 255
#define DMAX 100
#define DIM 256
#define NMAX 3

// Structura pentru retinerea valorilor unui pixel color
typedef struct {
	int r, g, b;  // Valori pentru culorile rosu, verde, albastru
} color_rgb;

// Structura pentru retinerea datelor unei imagini
typedef struct {
	int verif_incarcare; // Verifica daca a fost incarcata deja o imagine
	char magic[3];  // Cuvant magic (P2, P3, P5, P6)
	int latime, inaltime;  // Dimensiunile imaginii
	int val_max;  // Valoarea maxima a culorilor (doar pentru P2, P3)
	int **mat_gri;  // Matrice pentru tonuri de gri (P2, P5)
	color_rgb **mat_color;  // Matrice pentru imagini color (P3, P6)
	int x[2], y[2]; // Coordonatele de selectie
} imagine;

// Functie pentru eliberare matrice gri
void free_gri(int **matrice, int inaltime);

// Functie pentru eliberare matrice color
void free_color(color_rgb **matrice, int inaltime);

// Functia elibereaza imaginea din memorie
void eliberare(imagine *img)
{
	if (img->verif_incarcare == 1) {
		if (img->magic[1] == '2' || img->magic[1] == '5')
			free_gri(img->mat_gri, img->inaltime);
		else if (img->magic[1] == '3' || img->magic[1] == '6')
			free_color(img->mat_color, img->inaltime);

		// Se seteaza toate elementele din structura la 0 folosind memset
		memset(img, 0, sizeof(imagine));
	}
}

// Functie pentru eliberare matrice gri
void free_gri(int **matrice, int inaltime)
{
	for (int i = 0; i < inaltime; i++)
		free(matrice[i]);
	free(matrice);
}

// Functie pentru eliberare matrice color
void free_color(color_rgb **matrice, int inaltime)
{
	for (int i = 0; i < inaltime; i++)
		free(matrice[i]);
	free(matrice);
}

// Functie pentru alocarea memoriei pentru matricea de pixeli gri
int **alocare_gri(int inaltime, int latime, imagine *img)
{
	int **matrice;
	matrice = (int **)malloc(inaltime * sizeof(int *));
	if (!matrice) {
		fprintf(stderr, "Eroare alocare memorie\n");
		eliberare(img);
		exit(EXIT_FAILURE);
	}
	int i = 0;
	while (i < inaltime) {
		matrice[i] = (int *)malloc(latime * sizeof(int));
		if (!matrice[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = 0; j <= i; j++)
				free(matrice[j]);
			eliberare(img);
			exit(EXIT_FAILURE);
		}
		i++;
	}

	return matrice;
}

// Functie pentru alocarea memoriei pentru matricea de pixeli color
color_rgb **alocare_color(int inaltime, int latime, imagine *img)
{
	color_rgb **matrice = (color_rgb **)malloc(inaltime * sizeof(color_rgb *));
	if (!matrice) {
		fprintf(stderr, "Eroare alocare memorie\n");
		eliberare(img);
		exit(EXIT_FAILURE);
	}
	int i = 0;
	while (i < inaltime) {
		matrice[i] = (color_rgb *)malloc(latime * sizeof(color_rgb));
		if (!matrice[i]) {
			for (int j = 0; j <= i; j++)
				free(matrice[j]);
			fprintf(stderr, "Eroare alocare memorie\n");
			eliberare(img);
			exit(EXIT_FAILURE);
		}
		i++;
	}

	return matrice;
}

// Functie pentru eliminarea comentariilor din fisier
void fara_comentarii(FILE *fisier)
{
	char c;
	fscanf(fisier, "%c", &c);

	if (c == '#')
		// Comentariu, se ignora linia
		while (c != '\n' && c != EOF)
			// Continua citirea comentariului
			fscanf(fisier, "%c", &c);
	else
		// Nu este un comentariu, revenim la caracterul citit
		fseek(fisier, -1, SEEK_CUR);
}

// Functie pentru operatia LOAD, citirea unei imagini din fisier
void incarcare_imagine(imagine *img, const char *nume_fisier)
{
	if (img->verif_incarcare == 1)
		eliberare(img); // Se sterge orice imagine incarcata
	FILE *fisier = fopen(nume_fisier, "rt"); // Deschide fisierul in modul text
	if (!fisier) {
		printf("Failed to load %s\n", nume_fisier);
		eliberare(img);
		return;
	}
	fara_comentarii(fisier); // Eliminarea posibilelor comentarii
	fscanf(fisier, "%s", img->magic); fara_comentarii(fisier); // Cuv magic
	fscanf(fisier, "%d", &img->latime); // Dimensiuni imag
	img->x[0] = 0; img->x[1] = img->latime;
	fscanf(fisier, "%d", &img->inaltime); fara_comentarii(fisier);
	img->y[0] = 0; img->y[1] = img->inaltime;
	int w = img->latime, h = img->inaltime;
	fscanf(fisier, "%d", &img->val_max); fara_comentarii(fisier); // Val maxima
	if (img->magic[1] == '2' || img->magic[1] == '5') { // P2, P5 - Grayscale
		img->verif_incarcare = 1;
		img->mat_gri = alocare_gri(img->inaltime, img->latime, img);
	} else if (img->magic[1] == '3' || img->magic[1] == '6') {// P3, P6 - Color
		img->verif_incarcare = 1;
		img->mat_color = alocare_color(img->inaltime, img->latime, img);
	}
	if (img->magic[1] == '2') {
		int i = 0; // P2 Grayscale ASCII MODIFICAT SI NESALVAT
		while (i < h) {
			int j = 0;
			while (j < w) {
				fscanf(fisier, "%d", &img->mat_gri[i][j]);
				j++;
			}
			i++;
		}
	} else if (img->magic[1] == '3') {
		int i = 0; // P3 Color ASCII MODIFICAT SI NESALVAT
		while (i < h) {
			int j = 0;
			while (j < w) {
				fscanf(fisier, "%d%d%d", &img->mat_color[i][j].r,
					   &img->mat_color[i][j].g, &img->mat_color[i][j].b);
				j++;
			}
			i++;
		}
	} else {
		long pozitie = ftell(fisier); fclose(fisier);// Retine pozitia
		fisier = fopen(nume_fisier, "rb"); // Deschide fisierul in modul binar
		fseek(fisier, pozitie + 1, SEEK_SET); // Adauga 1 la pozitia retinuta
		if (img->magic[1] == '5') {
			unsigned char byte;
			int i = 0; // P5 Grayscale binar
			while (i < h) {
				int j = 0;
				while (j < w) {
					fread(&byte, sizeof(unsigned char), 1, fisier);
					img->mat_gri[i][j] = (int)byte;
					j++;
				}
				i++;
			}
		} else if (img->magic[1] == '6') {
			unsigned char r, g, b;
			int i = 0; // P6 Color binar
			while (i < h) {
				int j = 0; // Citire si atribuire pt fiecare pixel de culoare
				while (j < w) {
					fread(&r, sizeof(unsigned char), 1, fisier);
					img->mat_color[i][j].r = (int)r;
					fread(&g, sizeof(unsigned char), 1, fisier);
					img->mat_color[i][j].g = (int)g;
					fread(&b, sizeof(unsigned char), 1, fisier);
					img->mat_color[i][j].b = (int)b;
					j++;
				}
				i++;
			}
		}
	}
	printf("Loaded %s\n", nume_fisier); fclose(fisier);
}

// Functie pentru operatia SAVE - fisier tip ASCII
void salvare_ascii(const imagine *img, const char *nume_fisier)
{
	FILE *fisier = fopen(nume_fisier, "w");
	if (!fisier) {
		fprintf(stderr, "Failed to open file for saving: %s\n", nume_fisier);
		return;
	}
	int h = img->inaltime;
	int w = img->latime;
	if (img->magic[1] == '2' || img->magic[1] == '5') {
		// P2, P5 - imagine Grayscale
		// Scrie magic word si dimensiunile imaginii
		fprintf(fisier, "%s\n", "P2");
		fprintf(fisier, "%d %d\n", img->latime, img->inaltime);
		fprintf(fisier, "%d\n", img->val_max);

		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++)
				fprintf(fisier, "%d ", img->mat_gri[i][j]);
			fprintf(fisier, "\n");
		}
	} else if (img->magic[1] == '3' || img->magic[1] == '6') {
		// P3, P6 - imagine color
		// Scrie magic word si dimensiunile imaginii
		fprintf(fisier, "%s\n", "P3");
		fprintf(fisier, "%d %d\n", img->latime, img->inaltime);
		fprintf(fisier, "%d\n", img->val_max);

		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				fprintf(fisier, "%d %d %d ", img->mat_color[i][j].r,
						img->mat_color[i][j].g, img->mat_color[i][j].b);
			}
			fprintf(fisier, "\n");
		}
	}
	fclose(fisier);
}

// Functie pentru operatia SAVE - fisier tip binar
void salvare_binar(const imagine *img, const char *nume_fisier)
{
	FILE *fisier = fopen(nume_fisier, "wb");
	if (!fisier) {
		fprintf(stderr, "Failed to open file for saving: %s\n", nume_fisier);
		return;
	}
	int h = img->inaltime;
	int w = img->latime;
	if (img->magic[1] == '2' || img->magic[1] == '5') {
		// P2, P5 - imagine Grayscale
		// Scrie magic wordul
		fprintf(fisier, "%s\n", "P5");
	} else if (img->magic[1] == '3' || img->magic[1] == '6') {
		// P3, P6 - imagine color
		// Scrie magic wordul
		fprintf(fisier, "%s\n", "P6");
	}

	fprintf(fisier, "%d %d\n", img->latime, img->inaltime);
	fprintf(fisier, "%d\n", img->val_max);

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (img->magic[1] == '2' || img->magic[1] == '5') {
				// imagine Grayscale
				fwrite(&(unsigned char){img->mat_gri[i][j]},
					   sizeof(unsigned char), 1, fisier);
			} else if (img->magic[1] == '3' || img->magic[1] == '6') {
				// imagine color
				fwrite(&(unsigned char){img->mat_color[i][j].r},
					   sizeof(unsigned char), 1, fisier);
				fwrite(&(unsigned char){img->mat_color[i][j].g},
					   sizeof(unsigned char), 1, fisier);
				fwrite(&(unsigned char){img->mat_color[i][j].b},
					   sizeof(unsigned char), 1, fisier);
			}
		}
	}
	fclose(fisier);
}

// Functia SAVE
// cmd2 = nume fisier
void salvare(const imagine *img, char input[])
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	char cmd[DMAX], cmd2[DMAX], cmd3[DMAX];
	int binar; // Variabila care ne indica modul in care va fi salvata imaginea
	int num_args = sscanf(input, "%s %s %s", cmd, cmd2, cmd3);
	if (num_args == 3 && strcmp(cmd3, "ascii") == 0)
		binar = 0;
	else
		binar = 1;

	char nume_fisier[DMAX];
	strcpy(nume_fisier, cmd2);

	if (binar == 0)
		salvare_ascii(img, nume_fisier);
	else
		salvare_binar(img, nume_fisier);

	printf("Saved %s\n", nume_fisier);
}

// Functie care interschimba doua valori
void interschimba(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

// Functie care verifica daca un sir de caractere este numar intreg
int este_numar(const char *sir)
{
	// Verifica daca sirul incepe cu un semn plus sau minus
	if (*sir == '+' || *sir == '-')
		sir++;

	// Verifica daca cel putin un caracter este prezent
	if (*sir == '\0')
		return 0; // Sirul este gol, nu este un numar intreg

	while (*sir) {
		// Verifica daca caracterul curent nu este o cifra
		if (*sir < '0' || *sir > '9')
			return 0; // Nu este un numar intreg
		sir++;
	}

	return 1; // Este un numar intreg
}

// Functie care verifica validitatea coordonatelor pentru selectie
bool sunt_coordonate_valide(int x1, int x2, int y1, int y2, imagine *img)
{
	return (x1 >= 0 && x1 < img->latime &&
			x2 > 0 && x2 <= img->latime &&
			y1 >= 0 && y1 < img->inaltime &&
			y2 > 0 && y2 <= img->inaltime &&
			x1 != x2 && y1 != y2);
}

// Functie pentru operatia SELECT <x1> <y1> <x2> <y2>
void selectare(imagine *img, char input[])
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	char cmd[DMAX], cmd2[DMAX], cmd3[DMAX], cmd4[DMAX], cmd5[DMAX];
	input[strcspn(input, "\n")] = '\0';
	int num_args;
	num_args = sscanf(input, "%s %s %s %s %s", cmd, cmd2, cmd3, cmd4, cmd5);
	// Verific daca s-au introdus suficienti parametri pt realizarea comenzii
	if (num_args != 5) {
		printf("Invalid command\n");
		return;
	}
	// Verifica daca cmd2, cmd3, cmd4 si cmd5 sunt numere intregi
	if (!este_numar(cmd2) || !este_numar(cmd3) ||
		!este_numar(cmd4) || !este_numar(cmd5)) {
		printf("Invalid command\n");
		return;
	}
	int x1, y1, x2, y2;
	// Converteste un sir de caractere la o valore int
	x1 = atoi(cmd2);
	y1 = atoi(cmd3);
	x2 = atoi(cmd4);
	y2 = atoi(cmd5);

	// Se ordoneaza intervalele x1, x2 si y1, y2
	if (x1 > x2)
		interschimba(&x1, &x2);
	if (y1 > y2)
		interschimba(&y1, &y2);
	// Se verifica conditiile
	if (sunt_coordonate_valide(x1, x2, y1, y2, img)) {
		// Daca coordonatele sunt valide se retin valorile citite
		img->x[0] = x1;
		img->y[0] = y1;
		img->x[1] = x2;
		img->y[1] = y2;
	} else {
		printf("Invalid set of coordinates\n");
		return;
	}
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

// Functie pentru operatia SELECT ALL
void selectare_totala(imagine *img)
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	img->x[0] = 0;
	img->y[0] = 0;
	img->x[1] = img->latime;
	img->y[1] = img->inaltime;
	printf("Selected ALL\n");
}

// Functie care actualizeaza dimensiunile
void actualizeaza_dimensiuni(imagine *img, int w, int h)
{
	img->x[0] = 0;
	img->y[0] = 0;
	img->latime = w;
	img->inaltime = h;
	img->x[1] = w;
	img->y[1] = h;
}

// Functie pentru operatia CROP pt matrice gri
void taie_gri(imagine *img)
{
	int **matrice;
	int h, w; // noua inaltime si latime a matricei
	h = img->y[1] - img->y[0];
	w = img->x[1] - img->x[0];
	matrice = alocare_gri(h, w, img);

	// Copiere selectiei
	int y0 = img->y[0];
	int x0 = img->x[0];
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			matrice[i][j] = img->mat_gri[i + y0][j + x0];
	free_gri(img->mat_gri, img->inaltime);

	// Recopierea selectiei in matricea initiala
	img->mat_gri = alocare_gri(h, w, img);
	int i = 0;
	while (i < h) {
		int j = 0;
		while (j < w) {
			img->mat_gri[i][j] = matrice[i][j];
			j++;
		}
		i++;
	}
	// Actualizarea dimensiunilor
	actualizeaza_dimensiuni(img, w, h);
	// Eliberarea matricei copie
	free_gri(matrice, h);
	printf("Image cropped\n");
}

// Functie pentru operatia CROP pt matrice color
void taie_color(imagine *img)
{
	color_rgb **matrice;
	int h, w; // noua inaltime si latime a matricei
	h = img->y[1] - img->y[0];
	w = img->x[1] - img->x[0];
	matrice = alocare_color(h, w, img);

	// Copiere selectiei
	int y0 = img->y[0];
	int x0 = img->x[0];
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			matrice[i][j].r = img->mat_color[i + y0][j + x0].r;
			matrice[i][j].g = img->mat_color[i + y0][j + x0].g;
			matrice[i][j].b = img->mat_color[i + y0][j + x0].b;
		}

	free_color(img->mat_color, img->inaltime);

	// Recopierea selectiei in matricea initiala
	img->mat_color = alocare_color(h, w, img);
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			img->mat_color[i][j].r = matrice[i][j].r;
			img->mat_color[i][j].g = matrice[i][j].g;
			img->mat_color[i][j].b = matrice[i][j].b;
		}
	// Actualizarea dimensiunilor
	actualizeaza_dimensiuni(img, w, h);
	// Eliberarea matricei copieiliare
	free_color(matrice, h);
	printf("Image cropped\n");
}

// Functie pentru operatia CROP
void taiere(imagine *img)
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->magic[1] == '2' || img->magic[1] == '5') {
		// P2, P5 - Grayscale
		taie_gri(img);
	} else if (img->magic[1] == '3' || img->magic[1] == '6') {
		// P3, P6 - Color
		taie_color(img);
	}
}

// Functie pt realizea operatiei CLAMP
int clamp(double x)
{
	if (x < 0)
		x = 0;
	else if (x > 255)
		x = 255;
	return (int)x;
}

// Functie pentru alocarea dinamica a unei matrici de tip double
double **alocare_double(int linii, int coloane, imagine *img)
{
	double **matrice;

	matrice = (double **)malloc(linii * sizeof(double *));
	if (!matrice) {
		fprintf(stderr, "Eroare alocare memorie\n");
		eliberare(img);
		exit(EXIT_FAILURE);
	}
	int i = 0;
	while (i < linii) {
		matrice[i] = (double *)malloc(coloane * sizeof(double));
		if (!matrice[i]) {
			for (int j = 0; j <= i; j++)
				free(matrice[j]);
			fprintf(stderr, "Eroare alocare memorie\n");
			eliberare(img);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	return matrice;
}

// Functie pentru initializarea nucleului filtrului selectat
void init_nucleu(double **matrice, int tip)
{
	const double edge_nucleu[3][3] = { {-1, -1, -1}, {-1, 8, -1},
										{-1, -1, -1} };
	const double sharpen_nucleu[3][3] = { {0, -1, 0}, {-1, 5, -1},
											{0, -1, 0} };
	const double blur_nucleu[3][3] = { {1.0 / 9, 1.0 / 9, 1.0 / 9},
										{1.0 / 9, 1.0 / 9, 1.0 / 9},
										{1.0 / 9, 1.0 / 9, 1.0 / 9} };
	const double gaussian_blur_nucleu[3][3] = { {1.0 / 16, 1.0 / 8, 1.0 / 16},
												{1.0 / 8, 1.0 / 4, 1.0 / 8},
												{1.0 / 16, 1.0 / 8, 1.0 / 16} };

	const double (*nucleu_selectat)[3] = NULL;

	switch (tip) {
	case 0:
		nucleu_selectat = edge_nucleu;
		break;
	case 1:
		nucleu_selectat = sharpen_nucleu;
		break;
	case 2:
		nucleu_selectat = blur_nucleu;
		break;
	case 3:
		nucleu_selectat = gaussian_blur_nucleu;
		break;
	default:
		// Trateaza cazul în care valoarea "tipul" nu este valid
		printf("APPLY parameter invalid\n");
		return;
	}

	// Copiaza nucleul selectat in matrice
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			matrice[i][j] = nucleu_selectat[i][j];
}

// Functia alege filtrul care se va aplica imaginii
int alege_filtru(char filtru[])
{
	int tip_filtru = -1;
	if (strcmp(filtru, "EDGE") == 0)
		tip_filtru = 0;

	if (strcmp(filtru, "SHARPEN") == 0)
		tip_filtru = 1;

	if (strcmp(filtru, "BLUR") == 0)
		tip_filtru = 2;

	if (strcmp(filtru, "GAUSSIAN_BLUR") == 0)
		tip_filtru = 3;

	return tip_filtru;
}

/* Functie pentru aplicarea unei operatii pe fiecare pixel din matrice
	(excluzand marginile) */
void aplicare_operatie(imagine *img, color_rgb **c, double **ker, int i, int j)
{
	double copie_r = 0, copie_b = 0, copie_g = 0;
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			copie_r += img->mat_color[i - 1 + x][j - 1 + y].r * ker[x][y];
			copie_g += img->mat_color[i - 1 + x][j - 1 + y].g * ker[x][y];
			copie_b += img->mat_color[i - 1 + x][j - 1 + y].b * ker[x][y];
		}
	}
	c[i][j].r = clamp(round(copie_r));
	c[i][j].g = clamp(round(copie_g));
	c[i][j].b = clamp(round(copie_b));
}

// Functie pt aplicarea filtrului selectat
void aplicare_filtru(imagine *img, char tip_filtru)
{
	// Se realizeaza o copie a imaginii initiale
	int ***copie;
	copie = (int ***)calloc(img->inaltime, sizeof(int **));
	for (int i = 0; i < img->inaltime; i++) {
		copie[i] = (int **)calloc(img->latime, sizeof(int *));
		for (int j = 0; j < img->latime; j++)
			copie[i][j] = (int *)calloc(3, sizeof(int));
	}
	// Se initializeaza matricea nucleu asociata filtrului ales
	double **mat_nucleu;
	mat_nucleu = alocare_double(NMAX, NMAX, img);
	init_nucleu(mat_nucleu, tip_filtru);

	int i = img->y[0];
	while (i < img->y[1]) {
		int j = img->x[0];
		while (j < img->x[1]) {
			// Se verifica daca indicii sunt buni, adica daca exclud marginile
			if (i > 0 && i < img->inaltime - 1 &&
				j > 0 && j < img->latime - 1) {
				double copie_r = 0, copie_b = 0, copie_g = 0;
				for (int x = 0; x < 3; x++) {
					for (int y = 0; y < 3; y++) {
						copie_r += img->mat_color[i - 1 + x][j - 1 + y].r *
								   mat_nucleu[x][y];
						copie_g += img->mat_color[i - 1 + x][j - 1 + y].g *
								   mat_nucleu[x][y];
						copie_b += img->mat_color[i - 1 + x][j - 1 + y].b *
								   mat_nucleu[x][y];
					}
				}
				copie[i][j][0] = clamp(round(copie_r));
				copie[i][j][1] = clamp(round(copie_g));
				copie[i][j][2] = clamp(round(copie_b));
			}
			j++;
		}
		i++;
	}
	// Se elibereaza memoria matricei nucleu
	for (i = 0; i < 3; i++)
		free(mat_nucleu[i]);
	free(mat_nucleu);
	// Se recopiaza elementele modificate in matricea initiala
	i = img->y[0];
	while (i < img->y[1]) {
		int j = img->x[0];
		while (j < img->x[1]) {
			if (i > 0 && i < img->inaltime - 1 &&
				j > 0 && j < img->latime - 1) {
				img->mat_color[i][j].r = copie[i][j][0];
				img->mat_color[i][j].g = copie[i][j][1];
				img->mat_color[i][j].b = copie[i][j][2];
			}
			j++;
		}
		i++;
	}
	// Se elibereaza memoria alocata pt matricea copie
	for (i = 0; i < img->inaltime; i++) {
		for (int j = 0; j < img->latime; j++)
			free(copie[i][j]);
		free(copie[i]);
	}
	free(copie);
}

// Functie pentru operatia APPLY
void aplicare(imagine *img, char input[])
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	char cmd[DMAX], cmd2[DMAX];
	input[strcspn(input, "\n")] = '\0';
	int num_args;
	num_args = sscanf(input, "%s %s", cmd, cmd2);

	// Verific daca s-au introdus suficienti parametri pt realizarea comenzii
	if (num_args != 2) {
		printf("Invalid command\n");
		return;
	}

	if (img->magic[1] == '2' || img->magic[1] == '5') {
		// Daca imaginea e gri
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int tip_filtru = alege_filtru(cmd2);
	if (tip_filtru == -1) {
		// Daca parametrul este gresit
		printf("APPLY parameter invalid\n");
		return;
	}
	aplicare_filtru(img, tip_filtru);
	printf("APPLY %s done\n", cmd2);
}

/*  Functie pentru a verifica daca numarul de bin-uri este putere a lui 2
	si daca se afla in intervalul cerut*/
int conditie_bin(int numar)
{
	if (numar < 2 || numar > 256)
		return 0; // Numarul nu este în intervalul specificat

	double log_rez = log2(numar);

	// Verifica daca rezultatul este un numar intreg
	if (log_rez == (int)log_rez)
		return 1; // Este o putere a lui 2
	else
		return 0; // Nu este o putere a lui 2
}

// Functie care calculeaza numarul de stelute ce trebuie afisat
int stelute(int valoare, int maxi, int x)
{
	double nr;
	nr = (double)valoare / maxi * x;

	return (int)nr;
}

// Functie pentru egalizarea vectorului de frecventa al histogramei
void egalizare_histograma(int frecventa[], int nr, int numar_bin)
{
	int sumele_partiale[numar_bin];
	// Se initializeaza cu 0 folosind memset
	memset(sumele_partiale, 0, numar_bin * sizeof(int));

	// Calculeaza dimensiunea unui bin
	int dimensiune_bin = nr / numar_bin;

	// Calculul sumelor partiale
	for (int i = 0; i < nr; i++) {
		int index_bin = i / dimensiune_bin;
		sumele_partiale[index_bin] += frecventa[i];
	}
	// Copierea sumelor partiale inapoi in vectorul original
	for (int i = 0; i < numar_bin; i++)
		frecventa[i] = sumele_partiale[i];
}

// Functie pentru realizarea operatiei HISTOGRAMA
void histograma(imagine *img, char input[])
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	int x, y;
	char *token;
	token = strtok(input, "\n ");
	// Se retine al doilea posibil cuvant si se verifica daca e diferit de NULL
	token = strtok(NULL, "\n ");
	if (!token) {
		printf("Invalid command\n");
		return;
	}
	x = atoi(token); // Converteste un sir de caractere la o valore int
	// Se retine al treilea posibil cuv si se verifica daca e diferit de NULL
	token = strtok(NULL, "\n ");
	if (!token) {
		printf("Invalid command\n");
		return;
	}
	y = atoi(token); // Converteste un sir de caractere la o valore int
	// Se retine al patrulea posibil cuv si se verifica daca e NULL
	token = strtok(NULL, "\n ");
	if (token) {
		printf("Invalid command\n");
		return;
	}
	// Se verifica conditiile parametrilor
	if (conditie_bin(y) == 0) {
		printf("Invalid set of parameters\n");
		return;
	}
	if (img->magic[1] == '3' || img->magic[1] == '6') {
		// Daca imaginea e color
		printf("Black and white image needed\n");
		return;
	}
	// Se aloca dinamic vectorul de frecventa al pixelilor
	int *frecventa;
	frecventa = (int *)calloc(DIM, sizeof(int));
	if (!frecventa) {
		fprintf(stderr, "Eroare la alocarea memoriei\n");
		eliberare(img);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < img->inaltime; i++)
		for (int j = 0; j < img->latime; j++)
			frecventa[img->mat_gri[i][j]]++;

	// Se realizeaza egalizarea vectorului de frecventa
	egalizare_histograma(frecventa, DIM, y);
	// Se calculeaza maximul frecventei
	int maxim = -1, i = 0;
	while (i < y) {
		if (maxim < frecventa[i])
			maxim = frecventa[i];
		i++;
	}
	// Se afiseaza histograma
	i = 0;
	while (i < y) {
		printf("%d\t|\t", stelute(frecventa[i], maxim, x));
		int j = 0, numar = stelute(frecventa[i], maxim, x);
		while (j < numar) {
			printf("*");
			j++;
		}
		printf("\n");
		i++;
	}
	// Se elibereaza memoria vectorului de frecventa
	free(frecventa);
}

// Functie care calculeaza suma aparitiilor primului element pana la cel actual
int suma_aparitii(int *frecventa, int elem_actual)
{
	int s = 0;
	for (int i = 0; i <= elem_actual; i++)
		s += frecventa[i];
	return s;
}

// Functie pentru operatia EEQUALIZE
void egalizarea_imaginii(imagine *img)
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->magic[1] == '3' || img->magic[1] == '6') {
		// Daca imaginea e color
		printf("Black and white image needed\n");
		return;
	}
	// Se aloca dinamic vectorul de frecventa al pixelilor
	int *frecventa;
	frecventa = (int *)calloc(DIM, sizeof(int));
	if (!frecventa) {
		fprintf(stderr, "Eroare la alocarea memoriei\n");
		eliberare(img);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < img->inaltime; i++) {
		for (int j = 0; j < img->latime; j++) {
			if (img->mat_gri[i][j] < 0 || img->mat_gri[i][j] > 255)
				printf("i %d, j %d, %d\n", i, j, img->mat_gri[i][j]);
			frecventa[img->mat_gri[i][j]]++;
		}
	}
	// Se calculeaza aria
	double aria = (double)img->inaltime * img->latime;
	double f = VALMAX * (double)(1 / aria);
	// Se modifica valoarea fiecarui pixel din imagine
	int s = 0;
	double pixel;
	for (int i = 0; i < img->inaltime; i++) {
		for (int j = 0; j < img->latime; j++) {
			// Se calc suma de la 0 pana la elementul actual a aparitiilor
			s = suma_aparitii(frecventa, img->mat_gri[i][j]);
			// Se calc valoarea noului pixel conform formulei
			pixel = clamp(round(f * (double)s));
			img->mat_gri[i][j] = pixel;
		}
	}
	printf("Equalize done\n");
	free(frecventa);
}

// Functie pt operatia de rotire a intregii imagani color (90 de grade)
void rotire_complet_color(imagine *img)
{
	/* Se aloca dinamic o copie a imaginii si se inverseaza
	nr liniilor cu cel al coloanelor */
	color_rgb **copie = alocare_color(img->latime, img->inaltime, img);
	// Se roteste imaginea
	for (int i = 0; i < img->inaltime; i++) {
		int linie = img->inaltime - 1 - i;
		for (int j = 0; j < img->latime; j++) {
			copie[j][i].r = img->mat_color[linie][j].r;
			copie[j][i].g = img->mat_color[linie][j].g;
			copie[j][i].b = img->mat_color[linie][j].b;
		}
	}
	free_color(img->mat_color, img->inaltime);
	// Se interschimba latime, inaltimea si coordonatele
	interschimba(&img->inaltime, &img->latime);
	interschimba(&img->x[1], &img->y[1]);
	// Se aloca dinamic memorie pt noua imagine rotita
	img->mat_color = alocare_color(img->inaltime, img->latime, img);
	// Se copiaza in matricea initiala
	for (int i = 0; i < img->inaltime; i++) {
		for (int j = 0; j < img->latime; j++) {
			img->mat_color[i][j].r = copie[i][j].r;
			img->mat_color[i][j].g = copie[i][j].g;
			img->mat_color[i][j].b = copie[i][j].b;
		}
	}
	free_color(copie, img->inaltime);
}

// Functie pentru rotirea unei selectii nxn dintr-o imagine color (90 de grade)
void rotire_selectie_color(imagine *img)
{
	// selectie nxn => nr coloane = nr linii
	int dim = img->y[1] - img->y[0];
	color_rgb **copie = alocare_color(dim, dim, img);

	int start_linie = img->y[1] - 1;
	int start_col = img->x[0];

	for (int dest = 0; dest < img->y[1] - img->y[0]; dest++) {
		start_linie = img->y[1] - 1;
		for (int dest_col = 0; dest_col < img->x[1] - img->x[0]; dest_col++) {
			copie[dest][dest_col].r = img->mat_color[start_linie][start_col].r;
			copie[dest][dest_col].g = img->mat_color[start_linie][start_col].g;
			copie[dest][dest_col].b = img->mat_color[start_linie][start_col].b;
			start_linie--;
		}
		start_col++;
	}

	for (int i = img->y[0]; i < img->y[1]; i++) {
		int poz_linie = i - img->y[0];
		for (int j = img->x[0]; j < img->x[1]; j++) {
			int poz_col = j - img->x[0];
			img->mat_color[i][j].r = copie[poz_linie][poz_col].r;
			img->mat_color[i][j].g = copie[poz_linie][poz_col].g;
			img->mat_color[i][j].b = copie[poz_linie][poz_col].b;
		}
	}
	free_color(copie, dim);
}

// Functie pt operatia de rotire a intregii imagini gri (90 de grade)
void rotire_complet_gri(imagine *img)
{
	/* Se aloca dinamic o copie a imaginii si se inverseaza
	nr liniilor cu cel al coloanelor */
	int **copie = alocare_gri(img->latime, img->inaltime, img);
	// Se roteste imaginea
	for (int i = 0; i < img->inaltime; i++)
		for (int j = 0; j < img->latime; j++)
			copie[j][i] = img->mat_gri[img->inaltime - 1 - i][j];

	// Se eleibereaza memoria imaginii initiale
	free_gri(img->mat_gri, img->inaltime);

	// Se interschimba latime, inaltimea si coordonatele
	interschimba(&img->inaltime, &img->latime);
	interschimba(&img->x[1], &img->y[1]);
	// Se aloca dinamic memorie pt noua imagine rotita
	img->mat_gri = alocare_gri(img->inaltime, img->latime, img);
	// Se copiaza in matricea initiala
	for (int i = 0; i < img->inaltime; i++)
		for (int j = 0; j < img->latime; j++)
			img->mat_gri[i][j] = copie[i][j];
	free_gri(copie, img->inaltime);
}

// Functie pentru rotirea unei selectii nxn dintr-o imagine gri (90 de grade)
void rotire_selectie_gri(imagine *img)
{
	// Selectie nxn => nr coloane = nr linii
		int dim = img->y[1] - img->y[0];

	// Aloca o matrice grayscale pentru copie
	int **copie = alocare_gri(dim, dim, img);

	// Copiaza si roteste imaginea in matricea copie
	for (int dest_linie = 0; dest_linie < dim; dest_linie++) {
		for (int dest_col = 0; dest_col < dim; dest_col++) {
			int src_linie = img->y[1] - 1 - dest_col;
			int src_col = img->x[0] + dest_linie;

			copie[dest_linie][dest_col] = img->mat_gri[src_linie][src_col];
		}
	}

	// Copiaza matricea rotita inapoi in imaginea originala
	for (int dest_linie = 0; dest_linie < dim; dest_linie++) {
		for (int dest_col = 0; dest_col < dim; dest_col++) {
			int src_linie = img->y[0] + dest_linie;
			int src_col = img->x[0] + dest_col;

			img->mat_gri[src_linie][src_col] = copie[dest_linie][dest_col];
		}
	}

	// Elibereaza memoria alocatapentru matricea copie
	free_gri(copie, dim);
}

// Functie ce returneaza numarul rotirilor ce trebuie executate
int rotire_multipla(int grade)
{
	int numar_rotatii = grade / 90;
	if (numar_rotatii < 0) {
		if (numar_rotatii == -3)
			numar_rotatii = 1;
		if (numar_rotatii == -2)
			numar_rotatii = 2;
		if (numar_rotatii == -1)
			numar_rotatii = 3;
	} else if (numar_rotatii == 4 || numar_rotatii == -4) {
		numar_rotatii = 0;
	}
	return numar_rotatii;
}

// Functie care verifica daca are loc o rotire completa
int verifica_rotire_completa(const imagine *img)
{
	return (img->x[0] == 0 && img->x[1] == img->latime &&
			img->y[0] == 0 && img->y[1] == img->inaltime);
}

// Functie care verifica daca selectia este patratica
int verifica_selectie_patratica(const imagine *img)
{
	if (img->x[0] != 0 || img->y[0] != 0 || img->x[1] != img->latime ||
		img->y[1] != img->inaltime)
		if (img->x[1] - img->x[0] != img->y[1] - img->y[0])
			return 1;

	return 0; // Selectia e patratica
}

// Functia pt operatia ROTATE
void rotire(imagine *img, char input[])
{
	if (img->verif_incarcare == 0) {
		printf("No image loaded\n");
		return;
	}
	int grd; // grade
	char *token;
	token = strtok(input, "\n ");
	// Se retine al doilea cuvant si se verifica daca e diferit de NULL
	token = strtok(NULL, "\n ");
	if (!token) {
		printf("Invalid command\n");
		return;
	}
	grd = atoi(token); // Converteste un sir de caractere la o valore int
	// Se retine al treilea posibil cuv si se verifica daca e NULL
	token = strtok(NULL, "\n ");
	if (token) {
		printf("Invalid command\n");
		return;
	}
	// Se verifica conditiile de rotire
	if (grd < -360 || grd > 360 || grd % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	int numar_rotiri = rotire_multipla(grd);

	// In cazul unei selectari se verifica daca selectia e patratica
	if (verifica_selectie_patratica(img)) {
		printf("The selection must be square\n");
		return;
	}

	if (img->magic[1] == '3' || img->magic[1] == '6') {
		// Daca imaginea e color
		if (verifica_rotire_completa(img)) {
			// Daca trebuie rotita intreaga imagine
			for (int i = 0; i < numar_rotiri; i++)
				rotire_complet_color(img);
		} else {
			// Daca trebuie rotita o selectie nxn
			for (int i = 0; i < numar_rotiri; i++)
				rotire_selectie_color(img);
		}
	} else {
		// Daca imaginea e gri
		if (verifica_rotire_completa(img)) {
			// Daca trebuie rotita intreaga imagine
			for (int i = 0; i < numar_rotiri; i++)
				rotire_complet_gri(img);
		} else {
			// Daca trebuie rotita o selectie nxn
			for (int i = 0; i < numar_rotiri; i++)
				rotire_selectie_gri(img);
		}
	}
	printf("Rotated %d\n", grd);
}

// Functie pentru operatia EXIT
void iesire(imagine *img)
{
	if (img->verif_incarcare == 0)
		printf("No image loaded\n");

	if (img->verif_incarcare == 1)
		eliberare(img);
}

int main(void)
{
	char cmd[DMAX], cmd2[DMAX], input[DMAX];
	int ok = 1;
	imagine img;
	img.verif_incarcare = 0;
	while (ok) {
		fgets(input, sizeof(input), stdin);
		char cp[DMAX];
		strcpy(cp, input);
		char *token;
		// Separa primele caractere pana la spatiu sau enter
		token = strtok(cp, "\n ");
		strcpy(cmd, token);
		// Separa urmatoarele caractere pana la urm spatiu sau enter
		token = strtok(NULL, "\n ");
		// Verifica daca acest cuvand este diferit de NULL
		if (token)
			strcpy(cmd2, token);

		if (strcmp(cmd, "LOAD") == 0) {
			incarcare_imagine(&img, cmd2);
		} else if (strcmp(cmd, "SELECT") == 0) {
			if (strcmp(cmd2, "ALL") == 0)
				selectare_totala(&img);
			else
				selectare(&img, input);
		} else if (strcmp(cmd, "HISTOGRAM") == 0) {
			histograma(&img, input);
		} else if (strcmp(cmd, "EQUALIZE") == 0) {
			egalizarea_imaginii(&img);
		} else if (strcmp(cmd, "ROTATE") == 0) {
			rotire(&img, input);
		} else if (strcmp(cmd, "CROP") == 0) {
			taiere(&img);
		} else if (strcmp(cmd, "APPLY") == 0) {
			aplicare(&img, input);
		} else if (strcmp(cmd, "SAVE") == 0) {
			salvare(&img, input);
		} else if (strcmp(cmd, "EXIT") == 0) {
			iesire(&img);
			ok = 0;
		} else {
			printf("Invalid command\n");
		}
	}
}
