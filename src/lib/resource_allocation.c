/**
 * @file resource_allocation.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle allocating resources for comunication between processes
 */

#define _GNU_SOURCE
#include "resource_allocation.h"

#define SEM_ERR (void*)-1

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
  }

  ReturnCode retVal = NO_ERROR;

  // Destroy semafors
  if (sem_destroy(writeOutLock) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(rdWaitForHitch) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(rdHitched) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(getHelp) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(waitForHelp) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(elfHelped) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(wakeForHelp) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;
  if (sem_destroy(wakeForHitch) == -1)
    retVal |= SEMAPHOR_DESTROY_ERROR;

  // Deallocate shared memory
  if (shmctl(shm_readyRDCount_id, IPC_RMID, NULL) == -1)
    retVal |= SM_DESTROY_ERROR;
  if (shmctl(shm_elfReadyQueue_id, IPC_RMID, NULL) == -1)
    retVal |= SM_DESTROY_ERROR;
  if (shmctl(shm_shopClosed_id, IPC_RMID, NULL) == -1)
    retVal |= SM_DESTROY_ERROR;
  if (shmctl(shm_actionId_id, IPC_RMID, NULL) == -1)
    retVal |= SM_DESTROY_ERROR;
  if (shmctl(shm_christmasStarted_id, IPC_RMID, NULL) == -1)
    retVal |= SM_DESTROY_ERROR;

  // Unlink shared memory
  if (shmdt(readyRDCount) == -1)
    retVal |= SM_UNLINK_ERROR;
  if (shmdt(elfReadyQueue) == -1)
    retVal |= SM_UNLINK_ERROR;
  if (shmdt(shopClosed) == -1)
    retVal |= SM_UNLINK_ERROR;
  if (shmdt(actionId) == -1)
    retVal |= SM_UNLINK_ERROR;
  if (shmdt(christmasStarted) == -1)
    retVal |= SM_UNLINK_ERROR;

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
      (wakeForHitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
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
      sem_init(wakeForHitch, 1, 0) == -1)
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
