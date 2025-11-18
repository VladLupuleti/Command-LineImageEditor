//Lupuleti Vlad-Calin, 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	char format[3];
	int width, height;
	int max_val;
	unsigned char **pixels;
	int is_color;
	int is_binary;
	int x1, y1, x2, y2;
} stru;

void free_image(stru *image)
{
	//eliberam memoria matricei alocate dinamic
	if (image->pixels) {
		for (int i = 0; i < image->height; i++) {
			if (image->pixels[i]) {
				free(image->pixels[i]);
				image->pixels[i] = NULL;
			}
		}
		free(image->pixels);
		image->pixels = NULL;
	}
}

void skip_comments(FILE *file)
{
	//ne asiguram ca nu exista comentarii in fisierele citite
	int c = fgetc(file);
	while (c == '#') {
		while (c != '\n' && c != EOF) {
			c = fgetc(file);
		}
		c = fgetc(file);
	}
	if (c != EOF) {
		ungetc(c, file);
	}
}

int load_image(const char *filename, stru *image)
{
	FILE *file = fopen(filename, "rb"); //deschidem fisierul
	if (!file) { //in caz ca nu exista fisierul dat afisam mesajul specific
		printf("Failed to load %s\n", filename);
		return 0;
	}
	if (fscanf(file, "%2s", image->format) != 1) {
		printf("Failed to load %s\n", filename);
		fclose(file);
		return 0;
	}
	image->is_binary = (image->format[1] == '4' || image->format[1] == '5' ||
					image->format[1] == '6'); //retinem daca imaginea e binara
	image->is_color = (image->format[1] == '3' || image->format[1] == '6');
	skip_comments(file); //retinem daca imaginea e color
	if (fscanf(file, "%d %d", &image->width, &image->height) != 2) {
		printf("Failed to load %s\n", filename);
		fclose(file);
		return 0;
	}
	if (image->format[1] != '1' && image->format[1] != '4') {
		skip_comments(file);
		if (fscanf(file, "%d", &image->max_val) != 1 || image->max_val > 255) {
			printf("Failed to load %s\n", filename);
			fclose(file);
			return 0;
		}
	} else {
		image->max_val = 1; //citim valoarea maxima a pixelilor daca este cazul
	} //alfel initializam aceasta valoare cu 1
	int pixel_size = image->is_color ? 3 : 1; //nr de elemente a fiecarui pixel
		image->pixels = (unsigned char **)malloc
	(image->height * sizeof(unsigned char *));
	for (int i = 0; i < image->height; i++) {
		image->pixels[i] = (unsigned char *)malloc
			(image->width * pixel_size * sizeof(unsigned char));
	} //alocam dinamic matricea in functie de dimensiune
	fgetc(file);
	if (!image->is_binary) { // daca fisierul e binar, facem citirea specifica
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width * pixel_size; j++) {
				int pixel_value;
				if (fscanf(file, "%d", &pixel_value) != 1) {
					printf("Failed to load %s\n", filename);
					free_image(image);
					fclose(file);
					return 0;
				}
				if (image->format[1] == '1') {//transformam P1 din 0 1 in 0 255
					image->pixels[i][j] = (pixel_value == 1) ? 255 : 0;
				} else {
					image->pixels[i][j] = (unsigned char)pixel_value;
				}
			}
		}
	} else { // daca fisierul nu e binar, facem citirea corespunzatoare
		for (int i = 0; i < image->height; i++) {
			if (fread(image->pixels[i], sizeof(unsigned char),
					  image->width * pixel_size, file) !=
				(size_t)(image->width * pixel_size)) {
				printf("Failed to load %s\n", filename);
				free_image(image);
				fclose(file);
				return 0;
			}
			if (image->format[1] == '4') {//transformam P4 din 0 1 in 0 255
				for (int j = 0; j < image->width; j++) {
					image->pixels[i][j] =
						(image->pixels[i][j] == 1) ? 255 : 0;
				}
			}
		}
	}
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->width;
	image->y2 = image->height;//setam selectia pe toate imaginea
	fclose(file);
	printf("Loaded %s\n", filename);
	return 1;
}

