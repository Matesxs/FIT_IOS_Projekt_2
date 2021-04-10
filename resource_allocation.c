/**
 * @file resource_allocation.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle allocating resources for comunication between processes
 */

#include "resource_allocation.h"

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

  // Destroy semafors
  if (sem_destroy(writeOutLock) == -1 ||
      sem_destroy(rdWaitForHitch) == -1 ||
      sem_destroy(rdHitched) == -1 ||
      sem_destroy(elfWaitForHelp) == -1 ||
      sem_destroy(elfHelped) == -1)
  {
    return SEMAPHOR_DESTROY_ERROR;
  }

  // Deallocate shared memory
  if (shmctl(shm_readyRDCount_id, IPC_RMID, NULL) == -1 ||
      shmdt(readyRDCount) == -1 ||
      shmctl(shm_elfQueue_id, IPC_RMID, NULL) == -1 ||
      shmdt(elfQueue) == -1 ||
      shmctl(shm_elfsHelped_id, IPC_RMID, NULL) == -1 ||
      shmdt(elfsHelped) == -1 ||
      shmctl(shm_shopClosed_id, IPC_RMID, NULL) == -1 ||
      shmdt(shopClosed) == -1 ||
      shmctl(shm_actionId_id, IPC_RMID, NULL) == -1 ||
      shmdt(actionId) == -1)
  {
    return SM_DESTROY_ERROR;
  }

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
      (elfWaitForHelp = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED ||
      (elfHelped = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
  {
    return SEMAPHOR_CREATION_ERROR;
  }

  // Init semaphors
  if (sem_init(writeOutLock, 1, 1) == -1 ||
      sem_init(rdWaitForHitch, 1, 0) == -1 ||
      sem_init(rdHitched, 1, 0) == -1 ||
      sem_init(elfWaitForHelp, 1, 0) == -1 ||
      sem_init(elfHelped, 1, 0) == -1)
  {
    return SEMAPHOR_INIT_FAILED;
  }

  // Create shared memory
  if ((shm_readyRDCount_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_elfQueue_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_elfsHelped_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_shopClosed_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ||
      (shm_actionId_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)
  {
    return SM_CREATE_ERROR;
  }

  // Map shared memory
  if ((readyRDCount = (int*)shmat(shm_readyRDCount_id, NULL, 0)) == NULL ||
      (elfQueue = (int*)shmat(shm_elfQueue_id, NULL, 0)) == NULL ||
      (elfsHelped = (int*)shmat(shm_elfsHelped_id, NULL, 0)) == NULL ||
      (shopClosed = (int*)shmat(shm_shopClosed_id, NULL, 0)) == NULL ||
      (actionId = (int*)shmat(shm_actionId_id, NULL, 0)) == NULL)
  {
    return SM_MAP_ERROR;
  }

  return NO_ERROR;
}
