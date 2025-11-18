#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **resizematr(int n, int **a)
{
	n = n * 4;
	int i, j;
	int **ar = (int **)malloc(n * sizeof(int *));
	for (int i = 0; i < n; i++) {
		ar[i] = (int *)malloc(n * sizeof(int));
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			ar[i][j] = a[i / 4][j / 4];
		}
	}
	for (int i = 0; i < n / 4; i++) {
		free(a[i]);
	}
	free(a);
	return ar;
}

void write(int n, int **r, int **g, int **b)
{
	printf("P3\n");
	printf("%d %d\n255\n", n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%d %d %d\n", r[i][j], g[i][j], b[i][j]);
		}
	}
}

int **gridd(int n, int **r, int **g, int **b)
{
	int i, j;
	int m = n / 4 + 1;
	int **gr = (int **)malloc(m * sizeof(int *));
	for (int i = 0; i < m; i++) {
		gr[i] = (int *)malloc(m * sizeof(int));
	}
	int **medie = (int **)malloc(n * sizeof(int *));
	for (int i = 0; i < n; i++) {
		medie[i] = (int *)malloc(n * sizeof(int));
	}
	// calculam media aritmetica pentru cele 3 culori din pixel
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			medie[i][j] = (r[i][j] + g[i][j] + b[i][j]) / 3;
		}
	}
	// dupa formula de calcul data in enunt calculam gridul
	for (i = 0; i <= n; i = i + 4) {
		for (j = 0; j <= n; j = j + 4) {
			int l = i / 4, c = j / 4;
			if (i == 0) {
				if (j == 0) {
					gr[l][c] = (medie[i][j] + medie[i + 1][j + 1]) / 2;
				}
				if (j == n) {
					gr[l][c] = medie[i + 1][j - 1];
				}
				if (j != n && j != 0) {
					gr[l][c] = (medie[i][j] + medie[i + 1][j + 1]);
					gr[l][c] = (gr[l][c] + medie[i + 1][j - 1]) / 3;
				}
			}
			if (i == n) {
				if (j == 0) {
					gr[l][c] = medie[i - 1][j + 1];
				}
				if (j == n) {
					gr[l][c] = medie[i - 1][j - 1];
				}
				if (j != n && j != 0) {
					gr[l][c] = (medie[i - 1][j + 1] + medie[i - 1][j - 1]) / 2;
				}
			}
			if (i != 0 && i != n) {
				if (j == 0) {
					gr[l][c] = (medie[i - 1][j + 1] + medie[i + 1][j + 1]);
					gr[l][c] = (gr[l][c] + medie[i][j]) / 3;
				}
				if (j == n) {
					gr[l][c] = (medie[i - 1][j - 1] + medie[i + 1][j - 1]) / 2;
				}
				if (j != n && j != 0) {
					gr[l][c] = medie[i][j] + medie[i + 1][j + 1];
					gr[l][c] = gr[l][c] + medie[i + 1][j - 1];
					gr[l][c] = gr[l][c] + medie[i - 1][j + 1];
					gr[l][c] = (gr[l][c] + medie[i - 1][j - 1]) / 5;
				}
			}
			if (gr[l][c] <= 200) {
				gr[l][c] = 1;
			} else {
				gr[l][c] = 0;
			}
		}
	}
	for (int i = 0; i < n; i++) {
		free(medie[i]);
	}
	free(medie);
	return gr;
}

void printgrid(int **gr, int m)
{
	int i, j;
	/// afisam gridul mai putin ultima linie si coloana
	for (i = 0; i < m - 1; i++) {
		for (j = 0; j < m - 1; j++) {
			printf("%d ", gr[i][j]);
		}
		printf("\n");
	}
	for (int i = 0; i < m; i++) {
		free(gr[i]);
	}
	free(gr);
}

int cazgrid(int c1, int c2, int c3, int c4)
{
	return c4 + c3 * 2 + c2 * 4 + c1 * 8;
}