void SELECT(int x1s, int y1s, int x2s, int y2s, stru *image)
{
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}
	//daca coordonatele nu sunt in ord cresc, le inversam
	int aux;
	if (x1s > x2s) {
		aux = x1s;
		x1s = x2s;
		x2s = aux;
	}
	if (y1s > y2s) {
		aux = y1s;
		y1s = y2s;
		y2s = aux;
	}
	//verificam daca coordonatele date sunt valide
	//adica, daca sunt in interorul imaginii si daca nu desemneaza un patrat nul
	if (x1s < 0 || y1s < 0 || x2s > image->width ||
		y2s > image->height || x1s == x2s || y1s == y2s) {
		printf("Invalid set of coordinates\n");
		return;
	}
	//retinem noile coordonate de selectie
	image->x1 = x1s;
	image->y1 = y1s;
	image->x2 = x2s;
	image->y2 = y2s;
	printf("Selected %d %d %d %d\n", x1s, y1s, x2s, y2s);
}

void SELECT_ALL(stru *image)
{
	//modificam coordonatele selectiei in coordonatele matricei
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->width;
	image->y2 = image->height;
	printf("Selected ALL\n");
}

void CROP(stru *image)
{
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}
	//calculam dimensiunile noii imagini
	int h = image->y2 - image->y1;
	int w = image->x2 - image->x1;
	int pixel_size = image->is_color ? 3 : 1;
	//alocam dinamic o noua matrice unde copiem portinea selectata
	unsigned char **pix = (unsigned char **)malloc
		(h * sizeof(unsigned char *));
	for (int i = 0; i < h; i++) {
		pix[i] = (unsigned char *)malloc
			(w * pixel_size * sizeof(unsigned char));
	}
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w * pixel_size; j++) {
			pix[i][j] = image->pixels[i + image->y1]
								   [j + (image->x1 * pixel_size)];
		}
	}
	//eliberam matricea veche
	for (int i = 0; i < image->height; i++) {
		free(image->pixels[i]);
	}
	free(image->pixels);
	//copiem matricea nou creata, peste matricea din stru
	//ii actualizam coordonatele si variabilele de selectie
	image->pixels = pix;
	image->width = w;
	image->height = h;
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->width;
	image->y2 = image->height;
	printf("Image cropped\n");
}

void SAVE(const char *filename, stru *image, int ascii)
{
	FILE *out = fopen(filename, "wb");
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}
	//verificam in ce formate trebuie sa salvam imaginea
	if (ascii) {
		if (image->is_color) {
			fprintf(out, "P3\n%d %d\n255\n", image->width, image->height);
		} else {
			fprintf(out, "P2\n%d %d\n255\n", image->width, image->height);
		}
		//facem afisarea in format ascii
		int pixel_size = image->is_color ? 3 : 1;
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width * pixel_size; j++) {
				fprintf(out, "%d ", image->pixels[i][j]);
			}
			fprintf(out, "\n");
		}
	} else {
		if (image->is_color) {
			fprintf(out, "P6\n%d %d\n255\n", image->width, image->height);
		} else {
			fprintf(out, "P5\n%d %d\n255\n", image->width, image->height);
		}
		//facem afisarea in format binar
		int pixel_size = image->is_color ? 3 : 1;
		for (int i = 0; i < image->height; i++) {
			fwrite(image->pixels[i], sizeof(unsigned char),
				   image->width * pixel_size, out);
		}
	}
	fclose(out);
	printf("Saved %s\n", filename);
}

