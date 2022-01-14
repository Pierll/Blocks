#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "sdl.h"
#include "tetris.h"

void verifier_extremite_tetromino(Tetromino* t) {
    int extremite = 0;
    for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
        for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) {
            printf(" %d ",x);
            if (t->data[y][x] == 2 && x > extremite) {
                printf("extremité : %d\n", x);
            }
        }
    }
}
/*
int deplacement_tetrimino(Tetromino* t,Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN],int direction) {
	if (t->x > LARGEUR_TERRAIN || t->y > HAUTEUR_TERRAIN || !(t->direction_autorisee[direction_autorisee]))
		return 0;
	if ((t->y + TAILLE_MAX_TETROMINO) > HAUTEUR_TERRAIN)
	switch (direction) {
		case GAUCHE:
			for (int y=t->y; y<(t->y + TAILLE_MAX_TETROMINO); y++) {

			}
		break
	}
}
*/
int main(int argc, char* argv[]) {
    srand(time(NULL)); //générateur nombre aléatoire
    
    /** INIT SDL **/ 
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        error_sdl_launch("Impossible d'initialiser la SDL");
    }
    SDL_Window* Fenetre = NULL;
    Fenetre = SDL_CreateWindow("Tetris",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_SHOWN);
    SDL_Renderer* pRenderer = SDL_CreateRenderer(Fenetre, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;
    /** 	FIN INIT SDL	**/
    
    Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN] = {0};  //matrice 20*10
    Tetromino catalogue_tetromino[NOMBRE_TETROMINO];
    int sequence_tetromino[NOMBRE_TETROMINO];
    int tetromino_sac = 0;

    remplir_catalogue(catalogue_tetromino);
    Tetromino t = catalogue_tetromino[tetromino_sac]; //choisir le tetromino du sac

    /* !!! DEBUG SECTION !!!*/
    for (int i = 0; i < NOMBRE_TETROMINO; i++) {
        printf("data piece %d :\n", i);
        for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) {
            for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
                printf("%d", catalogue_tetromino[i].data[x][y]);
            }
            puts("");
        }
        printf("rayon rotation : %d\n", catalogue_tetromino[i].rayon_rotation);
        printf("etat rotation : %d\n", catalogue_tetromino[i].etat_rotation);
        printf("direction_autorisee: ");//DEBUG
        for (int k = 0; k < 3; k++) {
            printf("%d", catalogue_tetromino[i].direction_autorisee[k]);//DEBUG
        }
        puts("");
    }
    /* !!! FIN DEBUG SECTION !!!*/

    exit(1); //DEBUG
    int frame_attente = 1000.0/FPS;
    int frame_debut = 0;
    int frame_delai;

    if (!inserer_tetromino(&t,terrain)) {
        game_over();
    }

    /* BOUCLE PRINCIPALE */
    while (1) {
        /*if (tetromino_sac > 7) { //le sac est vide  // A IMPLEMENTER
        	choisir_sequence_tetromino(sequence_tetromino);
        	tetromino_sac = 0;
        }*/
        //event_clavier(&event, &t, terrain);
        //afficher_terrain(Fenetre,pRenderer, terrain);

        /* GESTION DELAI AFFICHAGE */
        frame_delai = frame_attente - (SDL_GetTicks() - frame_debut);
        if(frame_delai > 0) {
            SDL_Delay(frame_delai);
        }
        frame_debut = SDL_GetTicks();
        /** FIN GESTION DELAI AFFICHAGE **/
    }
    SDL_Quit();
    return 0;
}





void remplir_catalogue(Tetromino catalogue_tetromino[]) {
    FILE* fp;
    int nligne;
    char *ligne = NULL;
    size_t taille;
    if ( (fp = fopen("tetrominos.txt", "r")) == NULL) {
        printf("Erreur ! Le fichier \"tetromino\" est manquant ou inaccessible en lecture\n");
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < NOMBRE_TETROMINO; i++) {
            nligne = getline(&ligne, &taille, fp);
            if (nligne == -1) {
                printf("Erreur ! Le nombre de tetrominos dans le fichier est plus petit que le contenue de la macro NOMBRE_TETROMINO\n");
                exit(EXIT_FAILURE);
            }
            //printf("Ligne : %d : %s\n",i, ligne); //DEBUG
            catalogue_tetromino[i].type = i;
            int  pos = 0;
            for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) {
                for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
                    catalogue_tetromino[i].data[x][y] = ligne[pos] - '0'; // '-0' pour convertir char en int
                    pos++;
                }
            }
            char *next = strtok(ligne, ",");
            next = strtok(NULL, ",");//lire la valeur du rayon_rotation
            if (next == NULL) {
                    printf("Erreur fatale, le fichier tetromino est corrompu");
                    exit(EXIT_FAILURE);
            }
            catalogue_tetromino[i].rayon_rotation = atoi(next);
            int rgb[3];
            for (int y=0; y < 3; y++) { 
                next = strtok(NULL, ","); //lire la couleur
                if (next == NULL) {
                    printf("Erreur fatale, le fichier tetromino est corrompu");
                    exit(EXIT_FAILURE);
                }
                rgb[y] = atoi(next);
            }
            catalogue_tetromino[i].couleur = (SDL_Color) {rgb[0], rgb[1], rgb[2], 255};
            catalogue_tetromino[i].etat_rotation = 0;
            for (int y=0; y < 3; y++) {
                catalogue_tetromino[i].direction_autorisee[y] = 1; //toute les directions autorisée par défaut
            }
            
        }
    }
    fclose(fp);
}



void choisir_sequence_tetromino(int sequence_tetromino[]) {
    int i;
    for (i = 0; i<NOMBRE_TETROMINO; i++) { //remplir le sac de pieces dans l'ordre
        sequence_tetromino[i] = i;
    }
    for (i = 0; i<NOMBRE_TETROMINO; i++) { //mélanger le sac
        int position_aleatoire = rand() % NOMBRE_TETROMINO;
        int tmp = sequence_tetromino[i];
        sequence_tetromino[i] = sequence_tetromino[position_aleatoire];
        sequence_tetromino[position_aleatoire] = tmp;
    }
}

int inserer_tetromino(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    puts("INSERTION TETROMINO"); ///DEBUG
    /*t->couleur = (SDL_Color) {
        rand() % 255, rand() % 255, rand() % 255, 255
    }; //couleur aléatoire*/
    for (int y=0; y<TAILLE_MAX_TETROMINO; y++) {
        for (int x=LARGEUR_TERRAIN/2; x<LARGEUR_TERRAIN/2+TAILLE_MAX_TETROMINO; x++) {
            if (t->data[y][x-LARGEUR_TERRAIN/2] == 2) {
                if (terrain[x][y].valeur == 1) // si un bloc fixe obstrue la pièce
                    return 0;
                terrain[x][y].valeur = 2;
                terrain[x][y].couleur = t->couleur;
            }
        }
    }
    t->x = LARGEUR_TERRAIN/2;
    t->y = 0; //position par défaut
    return 1;
}

void rotation_tetromino(Tetromino* t, int angle) {
    
} 

void game_over() {
    printf("GAME OVER\n");
    exit(0);
}
