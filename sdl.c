#include <SDL2/SDL.h>
#include <stdio.h>
#include "sdl.h"

void supprimer_case(int x, int y, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    if (x > LARGEUR_TERRAIN || x < 0 || y > HAUTEUR_TERRAIN || y < 0) {
        fprintf(stderr, "Erreur fatale : case a supprimer hors du terrain : (%d,%d)", x, y);
        exit(EXIT_FAILURE);
    }
    terrain[x][y].valeur = 0;
    terrain[x][y].couleur = (SDL_Color) {0, 0, 0, 0};
}

void dessiner_rectangle(SDL_Renderer* pRenderer, int x, int y, int w, int h, SDL_Color couleur) {
    SDL_Rect rectangle = {x, y, w, h};
    if (SDL_SetRenderDrawColor(pRenderer, couleur.r, couleur.g, couleur.b, couleur.a) < 0)
        error_sdl_launch("Impossible de dessiner la couleur");
    if (SDL_RenderFillRect(pRenderer, &rectangle) < 0)
        error_sdl_launch("Impossible de dessiner les blocs");
}

void afficher_terrain(SDL_Window* Fenetre, SDL_Renderer* pRenderer, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    if (Fenetre == NULL) {
        error_sdl_launch("Impossible de créer la fenêtre");
    }
    if (pRenderer == NULL) {
        error_sdl_launch("Impossible de créer le renderer");
    }
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer); //nettoyer l'écran

    int facteur = TAILLE_BLOCK + 5; //le facteur corespond à TAILLE_BLOCK + l'espacement désiré sur les blocks
    for (int y = 0; y<HAUTEUR_TERRAIN*facteur; y+=facteur) {
        for (int x = 0; x<LARGEUR_TERRAIN*facteur; x+=facteur) {
            //printf("%d",terrain[x/facteur][y/facteur].valeur); // DEBUG
            //printf(" %d,%d ",x/facteur, y/facteur); //DEBUG
            dessiner_rectangle(pRenderer, x, y, TAILLE_BLOCK, TAILLE_BLOCK, terrain[x/facteur][y/facteur].couleur);//diviser par le facteur pour que les valeurs aillent de 0 à 19
        }
        //printf("\n"); // DEBUG
    }
    SDL_RenderPresent(pRenderer);
    //printf("\n\n"); // DEBUG
}

void error_sdl_launch(char* message) {
    fprintf(stderr,"%s, %s\n", message, SDL_GetError());
    exit(EXIT_FAILURE);
}

int event_clavier(SDL_Event *event) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_KEYDOWN) {      
            int scancode = event->key.keysym.scancode;
            switch (scancode) {
            case SDL_SCANCODE_UP:
                return FHAUT;
                break;
                
            case SDL_SCANCODE_RIGHT:
                return FDROIT;
                break;
                
            case SDL_SCANCODE_DOWN:
                return FBAS;
                break;
                
            case SDL_SCANCODE_LEFT:
                return FGAUCHE;
                break;
                
            case SDL_SCANCODE_SPACE:
                return ESPACE;
                break;
            
            case SDL_SCANCODE_ESCAPE:
                printf("échap, Au revoir\n");
                exit(EXIT_SUCCESS);
                break;
            }
            
        }
        if (event->type  == SDL_QUIT) {
            printf("Au revoir\n");
            exit(EXIT_SUCCESS);
        }
    }
    return 777; //code aucune touches appuyées
}
