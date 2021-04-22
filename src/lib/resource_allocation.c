/**
 * @file resource_allocation.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle allocating resources for comunication between processes
 */

#define _GNU_SOURCE
#include "resource_allocation.h"

/**
 * @brief Create shared memory
 * 
 * @param size size of memory to allocate
 * @param retVal pointer to return code that will be returned based on previous state and success of this action
 * 
 * @return void pointer to allocated memory, NULL on fail
 */
void* createSharedMemory(size_t size, ReturnCode *retVal)
{
  void *mem = NULL;

  if ((mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
  {
    (*retVal) |= SM_CREATE_ERROR;
    return NULL;
  }

  return mem;
}

/**
 * @brief Create semaphore
 * 
 * @param defVal default value for semaphore
 * @param sem return pointer to pointer for semaphore
 * @param retVal pointer to return code that will be returned based on previous state and success of this action
 */
void initSemaphore(int defVal, sem_t *sem, ReturnCode *retVal)
{
  if (sem_init(sem, 1, defVal) == -1)
    (*retVal) |= SEMAPHOR_INIT_FAILED;
}

/**
 * @brief Destroy shared memory segment
 * 
 * @param memLink pointer to link of shared memory
 * @param size size of memory to destroy
 * @param retVal pointer to return code that will be returned based on previous state and success of this action
 */
void destroySharedMemory(void **memLink, size_t size, ReturnCode *retVal)
{
  if (*memLink == NULL) return;

  if (munmap(*memLink, size) == -1)
    (*retVal) |= SM_DESTROY_ERROR;

  *memLink = NULL;
}

/**
 * @brief Destroy existing semaphore
 * 
 * @param sem pointer to semaphore to destroy
 * @param retVal pointer to return code that will be returned based on previous state and success of this action
 */
void destroySemaphore(sem_t *sem, ReturnCode *retVal)
{
  if (sem_destroy(sem) == -1)
    (*retVal) |= SEMAPHOR_DESTROY_ERROR;
}

/**
 * @brief Deallocates all memory used by semaphores and shared memory
 *
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode deallocateResources()
{
  // Close output file
  if (outputFile != NULL)
  {
    fclose(outputFile);
    outputFile = NULL;
  }

  if (processHolder.elfIds != NULL)
  {
    free(processHolder.elfIds);
    processHolder.elfIds = NULL;
    processHolder.elvesCount = 0;
  }

  if (processHolder.rdIds != NULL)
  {
    free(processHolder.rdIds);
    processHolder.rdIds = NULL;
    processHolder.rdCount = 0;
  }

  ReturnCode retVal = NO_ERROR;

  // Destroy semafors
  destroySemaphore(&semHolder->writeOutLock, &retVal);
  destroySemaphore(&semHolder->rdWaitForHitch, &retVal);
  destroySemaphore(&semHolder->rdHitched, &retVal);
  destroySemaphore(&semHolder->waitForHelp, &retVal);
  destroySemaphore(&semHolder->waitInQueue, &retVal);
  destroySemaphore(&semHolder->elfHelped, &retVal);
  destroySemaphore(&semHolder->wakeForHelp, &retVal);
  destroySemaphore(&semHolder->santaReady, &retVal);
  destroySemaphore(&semHolder->childFinished, &retVal);
  destroySemaphore(&semHolder->rdReadyCountMutex, &retVal);
  destroySemaphore(&semHolder->elfQueueMutex, &retVal);
  destroySemaphore(&semHolder->christmasStarted, &retVal);
  destroySemaphore(&semHolder->numOfElvesStable, &retVal);

  destroySharedMemory((void**)&semHolder, sizeof(SemHolder), &retVal);

  // Destroy shared memory
  destroySharedMemory((void**)&sharedMemory, sizeof(SharedMemory), &retVal);

  if (retVal != NO_ERROR)
    return retVal;

  return NO_ERROR;
}

/**
 * @brief Allocates all needed semaphores and shared memory for running program
 *
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode allocateResources()
{
  ReturnCode retVal = NO_ERROR;

  // Create semaphores
  semHolder = createSharedMemory(sizeof(SemHolder), &retVal);
  if (retVal != NO_ERROR) return retVal;

  initSemaphore(1, &semHolder->writeOutLock, &retVal);
  initSemaphore(0, &semHolder->rdWaitForHitch, &retVal);
  initSemaphore(0, &semHolder->rdHitched, &retVal);
  initSemaphore(0, &semHolder->waitForHelp, &retVal);
  initSemaphore(3, &semHolder->waitInQueue, &retVal);
  initSemaphore(0, &semHolder->elfHelped, &retVal);
  initSemaphore(0, &semHolder->wakeForHelp, &retVal);
  initSemaphore(0, &semHolder->santaReady, &retVal);
  initSemaphore(0, &semHolder->childFinished, &retVal);
  initSemaphore(1, &semHolder->rdReadyCountMutex, &retVal);
  initSemaphore(1, &semHolder->elfQueueMutex, &retVal);
  initSemaphore(0, &semHolder->christmasStarted, &retVal);
  initSemaphore(1, &semHolder->numOfElvesStable, &retVal);

  if (retVal != NO_ERROR) return retVal;

  // Create shared memory
  sharedMemory = createSharedMemory(sizeof(SharedMemory), &retVal);
  if (retVal != NO_ERROR) return retVal;

  // Init shared memory
  sharedMemory->readyRDCount = 0;
  sharedMemory->elfReadyQueue = 0;
  sharedMemory->numberOfElves = 0;
  sharedMemory->shopClosed = false;
  sharedMemory->actionId = 1;

  return NO_ERROR;
}
