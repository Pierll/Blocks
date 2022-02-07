#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sdl.h"

void supprimer_case(int x, int y, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN]) {
    if (x > LARGEUR_TERRAIN || x < 0 || y > HAUTEUR_TERRAIN || y < 0) {
        fprintf(stderr, "Erreur fatale : case a supprimer hors du terrain : (%d,%d)\n", x, y);
        exit(EXIT_FAILURE);
    }
    terrain[x][y].valeur = 0;
    terrain[x][y].couleur = (SDL_Color) {
        0, 0, 0, 0
    };
}

void dessiner_rectangle(SDL_Renderer* pRenderer, int x, int y, int w, int h, SDL_Color couleur) {
    SDL_Rect rectangle = {x, y, w, h};
    if (SDL_SetRenderDrawColor(pRenderer, couleur.r, couleur.g, couleur.b, couleur.a) < 0)
        error_sdl_launch("Impossible de dessiner la couleur");
    if (SDL_RenderFillRect(pRenderer, &rectangle) < 0)
        error_sdl_launch("Impossible de dessiner les blocs");
}

void afficher_texte(SDL_Window* Fenetre, SDL_Renderer* pRenderer, TTF_Font *font, char* texte, SDL_Color couleur) {

}

void afficher_terrain(SDL_Window* Fenetre, SDL_Renderer* pRenderer, Case terrain[LARGEUR_TERRAIN][HAUTEUR_TERRAIN], int score, TTF_Font *font) {
    /* gestion score */
    char score_text[50] = {};
    char buf_score[50] = {};
    int decimales_score = 0;
    if (score != 0) {
        decimales_score = log10(score) + 1;
    } else {
        decimales_score = 1;
    }
    sprintf(buf_score, "%d", score); //convertis le score en char et le met dans le buffer
    strncat(score_text, "SCORE: ", 8);
    strncat(score_text, buf_score, decimales_score);
    //printf("texte score: %s\n", score_text);

    SDL_Color couleur = {255,255,255,255};

    SDL_Surface * surface = TTF_RenderText_Solid(font, score_text, couleur);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(pRenderer, surface);
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { 420, 0, texW, texH };

    /* dessiner le terrain */
    if (Fenetre == NULL) {
        error_sdl_launch("Impossible de créer la fenêtre");
    }
    if (pRenderer == NULL) {
        error_sdl_launch("Impossible de créer le renderer");
    }
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer); //nettoyer l'écran
    dessiner_rectangle(pRenderer, 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE, (SDL_Color) {
        48,48,48
    } ); //affichage du fond
    int facteur = TAILLE_BLOCK + 5; //le facteur corespond à TAILLE_BLOCK + l'espacement désiré sur les blocks
    int offset_x = 20; //le décalagage de l'abscisse du terrain
    int offset_y = 0;  //le décalagage de l'ordonnée du terrain
    for (int y = offset_y; y<(HAUTEUR_TERRAIN*facteur + offset_y); y+=facteur) {
        for (int x = offset_x; x<(LARGEUR_TERRAIN*facteur + offset_x); x+=facteur) {
            dessiner_rectangle(pRenderer, x+offset_x, y+offset_y, TAILLE_BLOCK, TAILLE_BLOCK, terrain[(x-offset_x)/facteur][(y-offset_y)/facteur].couleur);//diviser par le facteur pour que les valeurs aillent de 0 à 19
        }
    }
    SDL_RenderCopy(pRenderer, texture, NULL, &dstrect); //afficher le score
    SDL_RenderPresent(pRenderer);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

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