void ROTATE_FULL_IMAGE(int angle, stru *image)
{
	int w = image->width;
	int h = image->height;
	int pixel_size = image->is_color ? 3 : 1;
	if (angle % 90 != 0 || angle / 90 > 5 || angle / 90 < -5) {
		printf("Unsupported rotation angle\n");
		return;
	}
	printf("Rotated %d\n", angle);
	if (angle < 0) { //daca unghiul e negativ, adunam perioada
		angle += 360;
	}
	if (angle == 0 || angle == 360) { //rotirea la 360 nu modifica matricea
		return;
	}
	if (angle == 90 || angle == 270) {
		unsigned char **pix = (unsigned char **)
							  malloc(w * sizeof(unsigned char *));
		for (int i = 0; i < w; i++) {
			pix[i] = (unsigned char *)
					 malloc(h * pixel_size * sizeof(unsigned char));
		}
		//pt aceste cazuri retinem matricea rotita intr o matrice noua
		//noua matrice va avea dimensinile inversate fata de prima
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				for (int c = 0; c < pixel_size; c++) {
					if (angle == 90) {
						pix[j][(h - 1 - i) * pixel_size + c] =
							image->pixels[i][j * pixel_size + c];
					} else if (angle == 270) {
						pix[w - 1 - j][i * pixel_size + c] =
							image->pixels[i][j * pixel_size + c];
					}
				}
			}
		}
		for (int i = 0; i < h; i++) {
			free(image->pixels[i]);
		}
		free(image->pixels); //eliberam matricea
		image->pixels = pix;
	} else if (angle == 180) {
		unsigned char **pix = (unsigned char **)
							  malloc(h * sizeof(unsigned char *));
		for (int i = 0; i < h; i++) {
			pix[i] = (unsigned char *)
					 malloc(w * pixel_size * sizeof(unsigned char));
		}
		//cream matricea noua dar care are acelasi dimeniuni in acest caz
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				for (int c = 0; c < pixel_size; c++) {
					pix[h - 1 - i][(w - 1 - j) * pixel_size + c] =
						image->pixels[i][j * pixel_size + c];
				}
			}
		}
		for (int i = 0; i < h; i++) {
			free(image->pixels[i]);
		}
		free(image->pixels); //eliberam matricea veche
		image->pixels = pix; //atrubuim matricii noastre matricea noua
	}
	if (angle == 90 || angle == 270) { // inversam dimensiunile matricei
		int aux = image->width;
		image->width = image->height;
		image->height = aux;
	}
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->width;
	image->y2 = image->height;
}

void ROTATE(int angle, stru *image)
{
	//scriem o functie asemanatoare functiei ROTATE_FULL_IMAGE, dar
	//verificam ca selectia sa fie patratica inainte de a o roti
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}
	if (angle % 90 != 0 || angle / 90 > 5 || angle / 90 < -5) {
		printf("Unsupported rotation angle\n");
		return;
	}
	int h = image->y2 - image->y1;
	int w = image->x2 - image->x1;
	if (w != h) {
		printf("The selection must be square\n");
		return;
	}
	printf("Rotated %d\n", angle);
	if (angle < 0) {
		angle += 360;
	}
	if (angle == 0 || angle == 360) {
		return;
	}
	int pixel_size = image->is_color ? 3 : 1;
	unsigned char **pix = (unsigned char **)
						  malloc(h * sizeof(unsigned char *));
	for (int i = 0; i < h; i++) {
		pix[i] = (unsigned char *)
				 malloc(w * pixel_size * sizeof(unsigned char));
	}
	int n = h;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int c = 0; c < pixel_size; c++) {
				if (angle == 90) {
					pix[j][(n - 1 - i) * pixel_size + c] =
						image->pixels[image->y1 + i]
									 [(image->x1 + j) * pixel_size + c];
				}
				if (angle == 180) {
					pix[n - 1 - i][(n - 1 - j) * pixel_size + c] =
						image->pixels[image->y1 + i]
									 [(image->x1 + j) * pixel_size + c];
				}
				if (angle == 270) {
					pix[n - 1 - j][i * pixel_size + c] =
						image->pixels[image->y1 + i]
									 [(image->x1 + j) * pixel_size + c];
				}
			}
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n * pixel_size; j++) {
			image->pixels[image->y1 + i]
						 [(image->x1 * pixel_size) + j] = pix[i][j];
		}
	}
	for (int i = 0; i < n; i++) {
		free(pix[i]);
	}
	free(pix);
}

unsigned char clamp(int x)
{
	if (x < 0) {
		return 0;
	}
	if (x > 255) {
		return 255;
	}
	return (unsigned char)x;
}

