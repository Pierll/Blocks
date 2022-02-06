#include <SDL2/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sdl.h"
#include "tetris.h"

int rotation(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) { //A CORRIGER (bug quand la pièce est en bas)

    //if (t->pos.y == HAUTEUR_TERRAIN)
    
    int** buffer_piece = malloc(sizeof(int*) * t->rayon_rotation); // matrice où va être copiée la pièce 
    int** buffer_blocs = malloc(sizeof(int*) * t->rayon_rotation); // matrice où est copiée les blocs autre que la piece
    int** buffer_rotat = malloc(sizeof(int*) * t->rayon_rotation); // matrice où va être copiée une rotation de la pièce
    for (int i = 0; i < t->rayon_rotation; i++) {
        buffer_piece[i] = malloc(sizeof(int) * t->rayon_rotation);
        buffer_blocs[i] = malloc(sizeof(int) * t->rayon_rotation);
        buffer_rotat[i] = malloc(sizeof(int) * t->rayon_rotation);
    }
    
    /* on copie le tetromino et les blocs dans leur buffer respectifs */
    int i = 0; 
    int j = 0;
    printf("DEBUG MATRICE\n");
    for (int y = t->pos.y; y < (t->pos.y + t->rayon_rotation); y++, i++) {
        int x = t->pos.x;
        j = 0;
        for (; x < (t->pos.x + t->rayon_rotation); x++, j++) {    
            printf("%d", terrain[x][y].valeur);
            buffer_blocs[i][j] = (terrain[x][y].valeur == 1) ? 1 : 0;  
            buffer_piece[i][j] = (terrain[x][y].valeur == 2) ? 2 : 0;
            //printf("i/j:%d,%d | x/y:%d,%d\n",i,j,x,y); //DEBUG
        }
        printf("\n");
    }
    
    /* DEBUG SECTION */
    printf("Buffer piece:\n");
    for (i = 0; i < t->rayon_rotation; i++) {
        for (int u = 0; u < t->rayon_rotation; u++) {
            printf("%d", buffer_piece[i][u]);
        }
        printf("\n");
    }
    printf("Buffer blocs:\n");
    for (i = 0; i < t->rayon_rotation; i++) {
        for (int u = 0; u < t->rayon_rotation; u++) {
            printf("%d", buffer_blocs[i][u]);
        }
        printf("\n");
    }
    /* FIN DEBUG SECTION */
    
    /* copie de la rotation de la pièce dans le buffer rotat */
    i = 0; 
    j = 0;
    for (int y = 0; y < t->rayon_rotation; y++, i++) {
        j = 0;
        for (int x = t->rayon_rotation-1; x > -1; x--, j++)  {
            buffer_rotat[i][j] = buffer_piece[x][y];
            //printf("%d", buffer_piece[x][y]);
        }
        //puts("");
    }
    
    printf("Buffer rotat:\n");
    for (i = 0; i < t->rayon_rotation; i++) {
        for (int u = 0; u < t->rayon_rotation; u++) {
            printf("%d", buffer_rotat[i][u]);
        }
        printf("\n");
    }
    
    /* vérification d'une obstruction de la pièce */
     for (i = 0; i < t->rayon_rotation; i++) {
        for (int y = 0; y < t->rayon_rotation; y++) {
            if (buffer_rotat[i][y] == 2 && buffer_blocs[i][y] == 1) { //la rotation ne peut avoir lieu, un bloc gène
                printf("Impossible d'effectuer la rotation de la pièce !\n");
                return 0;
            }
                
        }
    }
    
    /* tout est bon, on retire l'ancienne pièce du terrain et place la nouvelle version */
    
    for (int y = t->pos.y; y < (t->pos.y + t->rayon_rotation); y++) { //retirer les anciens 2
        for (int x = t->pos.x; x < (t->pos.x + t->rayon_rotation); x++) {    
            if (terrain[x][y].valeur == 2) supprimer_case(x, y, terrain);
        }
        printf("\n");
    }
    
    int k = 0; //pour itérer dans le tableau des coords
    i = 0;
    j = 0;
    for (int y = t->pos.y; y < (t->pos.y + t->rayon_rotation); y++, i++) { //mettre les nouveau 2
        j = 0;
        for (int x = t->pos.x; x < (t->pos.x + t->rayon_rotation); x++, j++) {    
            if (buffer_rotat[i][j] == 2) {
                terrain[x][y].valeur = 2;
                terrain[x][y].couleur = (SDL_Color) t->couleur;
                t->coords[k] = (Coord) {x, y}; //on update les coordonnées
                k++;
            }
        }
    }
    
    for (int i = 0; i < t->rayon_rotation; i++) {
        free(buffer_piece[i]);
        free(buffer_blocs[i]);
        free(buffer_rotat[i]);
    }
    free(buffer_piece);
    free(buffer_blocs);
    free(buffer_rotat);
    
    return 1;
}

