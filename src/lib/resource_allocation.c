/**
 * @file resource_allocation.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle allocating resources for comunication between processes
 */

#define _GNU_SOURCE
#include "resource_allocation.h"

#define SEM_ERR (void*)-1 /**< Error return value for shared memory mapping */

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
void createSemaphore(int defVal, sem_t **sem, ReturnCode *retVal)
{
  (*sem) = createSharedMemory(sizeof(sem_t), retVal);

  if (sem_init(*sem, 1, defVal) == -1)
    (*retVal) |= SEMAPHOR_INIT_FAILED;
}

/**
 * @brief Destroy shared memory segment
 * 
 * @param memLink pointer to link of shared memory
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
void destroySemaphore(sem_t **sem, ReturnCode *retVal)
{
  if (*sem == NULL) return;

  if (sem_destroy(*sem) == -1)
    (*retVal) |= SEMAPHOR_DESTROY_ERROR;

  destroySharedMemory((void**)sem, sizeof(sem), retVal);

  *sem = NULL;
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

  if (elf_processes != NULL)
  {
    free(elf_processes);
    elf_processes = NULL;
    elves_count = 0;
  }

  if (rd_processes != NULL)
  {
    free(rd_processes);
    rd_processes = NULL;
    rd_count = 0;
  }

  ReturnCode retVal = NO_ERROR;

  // Destroy semafors
  destroySemaphore(&writeOutLock, &retVal);
  destroySemaphore(&rdWaitForHitch, &retVal);
  destroySemaphore(&rdHitched, &retVal);
  destroySemaphore(&getHelp, &retVal);
  destroySemaphore(&waitForHelp, &retVal);
  destroySemaphore(&elfHelped, &retVal);
  destroySemaphore(&wakeForHelp, &retVal);
  destroySemaphore(&wakeForHitch, &retVal);
  destroySemaphore(&santaReady, &retVal);
  destroySemaphore(&santaFinished, &retVal);
  destroySemaphore(&elfFinished, &retVal);
  destroySemaphore(&rdFinished, &retVal);

  // Destroy shared memory
  destroySharedMemory((void**)&readyRDCount, sizeof(int), &retVal);
  destroySharedMemory((void**)&elfReadyQueue, sizeof(int), &retVal);
  destroySharedMemory((void**)&shopClosed, sizeof(int), &retVal);
  destroySharedMemory((void**)&actionId, sizeof(int), &retVal);
  destroySharedMemory((void**)&christmasStarted, sizeof(int), &retVal);

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
  createSemaphore(1, &writeOutLock, &retVal);
  createSemaphore(0, &rdWaitForHitch, &retVal);
  createSemaphore(0, &rdHitched, &retVal);
  createSemaphore(0, &getHelp, &retVal);
  createSemaphore(3, &waitForHelp, &retVal);
  createSemaphore(0, &elfHelped, &retVal);
  createSemaphore(0, &wakeForHelp, &retVal);
  createSemaphore(0, &wakeForHitch, &retVal);
  createSemaphore(1, &santaReady, &retVal);
  createSemaphore(0, &santaFinished, &retVal);
  createSemaphore(0, &elfFinished, &retVal);
  createSemaphore(0, &rdFinished, &retVal);

  if (retVal != NO_ERROR) return retVal;

  // Create shared memory
  readyRDCount = createSharedMemory(sizeof(int), &retVal);
  elfReadyQueue = createSharedMemory(sizeof(int), &retVal);
  shopClosed = createSharedMemory(sizeof(int), &retVal);
  actionId = createSharedMemory(sizeof(int), &retVal);
  christmasStarted = createSharedMemory(sizeof(int), &retVal);

  if (retVal != NO_ERROR) return retVal;

  // Init shared memory
  *readyRDCount = 0;
  *elfReadyQueue = 0;
  *shopClosed = 0;
  *actionId = 1;
  *christmasStarted = 0;

  return NO_ERROR;
}
