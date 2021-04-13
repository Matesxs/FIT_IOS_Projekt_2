/**
 * @file shared_resources.c
 * @author Martin Douša
 * @date April 2021
 * @brief Holder for all shared variables
 */

#include "shared_resources.h"

pid_t pid_mainprocess = 0;

sem_t *writeOutLock = NULL;
sem_t *rdWaitForHitch = NULL;
sem_t *rdHitched = NULL;
sem_t *getHelp = NULL;
sem_t *waitForHelp = NULL;
sem_t *elfHelped = NULL;
sem_t *wakeForHelp = NULL;
sem_t *wakeForHitch = NULL;

int shm_readyRDCount_id = 0;
int *readyRDCount = NULL;
int shm_elfReadyQueue_id = 0;
int *elfReadyQueue = NULL;
int shm_shopClosed_id = 0;
int *shopClosed = NULL;
int shm_actionId_id = 0;
int *actionId = NULL;

FILE *outputFile = NULL;