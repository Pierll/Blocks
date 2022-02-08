typedef struct Element {
	void* precedent;
	int valeur;
} Element;

typedef struct Pile {
	void* dernier;
    int nbrElements;
} Pile;

Pile* creerPile();
void empiler(Pile* p, int valeur);
int depiler(Pile* p);
void detruirePile(Pile* p);