void swap(Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int x, int y, int x2,
          int y2) {
    Case tmp = terrain[x][y];
    terrain[x][y] = terrain[x2][y2];
    terrain[x2][y2] = tmp;
}

int deplacement_tetrimino(Tetromino *t,
                          Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN],
                          int direction) {

    if (t->direction_autorisee[direction] == 0) {
        puts("Echec deplacement : Direction non autorisee");
        return 0;
    }

    for (int i = 0; i < t->nbr_coords; i++) {
        /* on commence par retirer toute les cases de l'ancienne position */
        /*terrain[t->coords[i].x][t->coords[i].y].valeur = 0;
        terrain[t->coords[i].x][t->coords[i].y].couleur = (SDL_Color) {
            0, 0, 0, 0
        };*/
        supprimer_case(t->coords[i].x, t->coords[i].y, terrain);
        switch (direction) {
        case DROITE:
            (t->coords[i].x)++;
            break;
        case GAUCHE:
            (t->coords[i].x)--;
            break;
        case BAS:
            (t->coords[i].y)++;
            break;
        }

    }

    for (int i = 0; i < t->nbr_coords; i++) {
        /* on place les cases dans les nouvelles positions */
        terrain[t->coords[i].x][t->coords[i].y].valeur = 2;
        terrain[t->coords[i].x][t->coords[i].y].couleur = (SDL_Color) t->couleur;
        //printf("Couleur in deplacement: %d\n", t->couleur);
    }
    switch (direction) { //on update le pos
        case DROITE:
            (t->pos.x)++;
            break;
        case GAUCHE:
            (t->pos.x)--;
            break;
        case BAS:
            (t->pos.y)++;
            break;
        }
    printf("Nouvelle pos: %d, %d\n", t->pos.x, t->pos.y);
    return 1;
}

void afficher_terrain_ascii(Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    printf("   0123456789\n");
    printf("   ---------\n");
    for (int y = 0; y < HAUTEUR_TERRAIN; y++) {
        if (y < 10) {
            printf(" %d-", y);
        } else {
            printf("%d-", y);
        }
        for (int x = 0; x < LARGEUR_TERRAIN; x++) {
            printf("%d", terrain[x][y].valeur);
        }
        puts("");
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // générateur nombre aléatoire

    /** INIT SDL **/
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        error_sdl_launch("Impossible d'initialiser la SDL");
    }
    SDL_Window *Fenetre = NULL;
    Fenetre =
        SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *pRenderer =
        SDL_CreateRenderer(Fenetre, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;
    /** 	FIN INIT SDL	**/

    Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN] = {0}; // matrice 20*10
    /*DEBUG CODE TERRAIN*/
    Case c = {1, (SDL_Color) {
        255, 0, 0, 255
    }
             };
    terrain[9][1] = c;
    terrain[6][2] = c;
    //terrain[4][1] = c;
    //terrain[8][0] = c;
    /* FIN DEBUG CODE TERRAIN*/
    Tetromino catalogue_tetromino[NOMBRE_TETROMINO];
    int sequence_tetromino[NOMBRE_TETROMINO];
    int tetromino_sac = 4;
    int test_event = 777;
    remplir_catalogue(catalogue_tetromino);
    Tetromino t =
        catalogue_tetromino[tetromino_sac]; // choisir le tetromino du sac
    int frame_attente = 1000.0 / FPS;
    int frame_debut = 0;
    int frame_delai;

    /* DEBUG */
    //rotation(t);
    //exit(1);


    if (!inserer_tetromino(&t, terrain)) {
        game_over();
    }

    for (int i = 0; i < t.nbr_coords; i++) {
        printf("%d,%d\n", t.coords[i].x, t.coords[i].y);
    }
    afficher_terrain_ascii(terrain);
    /* BOUCLE PRINCIPALE */
    while (1) {
        /*if (tetromino_sac > 7) { //le sac est vide  // A IMPLEMENTER
            choisir_sequence_tetromino(sequence_tetromino);
            tetromino_sac = 0;
        }*/
        test_event = event_clavier(&event);
        if (test_event != 777) { // si l'utilisateur appuie sur une touche
            verifier_mouvement_piece(&t, terrain);


            /* SECTION DEBUG */
            for (int k = 0; k < 3; k++) {
                printf("%d", t.direction_autorisee[k]); // DEBUG
            }
            puts("");
            /* FIN SECTION DEBUG */
            switch (test_event) {
            case FGAUCHE:
                puts("GAUCHE");
                deplacement_tetrimino(&t, terrain, GAUCHE);
                break;
            case FBAS:
                puts("BAS");
                deplacement_tetrimino(&t, terrain, BAS);
                break;
            case FDROIT:
                puts("DROIT");
                deplacement_tetrimino(&t, terrain, DROITE);
                break;
            case FHAUT:
                puts("HAUT");
                rotation(&t, terrain);
                break;
            case ESPACE:
                puts("ESPACE");
                break;
            }
            afficher_terrain_ascii(terrain);
        }

        afficher_terrain(Fenetre, pRenderer, terrain);

        /* GESTION DELAI AFFICHAGE */
        frame_delai = frame_attente - (SDL_GetTicks() - frame_debut);
        if (frame_delai > 0) {
            SDL_Delay(frame_delai);
        }
        frame_debut = SDL_GetTicks();
        /** FIN GESTION DELAI AFFICHAGE **/
    }
    free(t.coords);
    SDL_Quit();
    return 0;
}

