#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stdprof.h"
#define MAX_LINE_LENGTH 100
int i;
int j;
enum partie{placement, start, end1, end2};
enum mode{fichier,bot};
typedef struct coord {
	int x;
	int y;
} coordonnees;

typedef struct navire{
	coordonnees debut;
	coordonnees fin;
	char* nom;
} navire;

typedef struct tabNavires{
	struct navire* navires;
	int navNombre;
	int navSize;
} tabNavires;

char* creerTableau(int size){ /* alloue en mémoire un tableau de char de taille size*size rempli d'espace */
	char* tab;
	tab = malloc_prof(size*size*sizeof(char));
	for (i = 0;i < size*size; i++){
    tab[i] = ' ';
  }
	return tab;
}

tabNavires* creerTabNavires(){ /* permet de stocker les navires en mémoire afin de mémoriser leurs états (coulé ou non) et leurs noms*/
	tabNavires *tabn = malloc_prof(sizeof(tabNavires));
	tabn-> navSize = 8;
	tabn->navires = malloc_prof(tabn->navSize*sizeof(navire));
	tabn-> navNombre = 0;
	return tabn;
}

int navEgal(navire nav1, navire nav2) { /*vérifie l'égalité entre deux navires*/
    if (nav1.debut.x == nav2.debut.x && nav1.debut.y == nav2.debut.y && nav1.fin.x == nav2.fin.x && nav1.fin.y == nav2.fin.y) return 1;
    return 0;
}

void ajouterNav(tabNavires *tbn, navire nav) { /*ajoute un navire à la pile bs*/
    if (tbn->navNombre == tbn->navSize - 1) {
        tbn->navSize *= 2;
        tbn->navires = realloc_prof(tbn->navires, tbn->navSize * sizeof(navire));
    }
    tbn->navires[tbn->navNombre++] = nav;
}

void supprTabNavire(tabNavires *tabn, navire nav) { /* retire un navire de la pile tabn*/
    for (i = 0; i < tabn->navNombre; i++){
        if (navEgal(tabn->navires[i], nav) == 1){
            for (j = i; j < tabn->navNombre - 1; j++){
                tabn->navires[j] = tabn->navires[j+1];
            }
            tabn->navNombre--;
            break;
        }
    }
}

int estCoule(navire nav, char* tab, int size){
	int bool=1;
	if (nav.debut.x == nav.fin.x){
		for(i=nav.debut.y;i<nav.fin.y;i++){
			if (tab[size*i+nav.debut.x] != '*'){
				return 0;
			}
		}
	}
	else{
		for(i=nav.debut.x;i<nav.fin.x;i++){
			if (tab[size*(nav.debut.y)+i] != '*'){
				return 0;
			}
		}
	}
	return bool;
}

navire obtenirNavireNom(char *nom, tabNavires *tabn, int joueur) { /*retrouve un navire dans la pile tbn à partir de son nom*/
    char c;
    navire nav;
    (joueur==0)?(c='A'):(c='B');
    for (i = 0; i < tabn->navNombre; i++) {
        nav = tabn->navires[i];
        if (strcmp(nav.nom, nom) == 0) {
            return nav;
        }
    }
    printf("Il n'y a aucun navire ayant ce nom\n");
    free_prof(tabn);
	free_prof(nom);
    exit(0);
}

navire obtenirNavireCoord(coordonnees coord, tabNavires *tabn) { /*retrouve un navire dans la pile tbn à partir d'une coordonnée*/
    navire nav;
    for (i = 0; i < tabn->navNombre; i++) {
        nav = tabn->navires[i];
        if (nav.debut.x <= coord.x && coord.x <= nav.fin.x && nav.debut.y <= coord.y && coord.y <= nav.fin.y) {
            return nav;
        }
    }
    printf("Ces coordonnées ne correspondent à aucun navire\n");
    free_prof(tabn);
    exit(0);
}