void APPLY_EDGE(stru *image)
{
	//verificam posibilele erori
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}
	if (image->is_color == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int w = image->width;
	int h = image->height;
	int x1 = image->x1;
	int y1 = image->y1;
	int x2 = image->x2;
	int y2 = image->y2;
	unsigned char **r = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **g = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **b = (unsigned char **)malloc(h * sizeof(unsigned char *));
	for (int i = 0; i < h; i++) {
		r[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		g[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		b[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
	}
	//alocam dinamic matricele ce retin fiecare culoare din pixeli
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			r[i][j] = image->pixels[i][j * 3];
			g[i][j] = image->pixels[i][j * 3 + 1];
			b[i][j] = image->pixels[i][j * 3 + 2];
		}
	}
	//cream matricele ce retin valorile pt fiecare culoare din pixeli
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
				image->pixels[i][j * 3] = r[i][j];
				image->pixels[i][j * 3 + 1] = g[i][j];
				image->pixels[i][j * 3 + 2] = b[i][j];
			} else {
				image->pixels[i][j * 3] = clamp
					(8 * r[i][j] -
					(r[i - 1][j - 1] + r[i - 1][j] + r[i - 1][j + 1] +
					 r[i][j - 1] + r[i][j + 1] +
					 r[i + 1][j - 1] + r[i + 1][j] + r[i + 1][j + 1])
				);
				image->pixels[i][j * 3 + 1] = clamp
					(8 * g[i][j] -
					(g[i - 1][j - 1] + g[i - 1][j] + g[i - 1][j + 1] +
					 g[i][j - 1] + g[i][j + 1] +
					 g[i + 1][j - 1] + g[i + 1][j] + g[i + 1][j + 1])
				);
				image->pixels[i][j * 3 + 2] = clamp
					(8 * b[i][j] -
					(b[i - 1][j - 1] + b[i - 1][j] + b[i - 1][j + 1] +
					 b[i][j - 1] + b[i][j + 1] +
					 b[i + 1][j - 1] + b[i + 1][j] + b[i + 1][j + 1])
				);
			}
		}
	}
	//modificam valorile pixelilor in functie de vecini dupa formula data
	for (int i = 0; i < h; i++) {
		free(r[i]);
		free(g[i]);
		free(b[i]);
	}

	free(r);
	free(g);
	free(b);

	printf("APPLY EDGE done\n");
}

// pentru APPLY_SHARPEN, APPLY_BLUR, APPLY_GAUSSIAN_BLUR procedam similar
//functiei APPLY_EDGE

void APPLY_SHARPEN(stru *image)
{
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}

	if (image->is_color == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int w = image->width;
	int h = image->height;
	int x1 = image->x1;
	int y1 = image->y1;
	int x2 = image->x2;
	int y2 = image->y2;

	unsigned char **r = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **g = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **b = (unsigned char **)malloc(h * sizeof(unsigned char *));

	for (int i = 0; i < h; i++) {
		r[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		g[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		b[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			r[i][j] = image->pixels[i][j * 3];
			g[i][j] = image->pixels[i][j * 3 + 1];
			b[i][j] = image->pixels[i][j * 3 + 2];
		}
	}

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
				image->pixels[i][j * 3] = r[i][j];
				image->pixels[i][j * 3 + 1] = g[i][j];
				image->pixels[i][j * 3 + 2] = b[i][j];
			} else {
				image->pixels[i][j * 3] = clamp
					(5 * r[i][j] - (r[i - 1][j] + r[i][j - 1] +
								   r[i][j + 1] + r[i + 1][j])
				);
				image->pixels[i][j * 3 + 1] = clamp
					(5 * g[i][j] - (g[i - 1][j] + g[i][j - 1] +
								   g[i][j + 1] + g[i + 1][j])
				);
				image->pixels[i][j * 3 + 2] = clamp
					(5 * b[i][j] - (b[i - 1][j] + b[i][j - 1] +
								   b[i][j + 1] + b[i + 1][j])
				);
			}
		}
	}

	for (int i = 0; i < h; i++) {
		free(r[i]);
		free(g[i]);
		free(b[i]);
	}

	free(r);
	free(g);
	free(b);

	printf("APPLY SHARPEN done\n");
}

