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
  signal(SIGQUIT, SIG_IGN);
  signal(SIGUSR1, SIG_IGN);

  // Generate new size of elf process ids array
  size_t oldElvesCount = processHolder.elvesCount;
  size_t newElvesCount = oldElvesCount + (random() % params.ne) + 1;

  // printf("Adding %ld new elves\n", newElvesCount - oldElvesCount);

  // Reallocate elf process ids array
  pid_t *tmp = (pid_t*)realloc(processHolder.elfIds, newElvesCount * sizeof(pid_t));
  if (tmp == NULL)
    handleErrors(PID_ALLOCATION_ERROR);

  // Replace pointer
  processHolder.elfIds = tmp;
  processHolder.elvesCount = newElvesCount;

  signal(SIGQUIT, terminate);
  
  sharedMemory->numberOfElves = processHolder.elvesCount;

  // Generate new elves
  for (size_t i = oldElvesCount; i < newElvesCount; i++)
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
      processHolder.elfIds[i] = tmp_proc;
  }

  signal(SIGUSR1, addElves);
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
    sem_wait(&semHolder->waitInQueue);

    if (sharedMemory->shopClosed) break;

    // Wake Santa if third in queue
    sem_wait(&semHolder->elfQueueMutex);
    sharedMemory->elfReadyQueue++;

    if (sharedMemory->elfReadyQueue >= 3)
    {
      sem_post(&semHolder->wakeForHelp);
    }
    sem_post(&semHolder->elfQueueMutex);

    // Wait for help
    sem_wait(&semHolder->waitForHelp);

    sem_wait(&semHolder->elfQueueMutex);
    sharedMemory->elfReadyQueue--;
    sem_post(&semHolder->elfQueueMutex);

    if (sharedMemory->shopClosed) break;

    printToOutput("Elf", id, "get help");

    // Get help from Santa
    sem_post(&semHolder->elfHelped);

    // Signal to 3 next elves that workshop is free
    if (sharedMemory->elfReadyQueue == 0)
    {
      sem_post(&semHolder->waitInQueue);
      sem_post(&semHolder->waitInQueue);
      sem_post(&semHolder->waitInQueue);
    }
  }

  // take holidays
  printToOutput("Elf", id, "taking holidays");
  sem_post(&semHolder->childFinished);

  // printf("Elf %ld finished\n", id);
}

/**
 * @brief Handler for reindeer processes
 *
 * Wait for all reindeers to return and wakeup Santa
 *
 * @param id id of reindeer
 */
void handle_rd(size_t id)
{
  // Init random generator
  srand(time(NULL) * getpid());

  printToOutput("RD", id, "rstarted");

  // Wait some time before going home
  unsigned int vac_time = (random() % ((params.tr - params.tr / 2) + 1)) + params.tr / 2;
  usleep(vac_time * 1000);

  sem_wait(&semHolder->rdReadyCountMutex);
  sharedMemory->readyRDCount++;

  if (sharedMemory->readyRDCount == params.nr)
  {
    // Wake santa if last
    sem_wait(&semHolder->santaReady);
    printToOutput("RD", id, "return home");
    kill(processHolder.santaId, SIGUSR2);
    sem_post(&semHolder->santaReady);
  }
  else
    printToOutput("RD", id, "return home");
  
  sem_post(&semHolder->rdReadyCountMutex);

  // Wait for hitch
  sem_wait(&semHolder->rdWaitForHitch);

  printToOutput("RD", id, "get hitched");

  // Signalize was hitched
  sem_post(&semHolder->rdHitched);
  sem_post(&semHolder->childFinished);

  // printf("RD %ld finished\n", id);
}

/**
 * @brief Handle leaving of Santa
 */
void handle_santa_end()
{
  printToOutput("Santa", NO_ID, "closing workshop");
  sharedMemory->shopClosed = true;

  for (int i = 0; i < params.nr; i++)
  {
    // Hitch all RDs
    sem_post(&semHolder->rdWaitForHitch);
    sem_wait(&semHolder->rdHitched);
  }

  printToOutput("Santa", NO_ID, "Christmas started");
  sem_post(&semHolder->christmasStarted);

  // Send home elves
  sem_wait(&semHolder->numOfElvesStable);
  for (size_t i = 0; i < sharedMemory->numberOfElves; i++)
  {
    sem_post(&semHolder->waitInQueue);
    sem_post(&semHolder->waitForHelp);
  }
  sem_post(&semHolder->numOfElvesStable);

  sem_post(&semHolder->childFinished);

  // printf("Santa finished\n");

  exit(0);
}

/**
 * @brief Help elves
 * 
 * @param number number of elves to help
 */
void help_elves(size_t number)
{
  for(size_t i = 0; i < number; i++)
    sem_post(&semHolder->waitForHelp);

  for(size_t i = 0; i < number; i++)
    sem_wait(&semHolder->elfHelped);
}

/**
 * @brief Handler for Santa process
 *
 * Sleep, help elves and prepare reindeers
 */
void handle_santa()
{
  signal(SIGUSR2, handle_santa_end);

  printToOutput("Santa", NO_ID, "going to sleep");
  sem_post(&semHolder->santaReady);

  while (true)
  {
    // Santa will get woken up and will go help elfs
    sem_wait(&semHolder->wakeForHelp);

    sem_wait(&semHolder->santaReady);
    printToOutput("Santa", NO_ID, "helping elves");

    help_elves(3);

    printToOutput("Santa", NO_ID, "going to sleep");

    sem_post(&semHolder->santaReady);
  }
}