void remplir_catalogue(Tetromino catalogue_tetromino[]) {
    FILE *fp;
    int nligne;
    char *ligne = NULL;
    size_t taille;
    if ((fp = fopen("tetrominos.csv", "r")) == NULL) {
        printf("Erreur ! Le fichier \"tetromino\" est manquant ou inaccessible en "
               "lecture\n");
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < NOMBRE_TETROMINO; i++) {
            nligne = getline(&ligne, &taille, fp);
            if (nligne == -1) {
                printf("Erreur ! Le nombre de tetrominos dans le fichier est plus "
                       "petit que le contenue de la macro NOMBRE_TETROMINO\n");
                exit(EXIT_FAILURE);
            }
            // printf("Ligne : %d : %s\n",i, ligne); //DEBUG
            catalogue_tetromino[i].type = i;

            /* Remplir les matrices data[][][] */
            int pos = 0;
            for (int z = 0; z < 4; z++) {
                for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) {
                    for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
                        catalogue_tetromino[i].data[z][x][y] =
                            ligne[pos] - '0'; // '-0' pour convertir char en int
                        pos++;
                    }
                }
            }
            char *next = strtok(ligne, ",");
            next = strtok(NULL, ","); // lire la valeur du rayon_rotation
            if (next == NULL) {
                printf("Erreur fatale, le fichier tetromino est corrompu");
                exit(EXIT_FAILURE);
            }
            catalogue_tetromino[i].rayon_rotation = atoi(next);
            int rgb[3];
            for (int y = 0; y < 3; y++) {
                next = strtok(NULL, ","); // lire la couleur
                if (next == NULL) {
                    printf("Erreur fatale, le fichier tetromino est corrompu");
                    exit(EXIT_FAILURE);
                }
                rgb[y] = atoi(next);
            }
            // printf("Couleur lecture: %d, %d, %d\n",rgb[0], rgb[1], rgb[2]);
            catalogue_tetromino[i].couleur = (SDL_Color) {
                rgb[0], rgb[1], rgb[2], 255
            };
            catalogue_tetromino[i].etat_rotation = 0;
            for (int y = 0; y < 3; y++) {
                catalogue_tetromino[i].direction_autorisee[y] =
                    1; // toute les directions autorisée par défaut
            }
        }
    }
    fclose(fp);
}

void choisir_sequence_tetromino(int sequence_tetromino[]) {
    int i;
    for (i = 0; i < NOMBRE_TETROMINO;
            i++) { // remplir le sac de pieces dans l'ordre
        sequence_tetromino[i] = i;
    }
    for (i = 0; i < NOMBRE_TETROMINO; i++) { // mélanger le sac
        int position_aleatoire = rand() % NOMBRE_TETROMINO;
        int tmp = sequence_tetromino[i];
        sequence_tetromino[i] = sequence_tetromino[position_aleatoire];
        sequence_tetromino[position_aleatoire] = tmp;
    }
}

