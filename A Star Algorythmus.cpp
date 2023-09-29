#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
/* and not not_eq */
#include <iso646.h>
/* add -lm to command line to compile with this header */
#include <math.h>

#define REHEN 10
#define ZEILEN 10

char karte[REHEN][ZEILEN] = {//Feld, dass man ändern kann wie man will (1 = nicht begehbar)
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
	{1, 1, 1, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

/* description of graph node */
struct stop {
	double zeilen, reihen;
	int* n;
	int n_len;
	double f, g, h;         //Kosten
	//f=g+h
	//f=gesamtkosten
	//g=wirkliche kosten
	//h=geschätzte kosten
	int from;
};

int feld2[REHEN][ZEILEN] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

struct route {                          //Strucktur, um mehrere ähnliche Variablen zu vereinen

	int x;      //Index der Haltestelle im Array aller Haltestellen der Quelle dieser Route* /
	int y;       //Index der Haltestelle im Array aller Haltestellen des Ziels dieser Route * /
	double d;
};

int main() {
	int i, j, k, l, b, gefunden;
	int p_len = 0;
	int* pfad = NULL;
	int c_len = 0;
	int* geschlossen = NULL;
	int o_len = 1;
	int* offen = (int*)calloc(o_len, sizeof(int));
	double min, tempg;
	int start;
	int ende;
	int aktuell;
	int s_len = 0;
	struct stop* stops = NULL;
	int r_len = 0;
	struct route* routes = NULL;

	for (i = 1; i < REHEN - 1; i++) {
		for (j = 1; j < ZEILEN - 1; j++) {
			if (!karte[i][j]) {
				++s_len;                                                                //Startlänge +1
				stops = (struct stop*)realloc(stops, s_len * sizeof(struct stop));      //Ändert die Größe des Speicherblocks, auf den pointer zeigt. Die Funktion kann den Speicherblock an eine neue Stelle verschieben
				int t = s_len - 1;														//Neue Variable t mit value von s_länge - 1
				stops[t].zeilen = j;													//in struktur stop wird der wert zeile von j übernommen
				stops[t].reihen = i;													//in struktur stop wird der wert reihe von j übernommen
				stops[t].from = -1;														//in struktur stop wird der wert from auf -1 gesetzt
				stops[t].g = DBL_MAX;													//in struktur stop wird der wert g auf den Wert der maximal darstellbaren endlichen Gleitkommazahl (Double) gesetzt
				stops[t].n_len = 0;														//in struktur stop wird der wert n_länge auf 0 gesetzt
				stops[t].n = NULL;														//Pointer n wird auf NULL gesetzt
				feld2[i][j] = t;														//Die stelle von feld2[i][j] wird auf den wert von t gesetzt
				//printf("s_len: %i . \nzeilen: %d . \nreihen: %d . \nfrom: %i. \ng: %i. \nn_len: %i. \nn: %i\n\n", s_len, stops->zeilen, stops->reihen, stops->from, stops->g, stops->n_len, stops->n);
			}
		}
	}


	start = 0;                                  //Startfeld        
	ende = s_len - 1;                          //Zielfeld

	for (i = 0; i < s_len; i++) {
		stops[i].h = sqrt(pow(stops[ende].reihen - stops[i].reihen, 2) + pow(stops[ende].zeilen - stops[i].zeilen, 2));		//h = √ ([wert von ende in reihe] - [wert von i in reihe]²) + ([wert von ende in zeile] - [wert von i in zeile]²)
	}

	for (i = 1; i < REHEN - 1; i++) {							//zählt hoch bis REIHEN (10)
		for (j = 1; j < ZEILEN - 1; j++) {						//zählt hoch bis ZEILEN (10)
			if (feld2[i][j] >= 0) {
				for (k = i - 1; k <= i + 1; k++) {
					for (l = j - 1; l <= j + 1; l++) {
						if ((k == i) and (l == j)) {
							continue;
						}
						if (feld2[k][l] >= 0) {					//wenn der wert von feld2[k][l] größer oder genau 0 ist
							++r_len;							//r_len wird um 1 erhöht
							routes = (struct route*)realloc(routes, r_len * sizeof(struct route));
							int t = r_len - 1;					//neue Variable t mit Wert von r_laenge - 1
							routes[t].x = feld2[i][j];			//X Koordinate
							routes[t].y = feld2[k][l];			//Y Koordinate
							routes[t].d = sqrt(pow(stops[routes[t].y].reihen - stops[routes[t].x].reihen, 2) + pow(stops[routes[t].y].zeilen - stops[routes[t].x].zeilen, 2));
							++stops[routes[t].x].n_len;
							stops[routes[t].x].n = (int*)realloc(stops[routes[t].x].n, stops[routes[t].x].n_len * sizeof(int));
							stops[routes[t].x].n[stops[routes[t].x].n_len - 1] = t;

						}
					}
				}
			}
		}
	}

	offen[0] = start;									//open stelle 0 wird auf den Wert von s gesetzt
	stops[start].g = 0;
	stops[start].f = stops[start].g + stops[start].h;
	gefunden = 0;

	while (o_len and not gefunden) {
		min = DBL_MAX;								//maximal darstellbaren endlichen Gleitkommazahl (Double) 

		for (i = 0; i < o_len; i++) {
			if (stops[offen[i]].f < min) {
				aktuell = offen[i];
				min = stops[offen[i]].f;
			}
		}
		if (aktuell == ende) {
			gefunden = 1;										//Computer weiß, dass die aktuelle stelle der Zielpunkt ist

			++p_len;											//Pfad länge wird erhöht
			pfad = (int*)realloc(pfad, p_len * sizeof(int));	//realloc = Ändert die Größe des Speicherblocks
			pfad[p_len - 1] = aktuell;
			while (stops[aktuell].from >= 0) {
				aktuell = stops[aktuell].from;
				++p_len;										//Pfad länge wird erhöht
				pfad = (int*)realloc(pfad, p_len * sizeof(int));
				pfad[p_len - 1] = aktuell;
			}
		}

		for (i = 0; i < o_len; i++) {
			if (offen[i] == aktuell) {
				if (i not_eq (o_len - 1)) {
					for (j = i; j < (o_len - 1); j++) {
						offen[j] = offen[j + 1];
					}
				}
				--o_len;
				offen = (int*)realloc(offen, o_len * sizeof(int));
				break;
			}
		}

		++c_len;
		geschlossen = (int*)realloc(geschlossen, c_len * sizeof(int));
		geschlossen[c_len - 1] = aktuell;

		for (i = 0; i < stops[aktuell].n_len; i++) {
			b = 0;

			for (j = 0; j < c_len; j++) {
				if (routes[stops[aktuell].n[i]].y == geschlossen[j]) {
					b = 1;
				}
			}

			if (b) {
				continue;
			}

			tempg = stops[aktuell].g + routes[stops[aktuell].n[i]].d;

			b = 1;

			if (o_len > 0) {
				for (j = 0; j < o_len; j++) {
					if (routes[stops[aktuell].n[i]].y == offen[j]) {
						b = 0;
					}
				}
			}

			if (b or (tempg < stops[routes[stops[aktuell].n[i]].y].g)) {
				stops[routes[stops[aktuell].n[i]].y].from = aktuell;
				stops[routes[stops[aktuell].n[i]].y].g = tempg;
				stops[routes[stops[aktuell].n[i]].y].f = stops[routes[stops[aktuell].n[i]].y].g + stops[routes[stops[aktuell].n[i]].y].h;

				if (b) {
					++o_len;
					offen = (int*)realloc(offen, o_len * sizeof(int));
					offen[o_len - 1] = routes[stops[aktuell].n[i]].y;
				}
			}
		}

	}

	for (i = 0; i < REHEN; i++) {
		for (j = 0; j < ZEILEN; j++) {
			if (karte[i][j]) {
				putchar(0xdb);
			}
			else {
				b = 0;
				for (k = 0; k < p_len; k++) {
					if (feld2[i][j] == pfad[k]) {
						++b;
					}
				}
				if (b) {
					putchar('x');
				}
				else {
					putchar('.');
				}
			}
		}
		putchar('\n');
	}

	if (not gefunden) {
		puts("Nicht möglich!");
	}
	else {
		printf("Die Pfadkosten sind %d:\n", p_len);
		for (i = p_len - 1; i >= 0; i--) {
			printf("(%1.0f, %1.0f)\n", stops[pfad[i]].zeilen, stops[pfad[i]].reihen);
		}
	}

	for (i = 0; i < s_len; ++i) {
		free(stops[i].n);
	}
	free(stops);
	free(routes);
	free(pfad);
	free(offen);
	free(geschlossen);

	system("PAUSE");
	return 0;

}