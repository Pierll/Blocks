#ifndef BLOCKS
#define BLOCKS

#define TAILLE_MAX_TETROMINO 4
#define NOMBRE_TETROMINO 7

enum type_piece{I, J, L, O, S, T, Z};
enum direction{GAUCHE,BAS,DROITE,HAUT};

typedef struct Coord {
	int x;
	int y;
} Coord;

typedef struct Tetromino {
	int type;
	Coord pos; //coordonnée à partir duquel la zone entournat la pièce est déterminée
	Coord *coords; //tableu de coordonnées à allouer
	int nbr_coords; //taille du tableau de coords
	int data[TAILLE_MAX_TETROMINO][TAILLE_MAX_TETROMINO]; //4 matrices représentants les 4 rotations de la pièce
	int rayon_rotation;
	int direction_autorisee[4]; //voir enum direction
	SDL_Color couleur;
} Tetromino;

void geler_tetromino(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
int deplacement_tetrimino(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int direction);
void inserer_prochain_tetromino(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int sac_tetromino[NOMBRE_TETROMINO], Tetromino catalogue_tetromino[NOMBRE_TETROMINO], int* tetromino_sac);
int rotation(Tetromino *t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
void remplir_catalogue(Tetromino catalogue_tetromino[]);
void choisir_sequence_tetromino(int sequence_tetromino[]);
int inserer_tetromino(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
void verifier_mouvement_piece(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]); 
void game_over();

#endif
