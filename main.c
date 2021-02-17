#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define LECTURE 0
#define ECRITURE 1
#define MAX NULL

volatile sig_atomic_t print_flag = false;
int* tube[5][2];

void createFiles();
void createProcesses();
void createQG();
void createAttaquant();
void alarm_attaque();
void handle_alarm();

int main()
{
    // Lors de l'initialisation du programme on vérifie qu'on lance le programme qui sera chargé de créer les fichiers

    printf("-- Initialisation de la bataille de processus -- \n");
    createFiles();
    createProcesses();
    //handle_alarm();
}

// Creation de mes fichiers au début du programme

void createFiles(){
    int numfiles = 10;
    FILE *files[numfiles];

    printf("-- Création des fichiers en cours -- \n");

    for (int i = 0; i < numfiles; i++)
    {
        char filename[10];
        sprintf(filename, "F%d.txt", i);
        files[i] = fopen(filename, "w");
        printf("fichier %i est cree %li\n");
    }

    printf("-- Création des fichiers terminée -- \n");
}

// Création du processus général

void createProcesses(){

    // On créé un processus maitre

    int pmaster = fork();

    if(pmaster > 0){
        createQG(pmaster);
    }
    wait(100);
}

// Création de mon QG

void createQG(){

pid_t pidqg[2];

    for(int i=0;i<2;i++) // Je créé mes processus enfants
    {
        pidqg[i] = fork();
        if(pidqg[i] == 0)
        {
            createAttaquant();
            printf("[ process QG créé ] pid %d from [process game master] pid %d\n",getpid(),getppid());
            wait(MAX);
            exit(0);
        }

    }
}

// Création de mes attaquants

void createAttaquant(){

// Initialisation du pipe pour la communication entre processus

char LectureTube[15];
pid_t pidattaquant[5];

    // Itération pour création tube par attaquant

    for(int i=0; i<5 ;i++){
        pipe(tube[i]);
    }

    for(int i=0;i<5;i++){ // Je créé mes processus enfants

        pidattaquant[i] = fork();

            if(pidattaquant[i] == 0)
            {
                printf("Processus (pid=%d) créé par processus QG %d \n",getpid(),getppid());
                // On lit notre descripteur qui vient de Processus QG

                close(tube[i][ECRITURE]);

                while(1){
                  read(tube[i][LECTURE],LectureTube,15);
                  sleep(MAX);
                  printf("C'est le processus (pid=%d) qui a recu le message suivant %s du processus QG %d \n",getpid(),LectureTube,getppid());
                }

                close(tube[i][LECTURE]);
            }
    }

    /* On ecrit ici d'un QG vers un fils
    On initialise nos tubes TODO mettre tempo toutes les 20 secondes
    On ecrit le message de notre descripteur qui est destiné a chaque processus fils */

}

// On écrit au tube pour envoyer l'ordre d'attaque aux fils

void writeOrder(){

    for(int i=0;i<5;i++){
        close(tube[i][LECTURE]);
        write(tube[i][ECRITURE],"attaquez",15);
        close(tube[i][ECRITURE]);
    }
}

// Flag pour tempo alarme

void alarm_attaque( int sig ) {
    print_flag = true;
}

// Gestion du sig alarm toute les 20sec

void handle_alarm(){

    signal( SIGALRM, alarm_attaque );
    alarm( 20 );
    for (;;) {
        sleep( 20 );
        if ( print_flag ) {
            // On lance l'ordre d'attaque chaque 20 secondes
            writeOrder();
            print_flag = false;
            alarm( 20 );
        }
    }
}