void APPLY_BLUR(stru *image)
{
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}

	if (image->is_color == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int w = image->width;
	int h = image->height;
	int x1 = image->x1;
	int y1 = image->y1;
	int x2 = image->x2;
	int y2 = image->y2;

	unsigned char **r = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **g = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **b = (unsigned char **)malloc(h * sizeof(unsigned char *));

	for (int i = 0; i < h; i++) {
		r[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		g[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		b[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			r[i][j] = image->pixels[i][j * 3];
			g[i][j] = image->pixels[i][j * 3 + 1];
			b[i][j] = image->pixels[i][j * 3 + 2];
		}
	}

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
				image->pixels[i][j * 3] = r[i][j];
				image->pixels[i][j * 3 + 1] = g[i][j];
				image->pixels[i][j * 3 + 2] = b[i][j];
			} else {
				image->pixels[i][j * 3] = (r[i][j] + r[i - 1][j - 1] +
										   r[i - 1][j] + r[i - 1][j + 1] +
										   r[i][j - 1] + r[i][j + 1] +
										   r[i + 1][j - 1] + r[i + 1][j] +
										   r[i + 1][j + 1]) / 9;

				image->pixels[i][j * 3 + 1] = (g[i][j] + g[i - 1][j - 1] +
											   g[i - 1][j] + g[i - 1][j + 1] +
											   g[i][j - 1] + g[i][j + 1] +
											   g[i + 1][j - 1] + g[i + 1][j] +
											   g[i + 1][j + 1]) / 9;

				image->pixels[i][j * 3 + 2] = (b[i][j] + b[i - 1][j - 1] +
											   b[i - 1][j] + b[i - 1][j + 1] +
											   b[i][j - 1] + b[i][j + 1] +
											   b[i + 1][j - 1] + b[i + 1][j] +
											   b[i + 1][j + 1]) / 9;
			}
		}
	}

	for (int i = 0; i < h; i++) {
		free(r[i]);
		free(g[i]);
		free(b[i]);
	}

	free(r);
	free(g);
	free(b);

	printf("APPLY BLUR done\n");
}

void APPLY_GAUSSIAN_BLUR(stru *image)
{
	if (!image->pixels) {
		printf("No image loaded\n");
		return;
	}

	if (image->is_color == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int w = image->width;
	int h = image->height;
	int x1 = image->x1;
	int y1 = image->y1;
	int x2 = image->x2;
	int y2 = image->y2;

	unsigned char **r = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **g = (unsigned char **)malloc(h * sizeof(unsigned char *));
	unsigned char **b = (unsigned char **)malloc(h * sizeof(unsigned char *));

	for (int i = 0; i < h; i++) {
		r[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		g[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
		b[i] = (unsigned char *)malloc(w * sizeof(unsigned char));
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			r[i][j] = image->pixels[i][j * 3];
			g[i][j] = image->pixels[i][j * 3 + 1];
			b[i][j] = image->pixels[i][j * 3 + 2];
		}
	}

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
				image->pixels[i][j * 3] = r[i][j];
				image->pixels[i][j * 3 + 1] = g[i][j];
				image->pixels[i][j * 3 + 2] = b[i][j];
			} else {
				image->pixels[i][j * 3] = (4 * r[i][j] + r[i - 1][j - 1] +
										   2 * r[i - 1][j] + r[i - 1][j + 1] +
										   2 * r[i][j - 1] + 2 * r[i][j + 1] +
										   r[i + 1][j - 1] + 2 * r[i + 1][j] +
										   r[i + 1][j + 1]) / 16;

				image->pixels[i][j * 3 + 1] = (4 * g[i][j] + g[i - 1][j - 1] +
											2 * g[i - 1][j] + g[i - 1][j + 1] +
											2 * g[i][j - 1] + 2 * g[i][j + 1] +
											g[i + 1][j - 1] + 2 * g[i + 1][j] +
											g[i + 1][j + 1]) / 16;

				image->pixels[i][j * 3 + 2] = (4 * b[i][j] + b[i - 1][j - 1] +
											2 * b[i - 1][j] + b[i - 1][j + 1] +
											2 * b[i][j - 1] + 2 * b[i][j + 1] +
											b[i + 1][j - 1] + 2 * b[i + 1][j] +
											b[i + 1][j + 1]) / 16;
			}
		}
	}

	for (int i = 0; i < h; i++) {
		free(r[i]);
		free(g[i]);
		free(b[i]);
	}

	free(r);
	free(g);
	free(b);

	printf("APPLY GAUSSIAN_BLUR done\n");
}

