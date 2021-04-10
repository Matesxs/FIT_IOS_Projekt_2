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
sem_t *elfWaitForHelp = NULL;
sem_t *elfHelped = NULL;

int shm_readyRDCount_id = 0;
int *readyRDCount = NULL;
int shm_elfQueue_id = 0;
int *elfQueue = NULL;
int shm_elfsHelped_id = 0;
int *elfsHelped = NULL;
int shm_shopClosed_id = 0;
int *shopClosed = NULL;
int shm_actionId_id = 0;
int *actionId = NULL;

FILE *outputFile = NULL;