#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

//Variables globales

#define LECTURE 0
#define ECRITURE 1
#define MAX NULL
#define NBFICHIER 10000
#define TPS 2
#define NBTUBE 5

// Notre clé pour le sémaphore

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


// Equipe tube 1
int tube1[5][2];

// Equipe tube 2
int tube2[5][2];

int f;

// Prototype de nos fonctions

void createFiles(char creation, short numfiles);
void createQG();
void createAttaquant(int equipe);
void writeOrder(int j,int equipe);
void processingFile(int occurence);
void alarm_handler();


int main()
{
    // Lors de l'initialisation du programme on vérifie qu'on lance le programme qui sera chargé de créer les fichiers

    printf("-- Initialisation de la bataille de processus %d -- \n",getpid());

    // Gestion du signal Alarm du QG au bout de 20sec fin de la bataille

    signal(SIGALRM,alarm_handler);
    alarm(20);

    createQG();
    wait(NULL);
}

// On tue nos processus et on envoie le sig alarm

void alarm_handler(){
    kill(0,14);
    exit(0);
}

// Fonction de création de nos fichiers

void createFiles(char creation, short numfiles){

	int i=0; //Variable utilisée pour les boucles
    FILE *files[numfiles];
    char filename[10];

    for (i = 0; i < numfiles; i++){


        sprintf(filename, "F%d.bin", i);

        //Création des fichiers
        if (creation=='C'){
            files[i] = fopen(filename, "w");
            fclose(files[i]);
            printf("Fichier:\t%d créé\n",i);
        }

        //Suppression des fichiers
        else if(creation == 'S'){
            printf("Fichier :\t%d supprimé\n",i);
            remove(filename);
        }

    }

    //Partie sémaphores

    //Création
    if(creation=='C'){
        semaphoresId=semget(SEMAPHORE_KEY,(int)numfiles, 750 | IPC_CREAT | IPC_EXCL);
        for(i=0;i<numfiles;i++){
            semctl(semaphoresId, i, SETVAL,1);
        }

    }

    //Destruction
    else if(creation=='S'){
        semctl(semaphoresId,i,IPC_RMID,0);
    }

}

//void createFileSemaphore procédure pour la gestion des sémaphores.

void useSemaphore(SEMBUF *structure,char type,unsigned short num){
	structure->sem_num=num;
	if(type=='P'){
		structure->sem_op=-1;
	}

	else if (type=='V'){
		structure->sem_op=1;
	}

	structure->sem_flg=0;
}


// Création de mon QG

void createQG(){

pid_t pidqg[2];
int j=0;

    // Itération pour création tube par attaquant

    for(int i=0; i<NBTUBE ;i++){
        pipe(tube1[i]);
        pipe(tube2[i]);
    }


    for(int i=0;i<2;i++) // Je créé mes processus enfants et mes tubes
    {
        pidqg[i] = fork();
        if(pidqg[i] == 0)
        {
            createAttaquant(i);

            close(tube1[i][LECTURE]);
            close(tube2[i][LECTURE]);
            for(;;){
                sleep(2);
                writeOrder(j,i);
                j++;
            }
            close(tube1[i][ECRITURE]);
            close(tube2[i][ECRITURE]);
            printf("[ process QG créé ] pid %d from [process game master] pid %d\n",getpid(),getppid());
        }
    }
    // On créé nos fichiers
     createFiles('C',(short)NBFICHIER);
}

// Création de mes attaquants

void createAttaquant(int equipe){

// Initialisation du pipe pour la communication entre processus

char LectureTube[25];
pid_t pidattaquant[5];

    for(int i=0;i<5;i++){ // Je créé mes processus enfants qui seront les attaquants

        pidattaquant[i] = fork();

            if(pidattaquant[i] == 0)
            {
                printf("Processus (pid=%d) créé par processus QG %d \n",getpid(),getppid());

                // On lit nos descripteurs pour chaque equipe qui vient des Processus QG

                if(equipe == 0){
                    close(tube1[i][ECRITURE]);
                }else{
                    close(tube2[i][ECRITURE]);
                }
                // Boucle infinie pour la lecture dans tube
                for(;;){
                    if(equipe == 0){
                        read(tube1[i][LECTURE],&LectureTube,25);
                    }else{
                        read(tube2[i][LECTURE],&LectureTube,25);
                    }
                  printf("C'est le processus (pid=%d) qui a recu le message suivant %s du processus QG %d \n",getpid(),LectureTube,getppid());

                  // On passe lecture tube en parametre pour l'ordre de traitement fichier
                  processingFile(atoi(LectureTube));
                }
                if(equipe == 0){
                    close(tube1[i][LECTURE]);
                }else{
                    close(tube2[i][LECTURE]);
                }
            }

    }

}


// Fonction qui ecrit les ordres QG dans les tubes de chaque equipes en generant aleatoirement les numéros de fichiers a attaquer


void writeOrder(int j,int equipe){

   char buff[15];
   int nbattaquant=5;
   srand(time(NULL));
   for(int i=0;i<nbattaquant;i++){
        int nRandonNumber = rand()%((NBFICHIER+1)-0) + 0;
        sprintf(buff,"%d",nRandonNumber);
        if(equipe == 0){
            write(tube1[i][ECRITURE],buff,15);
        }else{
            write(tube2[i][ECRITURE],buff,15);
        }
   }

}


// Fonction qui lit dans les fichiers + écrit son PID
// Si un PID est trouvé on le kill

void processingFile(int occurence){

    char nomFichier[15];

    pid_t pidRecup;
    FILE *fichierBinaire;
    sprintf(nomFichier,"F%d.bin",occurence);

    int pidEquipe=0;
    int pidAttaquant=0;

    // Blocage a l'aide de semaphores pour exclusion mutuelle

    printf("P\t%d\n",getpid());

    useSemaphore(&semaphoresFichiers,'P',occurence);
    semop(semaphoresId,&semaphoresFichiers,1);

    // Ouverture des fichiers binaires

    fichierBinaire=fopen(nomFichier,"rb");

    //On se déplace au niveau du PID

    fseek(fichierBinaire,0,SEEK_SET);

    fread(&pidEquipe,sizeof(int),1,fichierBinaire);
    fread(&pidAttaquant,sizeof(int),1,fichierBinaire);

    fclose(fichierBinaire);

    // Apres avoir lu on ecrit dans le fichier recupéré

    fichierBinaire=fopen(nomFichier,"wb");
    fseek(fichierBinaire,0,SEEK_SET);

    // Ecriture

    pidRecup=getpid();

    fwrite(&pidRecup,sizeof(pidRecup),1,fichierBinaire);

    fclose(fichierBinaire);

    // On tue les processus de l'équipe opposée

    if(getppid()!=pidEquipe && pidEquipe>0 && pidAttaquant>0){
        kill(pidAttaquant,SIGQUIT);
    }

    // Tempo pour affichage console
    sleep(TPS);
    printf("V\t%d\n",getpid());

    useSemaphore(&semaphoresFichiers,'V',occurence);
    semop(semaphoresId,&semaphoresFichiers,(unsigned short)1);
}








