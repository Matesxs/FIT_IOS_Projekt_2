/**
 * @file process_handlers.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle actions for heach process
 */

#include "process_handlers.h"

/**
 * @brief Add random number of elves
 * 
 * Generate random number of new elves based on NE argument to help with work when called
 */
void addElves()
{
  size_t newElvesCount = processHolder.elvesCount + (random() % params.ne) + 1;

  pid_t *tmp = (pid_t*)realloc(processHolder.elfIds, newElvesCount * sizeof(pid_t));
  if (tmp == NULL)
  {
    globalElvesReturncode = PID_ALLOCATION_ERROR;
    return;
  }

  processHolder.elfIds = tmp;

  for (size_t i = processHolder.elvesCount; i < newElvesCount; i++)
  {
    pid_t tmp_proc = fork();

    if (tmp_proc < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (tmp_proc == 0)
    {
      handle_elf(i + 1);
      exit(0);
    }
    else
    {
      processHolder.elfIds[i] = tmp_proc;
    }
  }

  processHolder.elvesCount = newElvesCount;
}

/**
 * @brief Handler for elf processes
 *
 * Solves elves work and comunicate with Santa
 *
 * @param id id of elf
 */
void handle_elf(size_t id)
{
  // Init random generator
  srand(time(NULL) * getpid());

  printToOutput("Elf", id, "started");

  while (true)
  {
    // Work for random amount of time
    unsigned int work_time = (random() % (params.te + 1));
    usleep(work_time * 1000);

    printToOutput("Elf", id, "need help");

    // If shop is closed go elf dont need help and can take holidays
    if (sharedMemory->shopClosed) break;

    // Wait in queue for empty workshop
    while(sem_trywait(&semHolder->waitForHelp) != 0)
    {
      if (sharedMemory->shopClosed)
        break;
    }

    if (sharedMemory->shopClosed) break;

    // Wait for help
    sharedMemory->elfReadyQueue++;
    if (sharedMemory->elfReadyQueue == 3)
    {
      sem_post(&semHolder->wakeForHelp);
    }

    while(sem_trywait(&semHolder->getHelp) != 0)
    {
      if (sharedMemory->shopClosed)
        break;
    }

    sharedMemory->elfReadyQueue--;
    if (sharedMemory->shopClosed) break;

    printToOutput("Elf", id, "get help");

    // Get help from Santa
    sem_post(&semHolder->elfHelped);

    // Signal to 3 next elves that workshop is free
    if (sharedMemory->elfReadyQueue == 0)
    {
      sem_post(&semHolder->waitForHelp);
      sem_post(&semHolder->waitForHelp);
      sem_post(&semHolder->waitForHelp);
    }
  }

  // take holidays
  printToOutput("Elf", id, "taking holidays");
  sem_post(&semHolder->elfFinished);
}

/**
 * @brief Handler for reindeer processes
 *
 * Wait for all raindeers to return and wakeup Santa
 *
 * @param id id of raindeer
 */
void handle_rd(size_t id)
{
  // Init random generator
  srand(time(NULL) * getpid());

  printToOutput("RD", id, "rstarted");

  // Wait some time before going home
  unsigned int vac_time = (random() % ((params.tr - params.tr / 2) + 1)) + params.tr / 2;
  usleep(vac_time * 1000);

  // Wait for hitch
  sharedMemory->readyRDCount++;
  if (sharedMemory->readyRDCount == params.nr)
  {
    sem_wait(&semHolder->santaReady);
    sem_post(&semHolder->wakeForHitch);
    sem_post(&semHolder->santaReady);
  }

  printToOutput("RD", id, "return home");

  sem_wait(&semHolder->rdWaitForHitch);

  printToOutput("RD", id, "get hitched");
  sem_post(&semHolder->rdHitched);
  sem_post(&semHolder->rdFinished);
}

/**
 * @brief Handler for Santa process
 *
 * Sleep, help elves and prepare raindeers
 */
void handle_santa()
{
  printToOutput("Santa", NO_ID, "going to sleep");

  while (true)
  {
    sem_wait(&semHolder->santaReady);

    // All RDs ready for hitching, close workshop
    if (sem_trywait(&semHolder->wakeForHitch) == 0)
    {
      sem_post(&semHolder->santaReady);
      break;
    }

    // Santa will get woken up and will go help elfs
    if (sem_trywait(&semHolder->wakeForHelp) == 0)
    {
      printToOutput("Santa", NO_ID, "helping elves");

      // Help 3 elves
      for (int i = 0; i < 3; i++)
      {
        sem_post(&semHolder->getHelp);
        sem_wait(&semHolder->elfHelped);
      }

      printToOutput("Santa", NO_ID, "going to sleep");
    }

    sem_post(&semHolder->santaReady);
  }

  printToOutput("Santa", NO_ID, "closing workshop");
  sharedMemory->shopClosed = true;

  for (int i = 0; i < params.nr; i++)
  {
    // Hitch all RDs
    sem_post(&semHolder->rdWaitForHitch);
    sem_wait(&semHolder->rdHitched);
  }

  printToOutput("Santa", NO_ID, "Christmas started");
  sharedMemory->christmasStarted = true;
  sem_post(&semHolder->santaFinished);
}