void deplacerNav(navire* nav, char c, char* tab, int size, tabNavires *tabn){ /*déplace un navire sur le tableau en prenant en compte les mines sur celui-ci ainsi que les collisions avec les autres navires*/
	int test;
	navire newNav;
	newNav.nom = nav->nom;
	if (c == 'H'){
		if (nav->debut.y != 0){
			if (nav->debut.x == nav->fin.x){
				if (tab[size*(nav->debut.y -1)+nav->debut.x] == ' '){
					for(i=nav->debut.y;i<nav->fin.y+1;i++){
						tab[size*(i-1)+nav->debut.x] = tab[size*i+nav->debut.x];
						tab[size*i+nav->debut.x] = ' ';
					}
					newNav.debut.x = nav->debut.x;
					newNav.fin.x = nav->fin.x;
					newNav.debut.y = nav->debut.y -1;
					newNav.fin.y = nav->fin.y -1;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					if(tab[size*(nav->debut.y -1)+nav->debut.x] == '#'){
						tab[size*(nav->debut.y)+nav->debut.x] = '*';
						printf("Mine percutée ! Le navire est touché.\n");
					}
					else if((tab[size*(nav->debut.y -1)+nav->debut.x] == 'A') || (tab[size*(nav->debut.y -1)+nav->debut.x] == 'B')){
						tab[size*(nav->debut.y)+nav->debut.x] = '*';
						tab[size*((nav->debut.y)-1)+nav->debut.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
					}
				}
			}
			else if (nav->debut.y == nav->fin.y){
				test = 0;
				for(i=nav->debut.x;i<nav->fin.x+1;i++){
					if (tab[size*(nav->debut.y -1)+i] != ' '){
						test++;
					}
				}
				if (test==0){
					for(i=nav->debut.x;i<nav->fin.x+1;i++){
						tab[size*(nav->debut.y-1)+i] = tab[size*nav->debut.y+i];
						tab[size*nav->debut.y+i] = ' ';
					}
					newNav.debut.x = nav->debut.x;
					newNav.fin.x = nav->fin.x;
					newNav.debut.y = nav->debut.y -1;
					newNav.fin.y = nav->fin.y -1;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					for(i=nav->debut.x;i<nav->fin.x+1;i++){
						if (tab[size*(nav->debut.y -1)+i] == '#'){
							tab[size*(nav->debut.y)+i] = '*';
							printf("Mine percutée ! Le navire est touché.\n");
						}
						else if ((tab[size*(nav->debut.y -1)+i] == 'A') || (tab[size*(nav->debut.y -1)+i] == 'B')){
						tab[size*(nav->debut.y)+i] = '*';
						tab[size*((nav->debut.y)-1)+i] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
						}
					}	
				}
			}
		}
		else exit(0);
	}
	if (c == 'B'){
		if(nav->fin.y != size){
			if (nav->debut.x == nav->fin.x){
				if (tab[size*(nav->fin.y +1)+nav->debut.x] == ' '){
					for(i=nav->fin.y;i>nav->debut.y-1;i--){
						tab[size*(i+1)+nav->debut.x] = tab[size*i+nav->debut.x];
						tab[size*i+nav->debut.x] = ' ';
					}
					newNav.debut.x = nav->debut.x;
					newNav.fin.x = nav->fin.x;
					newNav.debut.y = nav->debut.y +1;
					newNav.fin.y = nav->fin.y +1;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					if(tab[size*(nav->fin.y +1)+nav->debut.x] == '#'){
						tab[size*(nav->fin.y)+nav->debut.x] = '*';
						printf("Mine percutée ! Le navire est touché.\n");
					}
					else if((tab[size*(nav->fin.y +1)+nav->debut.x] == 'A') || (tab[size*(nav->fin.y +1)+nav->debut.x] == 'B')){
						tab[size*(nav->fin.y)+nav->debut.x] = '*';
						tab[size*((nav->fin.y)+1)+nav->debut.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
					}
				}
			}
			else if (nav->debut.y == nav->fin.y){
				test = 0;
				for(i=nav->fin.y;i>nav->debut.y-1;i--){
					if (tab[size*(nav->debut.y +1)+i] != ' '){
						test++;
					}
				}
				if (test==0){
					for(i=nav->fin.y;i>nav->debut.y-1;i--){
						tab[size*(nav->debut.y+1)+i] = tab[size*nav->debut.y+i];
						tab[size*nav->debut.y+i] = ' ';
					}
					newNav.debut.x = nav->debut.x;
					newNav.fin.x = nav->fin.x;
					newNav.debut.y = nav->debut.y +1;
					newNav.fin.y = nav->fin.y +1;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					for(i=nav->debut.x;i<nav->fin.x+1;i++){
						if (tab[size*(nav->fin.y +1)+i] == '#'){
							tab[size*(nav->fin.y)+i] = '*';
							printf("Mine percutée ! Le navire est touché.\n");
						}
						else if ((tab[size*(nav->debut.y +1)+i] == 'A') || (tab[size*(nav->debut.y +1)+i] == 'B')){
						tab[size*(nav->debut.y)+i] = '*';
						tab[size*((nav->debut.y)+1)+i] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
						}
					}	
				}
			}
		}
		else exit(0);
	}
	if (c == 'D'){
		if (nav->fin.x != size){
			if (nav->debut.y == nav->fin.y){
				if (tab[size*(nav->fin.y)+nav->fin.x+1] == ' '){
					for(i=nav->fin.x;i>nav->debut.x-1;i--){
						tab[size*(nav->debut.y)+i+1] = tab[size*(nav->debut.y)+i];
						tab[size*(nav->debut.y)+i] = ' ';
					}
					newNav.debut.x = nav->debut.x+1;
					newNav.fin.x = nav->fin.x+1;
					newNav.debut.y = nav->debut.y;
					newNav.fin.y = nav->fin.y;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					if(tab[size*(nav->debut.y)+nav->fin.x+1] == '#'){
						tab[size*(nav->debut.y)+nav->fin.x] = '*';
						printf("Mine percutée ! Le navire est touché.\n");
					}
					else if((tab[size*(nav->debut.y)+nav->fin.x+1] == 'A') || (tab[size*(nav->debut.y)+nav->fin.x+1] == 'B')){
						tab[size*(nav->debut.y)+nav->fin.x+1] = '*';
						tab[size*((nav->debut.y))+nav->fin.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
					}
				}
			}
			else if (nav->debut.x == nav->fin.x){
				test = 0;
				for(i=nav->debut.y;i<nav->fin.x+1;i++){
					if (tab[size*(i)+nav->debut.x+1] != ' '){
						test++;
					}
				}
				if (test==0){
					for(i=nav->debut.y;i<nav->fin.y+1;i++){
						tab[(size*i)+nav->debut.x+1] = tab[(size*i)+nav->debut.x];
						tab[(size*i)+nav->debut.x] = ' ';
					}
					newNav.debut.x = nav->debut.x+1;
					newNav.fin.x = nav->fin.x+1;
					newNav.debut.y = nav->debut.y;
					newNav.fin.y = nav->fin.y;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					for(i=nav->debut.y;i<nav->fin.y+1;i++){
						if (tab[(size*i)+nav->debut.x+1] == '#'){
							tab[(size*i)+nav->debut.x] = '*';
							printf("Mine percutée ! Le navire est touché.\n");
						}
						else if ((tab[(size*i)+nav->debut.x+1] == 'A') || (tab[(size*i)+nav->debut.x+1] == 'B')){
						tab[(size*i)+nav->debut.x+1] = '*';
						tab[(size*i)+nav->debut.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
						}
					}	
				}
			}
		}
		else exit(0);
	}
	if (c == 'G'){
		if (nav->debut.x != 0){
			if (nav->debut.y == nav->fin.y){
				if (tab[size*(nav->fin.y)+nav->fin.x-1] == ' '){
					for(i=nav->debut.x;i<nav->fin.x+1;i++){
						tab[size*(nav->debut.y)+i-1] = tab[size*(nav->debut.y)+i];
						tab[size*(nav->debut.y)+i] = ' ';
					}
					newNav.debut.x = nav->debut.x-1;
					newNav.fin.x = nav->fin.x-1;
					newNav.debut.y = nav->debut.y;
					newNav.fin.y = nav->fin.y;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					if(tab[size*(nav->debut.y)+nav->fin.x-1] == '#'){
						tab[size*(nav->debut.y)+nav->fin.x] = '*';
						printf("Mine percutée ! Le navire est touché.\n");
					}
					else if((tab[size*(nav->debut.y)+nav->fin.x-1] == 'A') || (tab[size*(nav->debut.y)+nav->fin.x-1] == 'B')){
						tab[size*(nav->debut.y)+nav->fin.x-1] = '*';
						tab[size*((nav->debut.y))+nav->fin.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
					}
				}
			}
			else if (nav->debut.x == nav->fin.x){
				test = 0;
				for(i=nav->debut.y;i<nav->fin.x+1;i++){
					if (tab[size*(i)+nav->debut.x-1] != ' '){
						test++;
					}
				}
				if (test==0){
					for(i=nav->debut.y;i<nav->fin.y+1;i++){
						tab[(size*i)+nav->debut.x-1] = tab[(size*i)+nav->debut.x];
						tab[(size*i)+nav->debut.x] = ' ';
					}
					newNav.debut.x = nav->debut.x-1;
					newNav.fin.x = nav->fin.x-1;
					newNav.debut.y = nav->debut.y;
					newNav.fin.y = nav->fin.y;
					supprTabNavire(tabn,*nav);
					ajouterNav(tabn,newNav);
				}
				else{
					for(i=nav->debut.y;i<nav->fin.y+1;i++){
						if (tab[(size*i)+nav->debut.x-1] == '#'){
							tab[(size*i)+nav->debut.x] = '*';
							printf("Mine percutée ! Le navire est touché.\n");
						}
						else if ((tab[(size*i)+nav->debut.x-1] == 'A') || (tab[(size*i)+nav->debut.x-1] == 'B')){
						tab[(size*i)+nav->debut.x-1] = '*';
						tab[(size*i)+nav->debut.x] = '*';
						printf("Navires en percussion ! Les deux navires sont touchés.\n");
						}
					}	
				}
			}
		}
		else exit(0);
	}
}

int pos(int size, coordonnees coord){ /* converti les coordonnées en l'indice correspondant dans le tableau */
	return (size*coord.y + coord.x);
}

void afficheTab(char *tab, int size) { /* affiche l'état du tableau */
	coordonnees coord;
	for (i=0; i < size + 3; i++){
		if (i == 0){
			printf("  ");
			for (j=0; j<size; j++){
				printf("%d ",j);
			}
			printf("\n");
		}
		else if (i == 1 || i == size+2){
			printf(" ");
			for (j=0; j < size*2+1; j++){
				if (j%2 == 0){
          printf("+");
        }
				else{
          printf("-");
			  }
      }
			printf("\n");
    }
		else{
			printf("%d|",i-2);
			for (j = 0; j<size*2-1; j++){
				if (j%2==0) {
					coord.x = j/2;
					coord.y = i-2;
					if (tab[pos(size,coord)] != 'B'){
						printf("%c", tab[pos(size,coord)]);
					}
					else printf(" ");
				}
				else printf(" ");
			}
			printf("|\n");
		}
	}
}

void afficheTab2Players(char *board, char *visibleTab, int size) { /*affiche l'état des 2 tableaux*/
    coordonnees coord;
    for (i = 0; i < size + 3; i++) {
        if (i == 0) {
            printf("  ");
            for (j = 0; j < size; j++) {
                printf("%d ",j);
            }
            printf("    ");
            for (j = 0; j < size; j++) {
                printf("%d ",j);
            }
            printf("\n");
        }
        else if (i == 1 || i == size + 2) {
            printf(" ");
            for (j = 0; j < size*2+1; j++) {
                if (j%2 == 0) printf("+");
                else printf("-");
            }
            printf("   ");
            for (j = 0; j < size*2+1; j++) {
                if (j%2 == 0) printf("+");
                else printf("-");
            }
            printf("\n");
        }
        else {
            printf("%d|",i-2);
            for (j = 0; j<size*2-1; j++) {
                if (j%2==0) {
                    coord.x = j/2;
                    coord.y = i-2;
                    printf("%c", board[pos(size,coord)]);
                }
                else printf(" ");
            }
            printf("|  ");
            printf("%d|",i-2);
            for (j = 0; j<size*2-1; j++) {
                if (j%2==0) {
                    coord.x = j/2;
                    coord.y = i-2;
                    printf("%c", visibleTab[pos(size,coord)]);
                }
                else printf(" ");
            }
            printf("|\n");
        }
    }
    printf("\n");
}

void placerNavire(navire nav, char *tab, int size, int joueur){ /* place un navire dans le tableau */
	char c;
	coordonnees newcoord;
	(joueur==0)?(c='A'):(c='B');
	if (nav.debut.x == nav.fin.x) {
		newcoord.x = nav.debut.x;
		for (i = nav.debut.y; i < nav.fin.y + 1; i++){
			newcoord.y = i;
			tab[pos(size,newcoord)] = c;
		}
	}
	else{
		newcoord.y = nav.debut.y;
		for (i = nav.debut.x; i < nav.fin.x + 1; i++){
			newcoord.x = i;
			tab[pos(size,newcoord)] = c;
		}
	}	
}

int tailleNavire(navire nav) { /*renvoie la taille d'un navire donné*/
    if (nav.debut.x == nav.fin.x) return (nav.fin.y - nav.debut.y + 1);
    return (nav.fin.x - nav.debut.x + 1);
}

int estPossible(char *board, navire nav, int size) {
    coordonnees start = nav.debut;
    coordonnees end = nav.fin;
    int onX = 0;
    int onY = 0;
    if (start.x < 0 || start.y < 0 || end.x >= size || end.y >= size) return 0;
    (start.x == end.x)?(onY = 1):(onX = 1);
    for (i = 0; i < tailleNavire(nav); i++) {
        if (board[pos(size,start)] != ' ') return 0;
        start.x += onX;
        start.y += onY;
    }
    return 1;
}

void Tir(int x1, int y1, char* tab, int size, tabNavires *tabn){ /* effectue un tir sur une case du tableau*/
	coordonnees tir;
	navire nav;
	tir.x = x1;
	tir.y = y1;
	if ((tab[pos(size,tir)] == 'A') || (tab[pos(size,tir)] == 'B')){
		printf("Tir en %d:%d. Touché !", x1, y1);
		tab[pos(size,tir)] = '*';
		nav = obtenirNavireCoord(tir,tabn);
        if (estCoule(nav,tab,size) == 1) printf(" Coulé !\n");
        else printf("\n");
	}
	else{
		printf("Tir en %d:%d. Raté !\n", x1, y1);
		tab[pos(size,tir)] = '#';
	}
}

void Tir2Players(coordonnees tirCoord, char *secretTab, char *visibleTab, int size, tabNavires *tbn) { /*fonction tir pour une partie à 2 joueurs*/
    navire nav;
    if (secretTab[pos(size,tirCoord)] == 'A' || secretTab[pos(size,tirCoord)] == 'B' || secretTab[pos(size,tirCoord)] == '*') {
        secretTab[pos(size,tirCoord)] = '*';
        visibleTab[pos(size,tirCoord)] = '*';
        printf("Touché !");
        nav = obtenirNavireCoord(tirCoord,tbn);
        if (estCoule(nav,secretTab,size) == 1) printf(" Coulé !\n");
        else printf("\n");
    }
    else {
        secretTab[pos(size,tirCoord)] = '#';
        visibleTab[pos(size,tirCoord)] = '#';
        printf("Raté...\n");
    }
}

int estFini2Players(char *board1, char *board2, int size) { /*vérifie si la partie est finie pour une partie à 2 joueurs*/
    int countJ1 = 0;
    int countJ2 = 0;
    for (i = 0; i < size*size; i++) {
        if (board1[i] == 'A' || board1[i] == 'B') countJ1++;
        if (board2[i] == 'B' || board2[i] == 'A') countJ2++;
    }
    if (countJ2 == 0) return 1;
    else if (countJ1 == 0) return 2;
    else return 0;
}

void placerNavireRandom(char *tab, int navSize, int size, int joueur, char *nom, tabNavires *tabn) { /*place aléatoirement un navire de taille donné dans le tableau*/
    int possible = 0;
    coordonnees start;
    coordonnees end;
    navire nav;
    int randomDirection;
    while (possible == 0) {
        start.x = rand()%10;
        start.y = rand()%10;
        randomDirection = rand()%2;
        if (randomDirection == 0) {
            end.x = start.x + navSize - 1;
            end.y = start.y;
        }
        else {
            end.y = start.y + navSize - 1;
            end.x = start.x;
        }
        nav.debut = start;
        nav.fin = end;
        nav.nom = nom;
        if (estPossible(tab,nav,size) == 1) {
            placerNavire(nav,tab,size,joueur);
            ajouterNav(tabn,nav);
            possible = 1;
        }
    }
}

int main(int argc, char *argv[]) {
	enum partie part;
	char line [MAX_LINE_LENGTH];
	int emptyFile = 1;
	FILE *fp;
	enum mode mode;
	int size;
	int x1;
	int x2;
	int y1;
	int y2;
	char* tab;
	tabNavires* tabn;
	navire nav1;
	int compte1;
	int compte2;
	mode = fichier;
	part = placement;
	for (i=1; i<argc; i++){
		if (strcmp(argv[i], "-f")==0 && i+1 < argc){
			fp = fopen(argv[i+1], "r");
			emptyFile = 0;
			if (fp==NULL){
				printf("erreur\n");
				exit(0);
			}
		}
        else if (strcmp(argv[i], "-i") == 0) {
            mode = bot;
        }
	}
	if ((emptyFile == 1) && (mode = bot)){
		fp = stdin;
	}
	if(mode == fichier){
	while (fgets(line, 100, fp) != NULL){
		if (strncmp(line, "Projet", 6) == 0){
      		sscanf(line, "Projet %d", &size);
      		tab = creerTableau(size);
			tabn = creerTabNavires();
      		if (tab == NULL){
				fclose(fp);
        		exit(0);
      		}
		}
		else if (strncmp(line, "J1", 2) == 0){
			if (line[3] == 'P'){
				if (part == placement){
					coordonnees start;
                    coordonnees end;
                    char *name = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
					char *getCoord = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
                    sscanf(line, "J1 P \"%*[^\"]\" %[^\n]", getCoord);
                    if (getCoord[1] == '-') {
                    	sscanf(line, "J1 P \"%[^\"]\" %d-%d:%d", name, &(start.x), &(end.x), &(start.y));
                        end.y = start.y;
                    }
                    else if (getCoord[3] == '-') {
                    sscanf(line, "J1 P \"%[^\"]\" %d:%d-%d", name, &(start.x), &(start.y), &(end.y));
                    end.x = start.x;
                    }
                    else {
                        sscanf(line, "J1 P \"%[^\"]\" %d:%d", name, &(start.x), &(start.y));
                        end.x = start.x;
                        end.y = start.y;
                    }
                    if (start.x >= 0 && start.y >= 0 && end.x < size && end.y < size) {
						nav1.debut.x = start.x;
						nav1.fin.x = end.x;
						nav1.debut.y = start.y;
						nav1.fin.y = end.y;
                        placerNavire(nav1,tab,size,0);
                        nav1.nom = name;
                        ajouterNav(tabn, nav1);
                    }
                    else {
                        printf("Position impossible\n");
                        free_prof(tab);
                        free_prof(name);
                        free_prof(tabn);
                        fclose(fp);
                        exit(0);
                    }
                    free_prof(getCoord);
			}
			else if(line[3] == 'T'){
				if (part == start){
                    sscanf(line, "J1 T %d:%d", &x1, &y1);
					if (x1 >= 0 && y1 < size) {
                        Tir(x1,y1,tab,size,tabn);
                    }
                    else {
                        printf("Position impossible\n");
                        free_prof(tab);
                        free_prof(tabn);
                        fclose(fp);
                        exit(0);
                    }
                }
                else {
                    printf("Tir avant que tous les navire ne soient placés\n");
                    free_prof(tab);
                    free_prof(tabn);
                    fclose(fp);
                    exit(0);
                }
			}
			}
			else if (line[3] == 'H' || line[3] == 'B' || line[3] == 'G' || line[3] == 'D') { /*déplacement des navires*/
                if (part == placement){
                        navire nav;
						char direction;
                        char *name = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
                        sscanf(line, "J1 %c \"%[^\"]s\"", &direction, name);
                        nav = obtenirNavireNom(name, tabn, 0);
                        supprTabNavire(tabn, nav);
                        deplacerNav(&nav, direction, tab, size, tabn);
                        free_prof(name);
                }
                else {
                    printf("Déplacement d'un navire avant qu'ils ne soient tous placés\n");
                    free_prof(tab);
                    free_prof(tabn);
                    fclose(fp);
                    exit(0);
                }
            }
		}
		else if (strncmp(line, "J2", 2) == 0){
			if (line[3] == 'P'){
				if (part == placement){
					coordonnees start;
                    coordonnees end;
                    char *name = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
					char *getCoord = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
                    sscanf(line, "J2 P \"%*[^\"]\" %[^\n]", getCoord);
                    if (getCoord[1] == '-') {
                    	sscanf(line, "J2 P \"%[^\"]\" %d-%d:%d", name, &(start.x), &(end.x), &(start.y));
                        end.y = start.y;
                    }
                    else if (getCoord[3] == '-') {
                    	sscanf(line, "J2 P \"%[^\"]\" %d:%d-%d", name, &(start.x), &(start.y), &(end.y));
                    	end.x = start.x;
                    }
                    else {
                        sscanf(line, "J2 P \"%[^\"]\" %d:%d", name, &(start.x), &(start.y));
                        end.x = start.x;
                        end.y = start.y;
                    }
                    if (start.x >= 0 && start.y >= 0 && end.x < size && end.y < size) {
						nav1.debut.x = start.x;
						nav1.fin.x = end.x;
						nav1.debut.y = start.y;
						nav1.fin.y = end.y;
                        placerNavire(nav1,tab,size,1);
                        nav1.nom = name;
                        ajouterNav(tabn, nav1);
                    }
                    else {
                        printf("Position impossible\n");
                        free_prof(tab);
                        free_prof(name);
                        free_prof(tabn);
                        fclose(fp);
                        exit(0);
                    }
                    free_prof(getCoord);
				}
				else if(line[3] == 'T'){
					if (part == start){
                   		sscanf(line, "J2 T %d:%d", &x1, &y1);
						if (x1 >= 0 && y1 < size) {
                        	Tir(x1,y1,tab,size,tabn);
                    	}
                    	else {
                        	printf("Position impossible\n");
                        	free_prof(tab);
                        	free_prof(tabn);
                        	fclose(fp);
                        	exit(0);
                    	}
                	}
                	else {
                    	printf("Tir avant que tous les navires ne soient placés\n");
                    	free_prof(tab);
                   		free_prof(tabn);
                    	fclose(fp);
                    	exit(0);
                	}
				}
			}
			else if (line[3] == 'H' || line[3] == 'B' || line[3] == 'G' || line[3] == 'D') { /*déplacement des navires*/
                if (part == placement){
                        navire nav;
						char direction;
                        char *name = malloc_prof(MAX_LINE_LENGTH * sizeof(char));
                        sscanf(line, "J2 %c \"%[^\"]s\"", &direction, name);
                        nav = obtenirNavireNom(name, tabn, 0);
                        supprTabNavire(tabn, nav);
                        deplacerNav(&nav, direction, tab, size, tabn);
                        free_prof(name);
                }
                else {
                    printf("Déplacement d'un navire avant qu'ils ne soient tous placés\n");
                    free_prof(tab);
                    free_prof(tabn);
                    fclose(fp);
                    exit(0);
                }
            }
		}
		else if (strncmp(line, "Afficher",8) == 0){
			afficheTab(tab,size);
		}
		else if (strncmp(line, "Jouer", 5) == 0){
			part = start;
		}
		if (part == start){
			compte1 = 0;
			compte2 = 0;
			for(j=0; j<size*size; j++){
				if (tab[j] == 'A'){
					++compte1;
				}
				else if (tab[j] == 'B'){
					++compte2;
				}
			}
			if (compte1==0){
				part = end2;
			}
			else if (compte2==0){
				part = end1;
			}
		}
		if (part == end2){
			printf("J2 a gagné !\n");
			free_prof(tab);
			fclose(fp);
			exit(0);
		}
		else if (part == end1){
			printf("J1 a gagné !\n");
			free_prof(tab);
			fclose(fp);
			exit(0);
		}	
		}
		fclose(fp);
	}
else if (mode == bot) { /*partie interactive contre un bot jouant aléatoirement*/
        char *navNom[5] = {"pa","c","ct","sm","t"};
        char *myBoard;
        char *enemyVisibleTab;
        char *enemySecretTab;
        tabNavires *tabn;
        tabNavires *enemyTabn;
		int k;
        int playing = 1;
        int fin = 0;
        int joueurRandom;
        size = 10;
        myBoard = creerTableau(size);
        enemyVisibleTab = creerTableau(size);
        enemySecretTab = creerTableau(size);
        tabn = creerTabNavires();
        enemyTabn = creerTabNavires();

        printf("--------------------------------------------------------------------------------------------------------------\n");
        printf("Partie contre le bot :\n");
		printf("   - Plateau 10x10\n");
		printf("   - un navire de taille 5, un de taille 4, deux de taille 3 et un de taille 2\n");
		printf("   - Possibilité de déplacer ses navires\n");
		printf("   - Le premier à couler tous les navires de son adversaire gagne\n");
        printf("--------------------------------------------------------------------------------------------------------------\n");

        srand(time(NULL));

        joueurRandom = rand()%2;
        if (joueurRandom == 0) printf("Vous êtes J1 !\n");
        else printf("Vous êtes J2 !\n");
        joueurRandom++;
        placerNavireRandom(enemySecretTab, 5, 10, 1, navNom[0], enemyTabn);
        placerNavireRandom(enemySecretTab, 4, 10, 1, navNom[1], enemyTabn);
        placerNavireRandom(enemySecretTab, 3, 10, 1, navNom[2], enemyTabn);
        placerNavireRandom(enemySecretTab, 3, 10, 1, navNom[3], enemyTabn);
        placerNavireRandom(enemySecretTab, 2, 10, 1, navNom[4], enemyTabn);
        printf("Le bot a placé tous ses navires !\n");
        printf("Placez vos navires :\n");
        for (k = 5; k > 0; k--) { /*placement des navires du joueur*/
            int newSize;
            navire nav;
            int possible = 0;
            if (k == 1 || k == 2) newSize = k+1;
            else newSize = k;
            printf("Navire de longueur %d à placer\n", newSize);
            while (possible == 0) {
                printf("Coordonnées de départ (X:Y) : ");
                scanf(" %d:%d", &x1, &y1);
                printf("\nCoordonnées d'arrivée (X:Y) : ");
                scanf(" %d:%d", &x2, &y2);
                printf("\n");
                nav.debut.x = x1;
                nav.debut.y = y1;
				nav.fin.x = x2;
				nav.fin.y = y2;
                nav.nom = navNom[5-k];
                if (tailleNavire(nav) != newSize) printf("Le navire ne fait pas la bonne taille !\n");
                else if (estPossible(myBoard,nav,size) == 0) printf("Cette position n'est pas possible !\n");
                else possible = 1;
            }
            placerNavire(nav,myBoard,size,joueurRandom);
			printf("nom : %s coordonnées : %d:%d | %d:%d\n",nav.nom, nav.debut.x, nav.debut.y, nav.fin.x, nav.fin.y);
            ajouterNav(tabn,nav);
			printf("tabn nom : %s, coordonnées : %d:%d | %d:%d\n", (tabn->navires[5-k]).nom, (tabn->navires[5-k]).debut.x, (tabn->navires[5-k]).debut.y, (tabn->navires[5-k]).fin.x, (tabn->navires[5-k]).fin.y);
            afficheTab2Players(myBoard,enemyVisibleTab,size);
        }
        printf("\nTous les navires ont été placés. La partie peut commencer !\n");
        while (fin == 0) {
            if (playing == joueurRandom) { /*tour du joueur*/
                coordonnees shootCoord;
                char c;
                int possible = 0;
                int correctInput = 0;
                printf("C'est à vous de jouer !\n");
                while (correctInput == 0) {
                    printf("Tirez (T) ou déplacez un navire (D).");
                    scanf(" %c",&c);
                    if (c == 'T') {
                        while (possible == 0) {
                            printf("Coordonnées de votre tir (X:Y) : ");
                            scanf("%d:%d", &(shootCoord.x), &(shootCoord.y));
                            if (shootCoord.x >= 0 && shootCoord.x < size && shootCoord.y >= 0 && shootCoord.y < size) {
                                Tir2Players(shootCoord, enemySecretTab, enemyVisibleTab, size, enemyTabn);
                                possible = 1;
                            }
                        }
                        correctInput = 1;
                    }
                    else if (c == 'D') {
                        possible = 0;
                        while (possible == 0) { 
                            int correct = 0;
                            int navNombre;
                            char *name;
                            char Direction;
                            navire nav;
                            while (correct == 0) {
                                printf("Choisissez le navire à déplacer : Porte-avion (1), Croiseur (2), Contre-torpilleur (3), Sous-marin (4), Torpilleur (5) : ");
                                scanf(" %d",&navNombre);
                                if (navNombre == 1 || navNombre == 2 || navNombre == 3 || navNombre == 4 || navNombre == 5) {
                                    correct = 1;
                                }
                                else printf("Entrez 1, 2, 3, 4 ou 5\n");
                            }
                            correct = 0;
                            name = navNom[navNombre-1];
                            while (correct == 0) {
                                printf("\nChoisissez une direction : haut (H), droite (D), bas (B), gauche (G) : ");
                                scanf(" %c",&Direction);
                                if (Direction == 'H' || Direction == 'D' || Direction == 'B' || Direction == 'G') {
                                    correct = 1;
                                }
                                else {
                                    printf("Entrez H, D, B ou G\n");
                                }
                            }
                            nav = obtenirNavireNom(name,tabn,joueurRandom);
                            deplacerNav(&nav,Direction,myBoard,size,tabn);
                            possible = 1;
                        }
                        correctInput = 1;
                    }
                    else printf("Entrez T ou D\n");
                }

            }
            else { /*au tour du bot*/
                int shootOrMove;
                shootOrMove = rand()%2;
                if (shootOrMove == 0) {
                    coordonnees shootCoord;
                    shootCoord.x = rand()%size;
                    shootCoord.y = rand()%size;
                    printf("Le bot a tiré en %d:%d\n", shootCoord.x, shootCoord.y);
                    Tir(shootCoord.x,shootCoord.y,myBoard,size,tabn);
                }
                else {
                    int posNav;
                    char *name;
                    int intDirection;
                    char chosenDirection;
                    navire nav;
                    printf("Le bot a déplacé un navire.\n");
                    posNav = rand()%5;
                    name = navNom[posNav];
                    intDirection = rand()%4;
                    switch (intDirection) {
                        case 0:
                            chosenDirection = 'H';
                            break;
                        case 1:
                            chosenDirection = 'D';
                            break;
                        case 2:
                            chosenDirection = 'B';
                            break;
                        default:
                            chosenDirection = 'G';
                            break;
                    }
                    nav = obtenirNavireNom(name,enemyTabn,3%(joueurRandom+1));
                    deplacerNav(&nav,chosenDirection,enemySecretTab,size,enemyTabn);
                }
            }
            (playing == 1)?(playing = 2):(playing = 1);
            afficheTab2Players(myBoard,enemyVisibleTab,size);
            if (estFini2Players(myBoard, enemySecretTab, size) == joueurRandom) {
                printf("Vous l'emportez !\n");
                fin = 1;
            }
            else if (estFini2Players(myBoard, enemySecretTab, size) != 0) {
                printf("Le bot l'emporte !\n");
                fin = 1;
            }
        }
        free_prof(myBoard);
        free_prof(enemyVisibleTab);
        free_prof(enemySecretTab);
        free_prof(tabn);
        free_prof(enemyTabn);
    }
    return 0;
}