int inserer_tetromino(Tetromino *t,
                      Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    puts("***INSERTION TETROMINO***"); /// DEBUG

    int nbr_coords = 0;
    for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) { //compte le nombre de block dans le tetromino pour allouer le tableau
        for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
            if (t->data[0][x][y] == 2) {
                nbr_coords++;
            }
        }
    }
    t->coords = malloc(sizeof(Coord) * nbr_coords); //alloue le tableau de coordonnés
    t->nbr_coords = nbr_coords;

    int i = 0; //pour compter dans le tableau des coords
    for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
        for (int x = LARGEUR_TERRAIN / 2; x < LARGEUR_TERRAIN / 2 + TAILLE_MAX_TETROMINO; x++) {
            if ((y == 0) && (x == (LARGEUR_TERRAIN / 2))) { //la première case est la case à partir de laquelle on peut calculer la zone entourant le tetromino
                printf("pos: %d, %d\n", x, y);
                t->pos.x = x;
                t->pos.y = y;
            }
            if (t->data[0][y][x - LARGEUR_TERRAIN / 2] == 2) {
                printf("Coords : %d,%d\n", y, x - LARGEUR_TERRAIN / 2);
                if (terrain[x][y].valeur == 1) { // si un bloc fixe obstrue la pièce
                    printf("Erreur dans l'insertion du tetromino\n");
                    return 0;
                }

                terrain[x][y].valeur = 2;
                terrain[x][y].couleur = t->couleur;

                t->coords[i].x = x;
                t->coords[i].y = y;
                i++;


            }
        }
    }
    return 1;
}

void rotation_tetromino(Tetromino *t, int angle) {}

void game_over() {
    printf("GAME OVER\n");
    exit(0);
}

void physique(Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    for (int y = 0; y < HAUTEUR_TERRAIN; y++) {
        for (int x = 0; x < LARGEUR_TERRAIN; x++) {
            if ((terrain[x][y].valeur == 2) && (terrain[x][y + 1].valeur == 1)) {
                printf("Solidification : %d,%d et %d,%d\n", x, y, x, y + 1); // debug
                // TODO LA PIECE SE SOLIDIFIE
            }
            if ((terrain[x][y].valeur == 1) && (terrain[x][y + 1].valeur == 0)) {
                printf("Gravitee 1: %d,%d et %d,%d\n", x, y, x, y + 1); // debug
                terrain[x][y].valeur = 0;
                terrain[x][y + 1].valeur = 1;
            }
            if ((terrain[x][y].valeur == 2) && (terrain[x][y + 1].valeur == 0)) {
                printf("Gravitee 2: %d,%d et %d,%d\n", x, y, x, y + 1); // debug
                /// TODO LA PIECE TOMBE DE 1 BLOC SI ELLE N'EST PAS OBSTRUE
            }
        }
    }
}

void gravitee_piece(Tetromino *t,
                    Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {

}

void verifier_mouvement_piece(Tetromino *t,
                              Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    printf("\nverif mouvement:\n");
    t->direction_autorisee[GAUCHE] = 1;
    t->direction_autorisee[BAS] = 1;
    t->direction_autorisee[DROITE] = 1;

    for (int i = 0; i < t->nbr_coords; i++) {
        int x = t->coords[i].x;
        int y = t->coords[i].y;
        if (x == 0) { //au bord de l'écran gauche ?
            t->direction_autorisee[GAUCHE] = 0;
        } else { // si le coord n'est pas au bord, on peut contrôler sans risque de dépassements
            if (terrain[x - 1][y].valeur == 1) //contrôler à gauche
                t->direction_autorisee[GAUCHE] = 0;
        }

        if (x == LARGEUR_TERRAIN-1) {//au bord de l'écran droite
            t->direction_autorisee[DROITE] = 0;
        } else {
            if (terrain[x + 1][y].valeur == 1) //contrôler à droite
                t->direction_autorisee[DROITE] = 0;
        }

        if (y == 0) {
            t->direction_autorisee[HAUT] = 0;
        } else {
            if (terrain[x][y+1].valeur == 1)
                t->direction_autorisee[BAS] = 0;
        }

        if (y == HAUTEUR_TERRAIN-1) {
            t->direction_autorisee[BAS] = 0;
        } else {
            if (terrain[x][y-1].valeur == 1)
                t->direction_autorisee[HAUT] = 0;
        }
    }


}
