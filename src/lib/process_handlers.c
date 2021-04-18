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
  size_t newElvesCount = elves_count + (random() % params.ne) + 1;

  pid_t *tmp = (pid_t*)realloc(elf_processes, newElvesCount * sizeof(pid_t));
  if (tmp == NULL)
  {
    globalElvesReturncode = PID_ALLOCATION_ERROR;
    return;
  }

  elf_processes = tmp;

  for (size_t i = elves_count; i < newElvesCount; i++)
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
      elf_processes[i] = tmp_proc;
    }
  }

  elves_count = newElvesCount;
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
    if (*shopClosed) break;

    // Wait in queue for empty workshop
    bool leave = false;

    while(sem_trywait(waitForHelp) != 0)
    {
      if (*shopClosed)
      {
        leave = true;
        break;
      }
    }

    if (leave) break;

    // Wait for help
    *elfReadyQueue += 1;
    if (*elfReadyQueue == 3)
    {
      sem_post(wakeForHelp);
    }

    while(sem_trywait(getHelp) != 0)
    {
      if (*shopClosed)
      {
        leave = true;
        break;
      }
    }

    *elfReadyQueue -= 1;
    if (leave) break;

    printToOutput("Elf", id, "get help");

    // Get help from Santa
    sem_post(elfHelped);

    // Signal to 3 next elves that workshop is free
    if (*elfReadyQueue == 0)
    {
      sem_post(waitForHelp);
      sem_post(waitForHelp);
      sem_post(waitForHelp);
    }
  }

  // take holidays
  printToOutput("Elf", id, "taking holidays");
  sem_post(elfFinished);
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
  *readyRDCount += 1;
  if (*readyRDCount == params.nr)
  {
    sem_wait(santaReady);
    sem_post(wakeForHitch);
    sem_post(santaReady);
  }

  printToOutput("RD", id, "return home");

  sem_wait(rdWaitForHitch);

  printToOutput("RD", id, "get hitched");
  sem_post(rdHitched);
  sem_post(rdFinished);
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
    sem_wait(santaReady);

    // All RDs ready for hitching, close workshop
    if (sem_trywait(wakeForHitch) == 0)
    {
      sem_post(santaReady);
      break;
    }

    // Santa will get woken up and will go help elfs
    if (sem_trywait(wakeForHelp) == 0)
    {
      printToOutput("Santa", NO_ID, "helping elves");

      // Help 3 elves
      for (int i = 0; i < 3; i++)
      {
        sem_post(getHelp);
        sem_wait(elfHelped);
      }

      printToOutput("Santa", NO_ID, "going to sleep");
    }

    sem_post(santaReady);
  }

  printToOutput("Santa", NO_ID, "closing workshop");
  *shopClosed = 1;

  for (int i = 0; i < params.nr; i++)
  {
    // Hitch all RDs
    sem_post(rdWaitForHitch);
    sem_wait(rdHitched);
  }

  printToOutput("Santa", NO_ID, "Christmas started");
  *christmasStarted = 1;
  sem_post(santaFinished);
}
