#define LECTURE 0
#define ECRITURE 1
#define NB_ATTAQUANT 5
#define NB_FICHIER 100

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

// **** CODES DES FONCTIONS ****

// Creation de mes fichiers au début du programme

void createFiles(char creation, short numfiles){
   
	int i=0; //Variable utilisée pour les boucles
    FILE *files[numfiles];
    char filename[10];

    for (i = 0; i < numfiles; i++){
        
       
        sprintf(filename, "F%d.txt", i);
        
        //Création des fichiers
        if (creation=='C'){
            files[i] = fopen(filename, "w");
            //printf("Fichier %i est cree %li\n");
            printf("Fichier %i créé\n",i);
            fclose(files[i]);
        }

        //Suppression des fichiers
        else if(creation == 'S'){
            remove(filename);
            //printf("Fichier %i est supprimé %li\n");
            printf("Fichier %i supprimé\n",i);
        }
                
    }

    //Partie sémaphores

    //Création
    if(creation=='C'){
        semaphoresId=semget(SEMAPHORE_KEY,(int)numfiles, 750 | IPC_CREAT | IPC_EXCL);
        semctl(semaphoresId, 0, SETVAL,0);
    }

    //Destruction
    else if(creation=='S'){
        semctl(semaphoresId,0,IPC_RMID,0);
    }
    
}



void createQG(){
	int i=0; //Variable utilisée pour les boucles

	pid_t pidqg[2];

    for(i=0;i<2;i++) // Je créé mes processus enfants
    {
        pidqg[i] = fork();
        if(pidqg[i] == 0)
        {
            //createAttaquant();
            printf("[ process QG créé ] pid %d from [process game master] pid %d\n",getpid(),getppid());
            wait(NULL);
            exit(0);
        }

    }
}


void createAttaquant(){

	// Initialisation du pipe pour la communication entre processus
	char LectureTube[15];
	int tube[5][2];
	pid_t pidattaquant[NB_ATTAQUANT];
	int i=0; //Variable utilisée pour les boucles

    for(i=0; i<NB_ATTAQUANT ;i++){
        pipe(tube[i]);
    }

    for(i=0;i<NB_ATTAQUANT;i++){ // Je créé mes processus enfants

        pidattaquant[i] = fork();

            if(pidattaquant[i] == 0)
            {
                //printf("[ process Attaquant créé ] pid %d sous les ordre du  [process QG ] %d\n",getpid(),getppid());

                // On lit notre descripteur qui vient de Processus QG

                //close(tube[i][ECRITURE]);

                
                read(tube[i][LECTURE],LectureTube,15);
                printf("C'est le processus (pid=%d) qui a recu le message suivant %s du processus QG %d \n",getpid(),LectureTube,getppid());
                pause();
                exit(0);
                

                //close(tube[i][LECTURE]);

            }
    }

    /* On ecrit ici d'un QG vers un fils
    On initialise nos tubes TODO mettre tempo toutes les 20 secondes
    On ecrit le message de notre descripteur qui est destiné a chaque processus fils */

    /*for(i=0;i<5;i++){

        close(tube[i][LECTURE]);
        write(tube[i][ECRITURE],"attaquez",15);
        close(tube[i][ECRITURE]);
    }*/
}

//void createFileSemaphore procédure à écrire pour la gestion des sémaphores.
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