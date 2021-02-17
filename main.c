#include "bataille.h"

int main()
{
    // Lors de l'initialisation du programme on vérifie qu'on lance le programme qui sera chargé de créer les fichiers

    printf("-- Initialisation de la bataille de processus -- \n");
    createFiles('C',NB_FICHIER);
    sleep(10);
    createFiles('S',NB_FICHIER);
}
