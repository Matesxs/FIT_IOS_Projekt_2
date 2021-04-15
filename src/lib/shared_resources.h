//
// Created by matesxs on 4/9/21.
//

#ifndef IOS_PROJECT2_SHARED_RESOURCES_H
#define IOS_PROJECT2_SHARED_RESOURCES_H

#include <stdio.h>
#include <semaphore.h>

// Semaphors
extern sem_t *writeOutLock;
extern sem_t *rdWaitForHitch;
extern sem_t *rdHitched;
extern sem_t *getHelp;
extern sem_t *waitForHelp;
extern sem_t *elfHelped;
extern sem_t *wakeForHelp;
extern sem_t *wakeForHitch;

// Shared memory
extern int shm_readyRDCount_id;
extern int *readyRDCount;
extern int shm_elfReadyQueue_id;
extern int *elfReadyQueue;
extern int shm_shopClosed_id;
extern int *shopClosed;
extern int shm_actionId_id;
extern int *actionId;

// Mic
extern FILE *outputFile;
extern pid_t pid_mainprocess;

#endif //IOS_PROJECT2_SHARED_RESOURCES_H