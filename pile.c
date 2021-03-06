#include <stdlib.h>
#include "pile.h"

Pile* creerPile() {
    Pile* p = (Pile*) malloc(sizeof(Pile));
    p->dernier = NULL;
    p->nbrElements = 0;
    return p;
}

void empiler(Pile* p, int valeur) {
    Element* e = (Element*) malloc(sizeof(Element));
    e->valeur = valeur;
    if (p->nbrElements != 0)
        e->precedent = p->dernier;
    else
        e->precedent = NULL;
    p->dernier = (Element*) e;
    (p->nbrElements)++;
}

int depiler(Pile* p) {
    if (p->nbrElements == 0) { //ne peut pas dépiler une pile vide
        exit(EXIT_FAILURE);
    }
    Element *dernier = p->dernier;
    int valeurDernier =  dernier->valeur;
    p->dernier = dernier->precedent;
    free(dernier);
    (p->nbrElements)--;
    return valeurDernier;
}

void detruirePile(Pile* p) {
    int nbrElements = p->nbrElements;
    for (int i = 0; i<nbrElements; i++) {
        depiler(p);
    }
    free(p);
}