int **init1(int caz)
{
	int i, j;
	// separam init_ul in doua cazuriin functie de cati pixeli aprinsi are
	// apoi initializam matricea cu 0 sau 180 in functie de caz si
	// marcam pixeli diferiti
	int **pat = (int **)malloc(4 * sizeof(int *));
	for (int i = 0; i < 4; i++) {
		pat[i] = (int *)malloc(4 * sizeof(int));
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			pat[i][j] = 0;
		}
	}
	if (caz == 1 || caz == 5) {
		pat[3][0] = 180;
		pat[2][0] = 255;
		pat[3][1] = 255;
	}
	if (caz == 2 || caz == 10) {
		pat[3][3] = 180;
		pat[3][2] = 255;
		pat[2][3] = 255;
	}
	if (caz == 3) {
		for (j = 0; j < 4; j++) {
			pat[2][j] = 255;
			pat[3][j] = 180;
		}
	}
	if (caz == 4 || caz == 5) {
		pat[0][3] = 180;
		pat[0][2] = 255;
		pat[1][3] = 255;
	}
	if (caz == 6) {
		for (i = 0; i < 4; i++) {
			pat[i][2] = 255;
			pat[i][3] = 180;
		}
	}
	if (caz == 8 || caz == 10) {
		pat[0][0] = 180;
		pat[0][1] = 255;
		pat[1][0] = 255;
	}
	if (caz == 9) {
		for (i = 0; i < 4; i++) {
			pat[i][1] = 255;
			pat[i][0] = 180;
		}
	}
	if (caz == 12) {
		for (j = 0; j < 4; j++) {
			pat[1][j] = 255;
			pat[0][j] = 180;
		}
	}
	return pat;
}

int **init2(int caz)
{
	int i, j;
	int **pat = (int **)malloc(4 * sizeof(int *));
	for (int i = 0; i < 4; i++) {
		pat[i] = (int *)malloc(4 * sizeof(int));
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			pat[i][j] = 180;
		}
	}
	if (caz == 7) {
		pat[0][0] = 0;
		pat[0][1] = 255;
		pat[1][0] = 255;
	}
	if (caz == 11) {
		pat[0][3] = 0;
		pat[0][2] = 255;
		pat[1][3] = 255;
	}
	if (caz == 13) {
		pat[3][3] = 0;
		pat[3][2] = 255;
		pat[2][3] = 255;
	}
	if (caz == 14) {
		pat[3][0] = 0;
		pat[2][0] = 255;
		pat[3][1] = 255;
	}
	if (caz == 15) {
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				pat[i][j] = 255;
			}
		}
	}
	return pat;
}

void print_init_contur(void)
{
	for (int ii = 0; ii <= 15; ii++) {
		int **pattern;
		if (ii != 7 && ii != 11 && ii != 13 && ii != 14 && ii != 15) {
			pattern = init1(ii);
		} else {
			pattern = init2(ii);
		}
		write(4, pattern, pattern, pattern);
		for (int i = 0; i < 4; i++) {
			free(pattern[i]);
		}
		free(pattern);
	}
}

void march(int n, int **r, int **g, int **b)
{
	printf("Marching Squares aplicat cu succes [%d-%d]\n", n, n);
	int m = n / 4 + 1;
	int i, j;
	int **gr = gridd(n, r, g, b);
	for (i = 0; i < m - 1; i++) {
		for (j = 0; j < m - 1; j++) {
			int caz, jj = j + 1;
			caz = cazgrid(gr[i][j], gr[i][jj], gr[i + 1][jj], gr[i + 1][j]);
			int **pattern;
			if (caz != 7 && caz != 11 && caz != 13 && caz != 14 && caz != 15) {
				pattern = init1(caz);
			} else {
				pattern = init2(caz);
			}
			// urmeaza sa punem patternul corespunzator pe pozitia destinata
			// in matricele noastre
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 4; l++) {
					r[4 * i + k][4 * j + l] = pattern[k][l];
					g[4 * i + k][4 * j + l] = pattern[k][l];
					b[4 * i + k][4 * j + l] = pattern[k][l];
				}
			}
			for (int ii = 0; ii < 4; ii++) {
				free(pattern[ii]);
			}
			free(pattern);
		}
	}
	for (int i = 0; i < m; i++) {
		free(gr[i]);
	}
	free(gr);
}

