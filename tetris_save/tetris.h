#ifndef TETRIS
#define TETRIS

#define TAILLE_MAX_TETROMINO 4
#define NOMBRE_TETROMINO 7

enum type_piece{I, J, L, O, S, T, Z};
enum direction{GAUCHE,BAS,DROITE};

typedef struct Tetromino {
	int type;
	int x; //interne
	int y; //interne
	//int extremite_x; //emplacement de l'extremité droite et bas de la pièce dans sa matrice data
	//int extremite_y; // "" haute
	int data[TAILLE_MAX_TETROMINO][TAILLE_MAX_TETROMINO];
	int rayon_rotation;
	int etat_rotation; //angle (1=0°,2= 90°,3=180°,4=270°)
	int direction_autorisee[2]; //voir enum direction
	SDL_Color couleur;
} Tetromino;

void remplir_catalogue(Tetromino catalogue_tetromino[]);
void choisir_sequence_tetromino(int sequence_tetromino[]);
int inserer_tetromino(Tetromino* t, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
void rotation_tetromino(Tetromino* t, int angle);
void game_over();

#endif
