#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#ifndef __BATAILLE_H__
#define __BATAILLE_H__

#include "bataille.c"


void createFiles(char creation, short numfiles);
void createQG();
void createAttaquant();
void useSemaphore(SEMBUF *structure,char type,unsigned short num);

#endif