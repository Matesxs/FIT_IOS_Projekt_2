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
void handle_elf(int id)
{
  // Init random generator
  srand(time(NULL) * getpid());

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Elf %d: started\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  while (true)
  {
    // Work for random amount of time
    unsigned int work_time = (random() % (params.te + 1));
    usleep(work_time * 1000);

    sem_wait(writeOutLock);
    fprintf(outputFile, "%d: Elf %d: need help\n", *actionId, id);
    *actionId += 1;
    sem_post(writeOutLock);

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

    sem_wait(writeOutLock);
    fprintf(outputFile, "%d: Elf %d: get help\n", *actionId, id);
    *actionId += 1;
    sem_post(writeOutLock);

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
  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Elf %d: taking holidays\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);
}

/**
 * @brief Handler for reindeer processes
 *
 * Wait for all raindeers to return and wakeup Santa
 *
 * @param id id of raindeer
 */
void handle_rd(int id)
{
  // Init random generator
  srand(time(NULL) * getpid());

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: RD %d: rstarted\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  // Wait some time before going home
  unsigned int vac_time = (random() % ((params.tr - params.tr / 2) + 1)) + params.tr / 2;
  usleep(vac_time * 1000);

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: RD %d: return home\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  // Wait for hitch
  *readyRDCount += 1;
  if (*readyRDCount == params.nr)
    sem_post(wakeForHitch);

  sem_wait(rdWaitForHitch);

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: RD %d: get hitched\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);
  sem_post(rdHitched);
}

/**
 * @brief Handler for Santa process
 *
 * Sleep, help elves and prepare raindeers
 */
void handle_santa()
{
  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Santa: going to sleep\n", *actionId);
  *actionId += 1;
  sem_post(writeOutLock);

  while (true)
  {
    // Santa will get woken up and will go help elfs
    if (sem_trywait(wakeForHelp) == 0)
    {
      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: helping elves\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);

      // Help 3 elves
      sem_post(getHelp);
      sem_post(getHelp);
      sem_post(getHelp);

      sem_wait(elfHelped);
      sem_wait(elfHelped);
      sem_wait(elfHelped);

      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: going to sleep\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);
    }

    // All RDs ready for hitching
    if (sem_trywait(wakeForHitch) == 0)
    {
      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: closing workshop\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);
      *shopClosed = 1;

      break;
    }
  }

  for (int i = 0; i < params.nr; i++)
  {
    // Hitch all RDs
    sem_post(rdWaitForHitch);
    sem_wait(rdHitched);
  }

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Santa: Christmas started\n", *actionId);
  *actionId += 1;
  sem_post(writeOutLock);
  *christmasStarted = 1;
}
