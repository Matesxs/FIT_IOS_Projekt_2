//
// Created by matesxs on 4/9/21.
//

#ifndef IOS_PROJECT2_SHARED_RESOURCES_H
#define IOS_PROJECT2_SHARED_RESOURCES_H

#include <stdio.h>
#include <semaphore.h>

#include "static_constructions.h"

// Semaphors
extern sem_t *writeOutLock;
extern sem_t *rdWaitForHitch;
extern sem_t *rdHitched;
extern sem_t *getHelp;
extern sem_t *waitForHelp;
extern sem_t *elfHelped;
extern sem_t *wakeForHelp;
extern sem_t *wakeForHitch;
extern sem_t *santaReady;

// Shared memory
extern int shm_readyRDCount_id;
extern int *readyRDCount;
extern int shm_elfReadyQueue_id;
extern int *elfReadyQueue;
extern int shm_shopClosed_id;
extern int *shopClosed;
extern int shm_actionId_id;
extern int *actionId;
extern int shm_christmasStarted_id;
extern int *christmasStarted;

// Mic
extern Params params;
extern FILE *outputFile;

extern pid_t pid_mainprocess;

extern pid_t *elf_processes;
extern size_t elves_count;
extern ReturnCode globalElvesReturncode;
extern pid_t *rd_processes;
extern size_t rd_count;
extern pid_t santa_process;

#endif //IOS_PROJECT2_SHARED_RESOURCES_H
