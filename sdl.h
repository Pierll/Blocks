#ifndef SDL
#define SDL

#define LARGEUR_TERRAIN 10
#define HAUTEUR_TERRAIN 20
#define TAILLE_BLOCK 17 /* la taille d'un bloc (case) en pixels */
#define FPS 60

enum touches{FBAS,FGAUCHE,FHAUT,FDROIT,ESPACE};

typedef struct Case {
	int valeur; //0 = espace vide, 1 = bloc fixe, 2 = pièce en mouvement, 
	SDL_Color couleur;
} Case;

void supprimer_case(int x, int y, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
void afficher_terrain(SDL_Window* Fenetre, SDL_Renderer* pRenderer, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]);
void error_sdl_launch(char *message);
void dessiner_rectangle(SDL_Renderer* pRenderer, int x, int y, int w, int h, SDL_Color couleur);
int event_clavier(SDL_Event *event);

#endif