void HISTOGRAM(int x, int y, stru *image)
{
	int H[257] = {0};
	//verificam posibilele erori si afisam mesajele specifice
	if (image->is_color) {
		printf("Black and white image needed\n");
		return;
	}
	if (y < 2 || y > 256) {
		printf("Invalid set of parameters\n");
		return;
	}
	int yy = y;
	while (yy % 2 == 0) {
		yy = yy / 2;
	}
	if (yy != 1) {
		printf("Invalid set of parameters\n");
		return;
	}
	int w = image->width;
	int h = image->height;
	//cream matricea de frecventa pt valorile pixelilor
	for (int i = 0; i < 256; ++i) {
		H[i] = 0;
	}
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int val = (int)image->pixels[i][j];
			H[val]++;
		}
	}
	//aflam frecventa maxima a valorii pixelilor ce apare in formula de calcul
	int z = 256 / y;
	int max_fr = 0;
	for (int i = 0; i < y; ++i) {
		int sum = 0;
		for (int j = 0; j < z; ++j) {
			sum = sum + H[i * z + j];
		}
		if (max_fr < sum) {
			max_fr = sum;
		}
	}
	//conform formulei date, afisam histograma
	for (int i = 0; i < y; ++i) {
		int sum = 0;
		for (int j = 0; j < z; ++j) {
			sum = sum + H[i * z + j];
		}
		sum = (int)(x * sum / max_fr);
		printf("%d\t|\t", sum);
		for (int j = 0; j < sum; ++j) {
			printf("*");
		}
		printf("\n");
	}
}

void EQUALIZE(stru *image)
{
	if (image->is_color) {
		printf("Black and white image needed\n");
		return;
	}

	int w = image->width;
	int h = image->height;
	double area = (double)(h * w);

	int H[256] = {0};
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int val = (int)image->pixels[i][j];
			H[val]++;
		}
	}

	int S[256] = {0};
	S[0] = H[0];
	for (int i = 1; i < 256; ++i) {
		S[i] = S[i - 1] + H[i];
	}

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int val = (int)image->pixels[i][j];
			double new_val = (255.0 * S[val]) / area;
			int intval = round(new_val);
			image->pixels[i][j] = (unsigned char)intval;
		}
	}

	printf("Equalize done\n");
}

int READ_SELECT(stru *image)
{
	//verificam ca paramterii sa fie valizi
	//altfel scriem mesajele de eroare specifice
	int okk = 1, okkk = 0;
	char param[10];
	scanf("%s", param);
	if (strcmp(param, "ALL") == 0) {
		if (!image->pixels) {
			printf("No image loaded\n");
		} else {
			SELECT_ALL(image);
		}
	} else {
		int x1, y1, x2, y2;
		x1 = atoi(param);
		if (scanf("%d", &y1) && scanf("%d", &x2) &&
			scanf("%d", &y2)) {
			okk = 0;
			if (!image->pixels) {
				printf("No image loaded\n");
			} else {
				SELECT(x1, y1, x2, y2, image);
			}
		}
		if (okk == 1) {
			printf("Invalid command\n");
			okkk = 1;
		}
	}
	return okkk;
}

