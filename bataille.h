#include <stdio.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>

#ifndef __BATAILLE_H__
#define __BATAILLE_H__

#include "bataille.c"

void createFiles(char creation, short numfiles);
void useSemaphore(SEMBUF *structure,char type,unsigned short num);
void attaque(PID_FICHIERS *lectureFichier, int occurence);

#endif