int read(int n, int **r, int **g, int **b)
{
	int i, j;
	//citim matricea si afisam mesajele de eroare daca este cazul
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (scanf("%d %d %d", &r[i][j], &g[i][j], &b[i][j]) == 3) {
				if (r[i][j] > 255 || r[i][j] < 0 || g[i][j] > 255) {
					printf("Eroare: valoare pixel necorespunzatoare\n");
					for (int i = 0; i < n; i++) {
						free(r[i]);
						free(g[i]);
						free(b[i]);
					}
					free(r);
					free(g);
					free(b);
					return 0;
				}
				if (g[i][j] < 0 || b[i][j] > 255 || b[i][j] < 0) {
					printf("Eroare: valoare pixel necorespunzatoare\n");
					for (int i = 0; i < n; i++) {
						free(r[i]);
						free(g[i]);
						free(b[i]);
					}
					free(r);
					free(g);
					free(b);
					return 0;
				}
			} else {
				printf("Eroare: eroare citire pixeli");
				for (int i = 0; i < n; i++) {
					free(r[i]);
					free(g[i]);
					free(b[i]);
				}
				free(r);
				free(g);
				free(b);
				return 0;
			}
		}
	}
	printf("Imagine citita cu succes [%d-%d]\n", n, n);
	return 1;
}

void free_matrix(int n, int **r, int **g, int **b)
{
	for (int i = 0; i < n; i++) {
		free(r[i]);
		free(g[i]);
		free(b[i]);
	}
	free(r);
	free(g);
	free(b);
}

int main(void)
{
	char command[10];
	int n, m, ok = 1;
	scanf("%s", command);
	while (ok == 1) {
		if (strcmp(command, "READ") == 0) {
			char p[10];
			int vmax;
			scanf("%s", p);
			if (strcmp(p, "P3") == 0) {
				scanf("%d %d", &n, &m);
				scanf("%d", &vmax);
				// alocam dinamic matricele
				int **r, **g, **b;
				r = (int **)malloc(n * sizeof(int *));
				g = (int **)malloc(n * sizeof(int *));
				b = (int **)malloc(n * sizeof(int *));
				for (int i = 0; i < n; i++) {
					r[i] = (int *)malloc(n * sizeof(int));
					g[i] = (int *)malloc(n * sizeof(int));
					b[i] = (int *)malloc(n * sizeof(int));
				}
				if (read(n, r, g, b) == 0) {
					return 0;
				}
				// citim comanda si executam ce ne cere
				scanf("%s", command);
				while (strcmp(command, "READ") != 0 && ok == 1) {
					if (strcmp(command, "RESIZE") == 0) {
						printf("Imagine redimensionata cu succes [%d-%d]\n",
						       n * 4, n * 4);
						r = resizematr(n, r);
						g = resizematr(n, g);
						b = resizematr(n, b);
						n = n * 4;
					}
					if (strcmp(command, "GRID") == 0) {
						int m = n / 4 + 1, **gr = gridd(n, r, g, b);
						printf("Grid calculat cu succes [%d-%d]\n",
						       m - 1, m - 1);
						printgrid(gr, m);
					}
					if (strcmp(command, "WRITE") == 0) {
						write(n, r, g, b);
					}
					if (strcmp(command, "INIT_CONTUR") == 0) {
						print_init_contur();
					}
					if (strcmp(command, "MARCH") == 0) {
						march(n, r, g, b);
					}
					if (strcmp(command, "EXIT") == 0) {
						printf("Gigel a terminat");
						free_matrix(n, r, g, b);
						return 0;
					}
					if (ok == 1) {
						scanf("%s", command);
					}
				}
				// eliberam matricele
				for (int i = 0; i < n; i++) {
					free(r[i]);
					free(g[i]);
					free(b[i]);
				}
				free(r);
				free(g);
				free(b);
			} else {
				printf("Eroare: trebuie sa fie P3");
				return 0;
			}
		}
	}
	return 0;
}
