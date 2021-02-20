#define LECTURE 0
#define ECRITURE 1
#define NB_ATTAQUANT 4
#define NB_FICHIER 2
#define TPS 2

//Variables globales
key_t SEMAPHORE_KEY=0x2021;

typedef struct{
    int equipe;
    int attaquant;
}PID_FICHIERS;

//Création du type SEMBUF
typedef struct {
	unsigned short sem_num;
	short sem_op;
	short sem_flg
}SEMBUF;

//Déclaration de la structure de contrôle des sémaphores pour les fichiers
SEMBUF semaphoresFichiers;
PID_FICHIERS recupFichier;

int semaphoresId;

// **** CODES DES FONCTIONS ****

// Creation de mes fichiers au début du programme

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

void attaque(int occurence){

    char nomFichier[15];
    pid_t pidRecup;
    FILE *fichierBinaire;
    sprintf(nomFichier,"F%d.bin",occurence);
    int pidEquipe=0;
    int pidAttaquant=0;
    
    printf("P\t%d\n",getpid());
    useSemaphore(&semaphoresFichiers,'P',occurence);
    semop(semaphoresId,&semaphoresFichiers,1);

    fichierBinaire=fopen(nomFichier,"rb");
    //On se déplace au niveau du PID
    fseek(fichierBinaire,0,SEEK_SET);
    fread(&pidEquipe,sizeof(int),1,fichierBinaire);
    fread(&pidAttaquant,sizeof(int),1,fichierBinaire);
    fclose(fichierBinaire);

    fichierBinaire=fopen(nomFichier,"wb");
    fseek(fichierBinaire,0,SEEK_SET);
    pidRecup=getppid();
    fwrite(&pidRecup,sizeof(pidRecup),1,fichierBinaire);
    pidRecup=getpid();
    fwrite(&pidRecup,sizeof(pidRecup),1,fichierBinaire);
    fclose(fichierBinaire);

    if(getppid()!=pidEquipe && pidEquipe>0 && pidAttaquant>0){
        kill(pidAttaquant,SIGQUIT);
    }
    
    sleep(TPS);
    printf("V\t%d\n",getpid());

    useSemaphore(&semaphoresFichiers,'V',occurence);
    semop(semaphoresId,&semaphoresFichiers,(unsigned short)1);

   //Pas fini
}