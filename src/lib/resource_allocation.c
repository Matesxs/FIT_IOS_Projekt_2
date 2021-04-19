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
 * @brief Destroy existing semaphore
 * 
 * @param sem pointer to semaphore to destroy
 * @param retVal return code that will be returned based on previous state and success of this action
 */
void destroySemaphore(sem_t **sem, ReturnCode *retVal)
{
  if (*sem == NULL) return;

  if (sem_destroy(*sem) == -1)
    (*retVal) |= SEMAPHOR_DESTROY_ERROR;

  *sem = NULL;
}

void destroySharedMemory(int memId, void **memLink, ReturnCode *retVal)
{
  // Deallocate
  if (shmctl(memId, IPC_RMID, NULL) == -1)
    (*retVal) |= SM_DESTROY_ERROR;

  // Unlink
  if (shmdt(*memLink) == -1)
    (*retVal) |= SM_UNLINK_ERROR;

  *memLink = NULL;
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
  destroySharedMemory(shm_readyRDCount_id, (void**)&readyRDCount, &retVal);
  destroySharedMemory(shm_elfReadyQueue_id, (void**)&elfReadyQueue, &retVal);
  destroySharedMemory(shm_shopClosed_id, (void**)&shopClosed, &retVal);
  destroySharedMemory(shm_actionId_id, (void**)&actionId, &retVal);
  destroySharedMemory(shm_christmasStarted_id, (void**)&christmasStarted, &retVal);

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
  // Allocate semaphors
  if ((writeOutLock = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (rdWaitForHitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (rdHitched = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (getHelp = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (waitForHelp = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (elfHelped = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (wakeForHelp = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (wakeForHitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (santaReady = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (santaFinished = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (elfFinished = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED || 
      (rdFinished = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
  {
    return SEMAPHOR_CREATION_ERROR;
  }

  // Init semaphors
  if (sem_init(writeOutLock, 1, 1) == -1 ||
      sem_init(rdWaitForHitch, 1, 0) == -1 ||
      sem_init(rdHitched, 1, 0) == -1 ||
      sem_init(getHelp, 1, 0) == -1 ||
      sem_init(waitForHelp, 1, 3) == -1 ||
      sem_init(elfHelped, 1, 0) == -1 ||
      sem_init(wakeForHelp, 1, 0) == -1 ||
      sem_init(wakeForHitch, 1, 0) == -1 ||
      sem_init(santaReady, 1, 1) == -1 ||
      sem_init(santaFinished, 1, 0) == -1 ||
      sem_init(elfFinished, 1, 0) == -1 ||
      sem_init(rdFinished, 1, 0) == -1)
  {
    return SEMAPHOR_INIT_FAILED;
  }

  // Create shared memory
  if ((shm_readyRDCount_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_elfReadyQueue_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_shopClosed_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_actionId_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_christmasStarted_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)
  {
    return SM_CREATE_ERROR;
  }

  // Map shared memory
  if ((readyRDCount = (int*)shmat(shm_readyRDCount_id, NULL, 0)) == SEM_ERR ||
      (elfReadyQueue = (int*)shmat(shm_elfReadyQueue_id, NULL, 0)) == SEM_ERR ||
      (shopClosed = (int*)shmat(shm_shopClosed_id, NULL, 0)) == SEM_ERR ||
      (actionId = (int*)shmat(shm_actionId_id, NULL, 0)) == SEM_ERR ||
      (christmasStarted = (int*)shmat(shm_christmasStarted_id, NULL, 0)) == SEM_ERR)
  {
    return SM_MAP_ERROR;
  }

  return NO_ERROR;
}
