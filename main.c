#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define LECTURE 0
#define ECRITURE 1
#define MAX NULL
#define NBFICHIER 10
#define TPS 2

//Variables globales
key_t SEMAPHORE_KEY=0x2021;

//Création du type SEMBUF
typedef struct {
	unsigned short sem_num;
	short sem_op;
	short sem_flg
}SEMBUF;

//Déclaration de la structure de contrôle des sémaphores pour les fichiers

SEMBUF semaphoresFichiers;

int semaphoresId;

FILE *fptr;
int tube[5][2];
int f;

void createFiles();
void createProcesses();
void createQG();
void createAttaquant();
void alarm_attaque();
void writeOrder();
void writeFiles();
void readFilesAndKill();

struct identityPID
{
   int n1,n2;
};

int main()
{
    // Lors de l'initialisation du programme on vérifie qu'on lance le programme qui sera chargé de créer les fichiers

    printf("-- Initialisation de la bataille de processus -- \n");
    createFiles();
    createProcesses();
}

// Creation de mes fichiers au début du programme

void createFiles(){

    FILE *files[NBFICHIER];

    printf("-- Création des fichiers en cours -- \n");

    for (int i = 0; i < NBFICHIER; i++)
    {
        char filename[NBFICHIER];
        sprintf(filename, "F%d.bin", i);
        files[i] = fopen(filename, "wb");
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
int j=0;

    for(int i=0;i<2;i++) // Je créé mes processus enfants
    {
        pidqg[i] = fork();
        if(pidqg[i] == 0)
        {
            createAttaquant();

            close(tube[i][LECTURE]);
            for(;;){
                sleep(2);
                writeOrder(j);
                j++;
            }
            close(tube[i][ECRITURE]);

            printf("[ process QG créé ] pid %d from [process game master] pid %d\n",getpid(),getppid());
            wait(2);
            exit(0);
        }

    }
}

// Création de mes attaquants

void createAttaquant(){

// Initialisation du pipe pour la communication entre processus

char LectureTube[25];
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

                for(;;){
                  read(tube[i][LECTURE],&LectureTube,25);
                  printf("C'est le processus (pid=%d) qui a recu le message suivant %s du processus QG %d \n",getpid(),LectureTube,getppid());
                  writeFile(LectureTube);
                  /* Futur partie code de l'attaque ecriture fichier */
                }

                close(tube[i][LECTURE]);
            }

    }

    /* On ecrit ici d'un QG vers un fils
    On initialise nos tubes TODO mettre tempo toutes les 20 secondes
    On ecrit le message de notre descripteur qui est destiné a chaque processus fils */

}


// Gestion du sig alarm toute les 20sec

void writeOrder(int j){

   char buff[15];
   int nbattaquant=5;
   srand(time(NULL));
   for(int i=0;i<nbattaquant;i++){
        int nRandonNumber = rand()%((NBFICHIER+1)-0) + 0;
        sprintf(buff,"F%d.bin",nRandonNumber);
        write(tube[i][ECRITURE],buff,15);
   }

}


void readFilesAndKill(char *filepath){

   int n;
   struct identityPID num;

   // Si le fichier n'existe pas on affiche un erreur

   if ((fptr = fopen(filepath,"rb")) == NULL){
       printf("Ce fichier n'existe pas !!");
       exit(1);
   }

   for(n = 1; n < 5; ++n){
      fread(&num, sizeof(struct identityPID), 1, fptr);
      //printf("PID RECUPERE: %d NUMERO d'EQUIPE: %d\n", num.n1, num.n2);

      // Tuer le processus ennemi

      if(num.n2 != getppid() && num.n2 > 0){
         printf("ON TUE LE PROC ENNEMI %d \n", num.n1);
      }

   }
   fclose(fptr);

   return 0;

}


void writeFile(char *filepath){

   struct identityPID num;
   FILE *fptr;

   if ((fptr = fopen(filepath,"wb")) == NULL){
       printf("Ce fichier n'existe pas !!!");
       exit(1);
   }

   //printf("Ecriture dans le fichier %s par %d de l'equipe %d",filepath,getpid(),getppid());

   num.n1 = getpid();
   num.n2 = getppid();

   for(int n = 1; n < 5; ++n)
   {
      num.n1 = getpid();
      num.n2 = getppid();

      fwrite(&num, sizeof(struct identityPID), 1, fptr);
   }
   fclose(fptr);
   readFilesAndKill(filepath);
   return 0;
}