void READ_APPLY(stru *image)
{
	//verificam ca paramterii sa fie valizi
	//altfel scriem mesajele de eroare specifice
	char param[20];
	char rest_of_line[256];
	if (!image->pixels) {
		printf("No image loaded\n");
		fgets(rest_of_line, sizeof(rest_of_line), stdin);
	} else {
		fgets(rest_of_line, sizeof(rest_of_line), stdin);
		rest_of_line[strcspn(rest_of_line, "\n")] = '\0';
		//in functie de parametru apelam functie specifica
		if (sscanf(rest_of_line, "%s", param) != 1) {
			printf("Invalid command\n");
		} else if (strcmp(param, "EDGE") == 0) {
			APPLY_EDGE(image);
		} else if (strcmp(param, "BLUR") == 0) {
			APPLY_BLUR(image);
		} else if (strcmp(param, "SHARPEN") == 0) {
			APPLY_SHARPEN(image);
		} else if (strcmp(param, "GAUSSIAN_BLUR") == 0) {
			APPLY_GAUSSIAN_BLUR(image);
		} else {
			printf("APPLY parameter invalid\n");
		}
	}
}

void READ_HISTOGRAM(stru *image)
{
	//verificam ca paramterii sa fie valizi
	//altfel scriem mesajele de eroare specifice
	int x, y;
	char rest_of_line[256];
	if (!image->pixels) {
		printf("No image loaded\n");
		fgets(rest_of_line, sizeof(rest_of_line), stdin);
	} else if (scanf("%d %d", &x, &y) != 2) {
		printf("Invalid command\n");
	} else {
		fgets(rest_of_line, sizeof(rest_of_line), stdin); //citim intreaga linie
		rest_of_line[strcspn(rest_of_line, "\n")] = '\0';
		if (strlen(rest_of_line) > 0) {
			printf("Invalid command\n");
		} else {
			HISTOGRAM(x, y, image);
		}
	}
}

void READ_ROTATE(stru *image)
{
	int angle;
	scanf("%d", &angle);
	//in fuctie de coordonatele de selectie vedem care dintre
	//functii trebuie apelata
	if (!image->pixels) {
		printf("No image loaded\n");
	} else if (image->x1 == 0 && image->x2 == image->width &&
			   image->y1 == 0 && image->y2 == image->height) {
		ROTATE_FULL_IMAGE(angle, image);
	} else {
		ROTATE(angle, image);
	}
}

int main(void)
{
	stru img = {0};
	char command[20] = "", format[10] = "";
	int running = 1, okkk = 0;
	while (running) { //un while care merge pana apare functia EXIT
		int ok = 0; //ok care verifica daca comanda citita este una valida
		if (command[0] != '\0' && format[0] != '\0' &&
			strcmp(command, "SAVE") == 0 &&
			strcmp(format, "ascii") != 0) {
			strcpy(command, format);
		} else {
			scanf("%s", command);
		}
		if (strcmp(command, "SELECT") == 0) {
			ok = 1;
			okkk = READ_SELECT(&img);
		}
		if (strcmp(command, "LOAD") == 0) {
			if (img.pixels) {
				free_image(&img);
			}
			ok = 1;
			char filename[256];
			scanf("%s", filename);
			load_image(filename, &img);
		}
		if (strcmp(command, "HISTOGRAM") == 0) {
			ok = 1;
			READ_HISTOGRAM(&img);
		}
		if (strcmp(command, "EQUALIZE") == 0) {
			ok = 1;
			if (!img.pixels) {
				printf("No image loaded\n");
			} else {
				EQUALIZE(&img);
			}
		}
		if (strcmp(command, "CROP") == 0) {
			ok = 1;
			CROP(&img);
		}
		if (strcmp(command, "ROTATE") == 0) {
			ok = 1;
			READ_ROTATE(&img);
		}
		if (strcmp(command, "APPLY") == 0) {
			ok = 1;
			READ_APPLY(&img);
		}
		if (strcmp(command, "SAVE") == 0) {
			ok = 1;
			char filename[256];
			scanf("%s %s", filename, format);

			if (strcmp(format, "ascii") == 0) {
				SAVE(filename, &img, 1);
			} else {
				SAVE(filename, &img, 0);
			}
		}
		if (strcmp(command, "EXIT") == 0) {
			if (!img.pixels) {
				printf("No image loaded\n");
			}
			free_image(&img);
			ok = 1;
			running = 0;
		}
		if (ok == 0 && okkk == 0) {
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {
				;
			} //daca comanda nu e valida, ne asiguram ca citim tot pana la '\n'
			printf("Invalid command\n");
		}
	}
	return 0;
}
