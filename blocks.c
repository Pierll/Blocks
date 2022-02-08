#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sdl.h"
#include "blocks.h"
#include "pile.h"

void afficher_terrain_ascii(Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) { /* pour le debug */
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

int score = 0; //variable score globale

int main(int argc, char *argv[]) {
    srand(time(NULL)); // générateur nombre aléatoire

    /** INIT SDL **/
    if(TTF_Init() == -1) {
        printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
        exit(2);
    }
    TTF_Font * font = TTF_OpenFont("FreeMono.ttf", 25);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        error_sdl_launch("Impossible d'initialiser la SDL");
    }
    SDL_Window *Fenetre = NULL;
    Fenetre = SDL_CreateWindow("Blocks !!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LARGEUR_FENETRE, HAUTEUR_FENETRE, SDL_WINDOW_SHOWN);
    SDL_Renderer *pRenderer = SDL_CreateRenderer(Fenetre, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;
    int frame_attente = 1000.0 / FPS;
    int frame_debut = 0;
    int frame_delai;
    /** 	FIN INIT SDL	**/

    Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN] = {0}; // matrice 20*10
    Tetromino catalogue_tetromino[NOMBRE_TETROMINO];
    int sac_tetromino[NOMBRE_TETROMINO];
    int tetromino_sac = 0;
    int test_event = 777;
    int last_time = 0, current_time; //pour calculer le temps écoulé

    int difficultee = 1000; //en milliseconde
    remplir_catalogue(catalogue_tetromino);
    for (int i = 0; i < NOMBRE_TETROMINO; i++) sac_tetromino[i] = i;//on commence par remplir le sac dans l'ordre
    choisir_sequence_tetromino(sac_tetromino); /* melanger le sac */
    Tetromino t = catalogue_tetromino[sac_tetromino[tetromino_sac]]; // choisir le tetromino du sac

    inserer_tetromino(&t, terrain); //inserer le premier tetromino
    /* BOUCLE PRINCIPALE */
    while (1) {
        test_event = event_clavier(&event);
        if (test_event != 777) { // si l'utilisateur appuie sur une touche
            switch (test_event) {
            case FGAUCHE:
                deplacement_tetrimino(&t, terrain, GAUCHE);
                break;
            case FBAS:
                deplacement_tetrimino(&t, terrain, BAS);
                break;
            case FDROIT:
                deplacement_tetrimino(&t, terrain, DROITE);
                break;
            case FHAUT:
                rotation(&t, terrain);
                break;
            case ESPACE:
                /* instachute */
                while (deplacement_tetrimino(&t, terrain, BAS)) {};
                geler_tetromino(&t, terrain);
                inserer_prochain_tetromino(&t, terrain, sac_tetromino, catalogue_tetromino, &tetromino_sac);
                break;
            }
        }



        /* GESTION DELAI AFFICHAGE */
        frame_delai = frame_attente - (SDL_GetTicks() - frame_debut);
        if (frame_delai > 0) {
            SDL_Delay(frame_delai);
        }
        frame_debut = SDL_GetTicks();
        /** FIN GESTION DELAI AFFICHAGE **/

        /** GESTION CHUTE TETROMINO **/
        current_time = SDL_GetTicks();
        if (current_time > (last_time + difficultee)) {
            if (!deplacement_tetrimino(&t, terrain, BAS)) {
                geler_tetromino(&t, terrain);
                inserer_prochain_tetromino(&t, terrain, sac_tetromino, catalogue_tetromino, &tetromino_sac);
            }
            last_time = current_time;
        }
        /** FIN GESTION CHUTE TETROMINO **/

        afficher_terrain(Fenetre, pRenderer, terrain, score, font);
    }
    TTF_CloseFont(font);
    free(t.coords);
    SDL_Quit();
    return 0;
}

void nettoyer_lignes(Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    Pile* p = creerPile();
    /* on commence par compter le nbr de ligne completes */
    for (int y = HAUTEUR_TERRAIN; y > 0; y--) {
        int complete = 1;
        for (int x = 0; x < LARGEUR_TERRAIN; x++) {
            if (terrain[x][y].valeur != 1) complete = 0;
        }
        if (complete) {
            empiler(p, y);
        }
    }



    /* on applique la gravitée pour combler la ligne manquante de haut en bas */
    if (p->nbrElements > 0) {
        switch (p->nbrElements) { //attribution du score
        case 1:
            score+=40;
            break;
        case 2:
            score+=100;
            break;
        case 3:
            score+=300;
            break;
        case 4:
            score+=1200;
            break;
        }
        int elements = p->nbrElements;
        for (int k = 0; k < elements; k++) {
            int ligne_y = depiler(p);
            for (int x = 0; x < LARGEUR_TERRAIN; x++)
                for (int y = ligne_y; y > 0; y--) {
                    terrain[x][y] = terrain[x][y-1];
                }
        }
    }
    detruirePile(p);
}

void geler_tetromino(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    for (int i = 0; i < t->nbr_coords; i++) {
        terrain[t->coords[i].x][t->coords[i].y].valeur = 1;
    }
    nettoyer_lignes(terrain);
}

int deplacement_tetrimino(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int direction) {
    verifier_mouvement_piece(t, terrain);
    if (t->direction_autorisee[direction] == 0) {
        return 0;
    }

    for (int i = 0; i < t->nbr_coords; i++) {
        /* on commence par retirer toute les cases de l'ancienne position */
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
    return 1;
}

void inserer_prochain_tetromino(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int sac_tetromino[NOMBRE_TETROMINO], Tetromino catalogue_tetromino[NOMBRE_TETROMINO], int* tetromino_sac) {
    if (*tetromino_sac >= NOMBRE_TETROMINO-1) { //le sac est vide, on le melange a nouveau
        choisir_sequence_tetromino(sac_tetromino);
        *tetromino_sac = 0;
    } else {
        (*tetromino_sac)++;
    }
    free(t->coords);
    *t = catalogue_tetromino[sac_tetromino[*tetromino_sac]]; // choisir le prochain tetromino du sac
    inserer_tetromino(t, terrain);
}

int rotation(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) { //nbr_rotation sert à définir combien de fois une rotation de 90° seras effectuée
    verifier_mouvement_piece(t, terrain);
    if ((t->pos.y + t->rayon_rotation >= HAUTEUR_TERRAIN+1) || (t->pos.x + t->rayon_rotation >= LARGEUR_TERRAIN+1) || (t->pos.x < 0)) { //prévenir les depassement de tableau
        return 0;
    }

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
    for (int y = t->pos.y; y < (t->pos.y + t->rayon_rotation); y++, i++) {
        int x = t->pos.x;
        j = 0;
        for (; x < (t->pos.x + t->rayon_rotation); x++, j++) {
            buffer_blocs[i][j] = (terrain[x][y].valeur == 1) ? 1 : 0;
            buffer_piece[i][j] = (terrain[x][y].valeur == 2) ? 2 : 0;
        }
    }

    int n = 1; //nombre de rotations
    int ok;
    do {

        /* copie de la rotation de la pièce dans le buffer rotat */
        ok = 1;
        i = 0;
        j = 0;
        for (int y = 0; y < t->rayon_rotation; y++, i++) {
            j = 0;
            for (int x = t->rayon_rotation-1; x > -1; x--, j++)  {
                buffer_rotat[i][j] = buffer_piece[x][y];
            }
        }

        // on copie le tableau buffer_rotat dans buffer pièce pour donner la posibilitée d'une autre rotation
        for (i = 0; i < t->rayon_rotation; i++) {
            for (int u = 0; u < t->rayon_rotation; u++) {
                buffer_piece[i][u] = buffer_rotat[i][u];
            }
        }

        /* vérification d'une obstruction de la pièce */
        for (i = 0; i < t->rayon_rotation; i++) {
            for (int y = 0; y < t->rayon_rotation; y++) {
                if (buffer_rotat[i][y] == 2 && buffer_blocs[i][y] == 1) { //la rotation ne peut avoir lieu, un bloc gène
                    if (n > 4)  //nombre max de tentative
                        return 0;
                    ok = 0;
                    n++;
                }

            }
        }
    } while (!ok);

    /* tout est bon, on retire l'ancienne pièce du terrain et place la nouvelle version */

    for (int y = t->pos.y; y < (t->pos.y + t->rayon_rotation); y++) { //retirer les anciens 2
        for (int x = t->pos.x; x < (t->pos.x + t->rayon_rotation); x++) {
            if (terrain[x][y].valeur == 2) supprimer_case(x, y, terrain);
        }
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
                t->coords[k] = (Coord) {
                    x, y
                }; //on update les coordonnées
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

void remplir_catalogue(Tetromino catalogue_tetromino[]) {
    FILE *fp;
    int nligne;
    char *ligne = NULL;
    size_t taille;
    if ((fp = fopen("tetrominos.csv", "r")) == NULL) {
        fprintf(stderr, "Erreur ! Le fichier \"tetromino\" est manquant ou inaccessible en "
                "lecture\n");
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < NOMBRE_TETROMINO; i++) {
            nligne = getline(&ligne, &taille, fp);
            if (nligne == -1) {
                fprintf(stderr, "Erreur ! Le nombre de tetrominos dans le fichier est plus "
                        "petit que le contenue de la macro NOMBRE_TETROMINO\n");
                exit(EXIT_FAILURE);
            }
            catalogue_tetromino[i].type = i;

            /* Remplir la matrice data[][] */
            int pos = 0;

            for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) {
                for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
                    catalogue_tetromino[i].data[x][y] =
                        ligne[pos] - '0'; // '-0' pour convertir char en int
                    pos++;
                }
            }

            char *next = strtok(ligne, ",");
            next = strtok(NULL, ","); // lire la valeur du rayon_rotation
            if (next == NULL) {
                fprintf(stderr, "Erreur fatale, le fichier tetromino est corrompu");
                exit(EXIT_FAILURE);
            }
            catalogue_tetromino[i].rayon_rotation = atoi(next);
            int rgb[3];
            for (int y = 0; y < 3; y++) {
                next = strtok(NULL, ","); // lire la couleur
                if (next == NULL) {
                    fprintf(stderr, "Erreur fatale, le fichier tetromino est corrompu");
                    exit(EXIT_FAILURE);
                }
                rgb[y] = atoi(next);
            }
            catalogue_tetromino[i].couleur = (SDL_Color) {
                rgb[0], rgb[1], rgb[2], 255
            };
            for (int y = 0; y < 3; y++) {
                catalogue_tetromino[i].direction_autorisee[y] = 1; // toute les directions autorisée par défaut
            }
        }
    }
    fclose(fp);
}

void choisir_sequence_tetromino(int sequence_tetromino[]) {
    int i;
    for (i = 0; i < NOMBRE_TETROMINO; i++) { // remplir le sac de pieces dans l'ordre
        sequence_tetromino[i] = i;
    }
    for (i = 0; i < NOMBRE_TETROMINO; i++) { // mélanger le sac
        int position_aleatoire = rand() % NOMBRE_TETROMINO;
        int tmp = sequence_tetromino[i];
        sequence_tetromino[i] = sequence_tetromino[position_aleatoire];
        sequence_tetromino[position_aleatoire] = tmp;
    }
}

int inserer_tetromino(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    int nbr_coords = 0;
    for (int x = 0; x < TAILLE_MAX_TETROMINO; x++) { //compte le nombre de block dans le tetromino pour allouer le tableau
        for (int y = 0; y < TAILLE_MAX_TETROMINO; y++) {
            if (t->data[x][y] == 2) {
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
                t->pos.x = x;
                t->pos.y = y;
            }
            if (t->data[y][x - LARGEUR_TERRAIN / 2] == 2) {
                if (terrain[x][y].valeur == 1) { // si un bloc fixe obstrue la pièce
                    game_over();
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
    exit(EXIT_SUCCESS);
}

void verifier_mouvement_piece(Tetromino *t,
                              Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
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
