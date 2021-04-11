/**
 * @file process_handlers.c
 * @author Martin Douša
 * @date April 2021
 * @brief Handle actions for heach process
 */

#include "process_handlers.h"

/**
 * @brief Handler for elf processes
 *
 * Solves elves work and comunicate with Santa
 *
 * @param id id of elf
 * @param params loaded params from argument
 */
void handle_elf(int id, Params params)
{
  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Elf %d: started\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  unsigned int work_time = random() % (params.te + 1);
  usleep(work_time * 1000);

  if (!(*shopClosed))
  {
    sem_wait(writeOutLock);
    fprintf(outputFile, "%d: Elf %d: need help\n", *actionId, id);
    *actionId += 1;
    sem_post(writeOutLock);

    sem_wait(waitForHelp);
    *elfReadyQueue += 1;
    sem_wait(getHelp);
    *elfReadyQueue -= 1;

    if (!(*shopClosed))
    {
      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Elf %d: get help\n", *actionId, id);
      *actionId += 1;
      sem_post(writeOutLock);

      sem_post(elfHelped);

      // Signal to 3 next elves that Santa is ready
      if (*elfReadyQueue == 0)
      {
        sem_post(waitForHelp);
        sem_post(waitForHelp);
        sem_post(waitForHelp);
      }
    }
  }

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
 * @param params loaded params from argument
 */
void handle_rd(int id, Params params)
{
  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: RD %d: started\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  unsigned int vac_time = (random() % (params.tr - params.tr / 2 + 1)) + params.tr / 2;
  usleep(vac_time * 1000);

  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: RD %d: return home\n", *actionId, id);
  *actionId += 1;
  sem_post(writeOutLock);

  *readyRDCount += 1;
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
 *
 * @param params loaded params from argument
 */
void handle_santa(Params params)
{
  sem_wait(writeOutLock);
  fprintf(outputFile, "%d: Santa: going to sleep\n", *actionId);
  *actionId += 1;
  sem_post(writeOutLock);

  while (true)
  {
    // Santa will get woken up and will go help elfs
    if (*elfReadyQueue >= 3)
    {
      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: helping elves\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);

      sem_post(getHelp);
      sem_post(getHelp);
      sem_post(getHelp);

      sem_wait(elfHelped);
      sem_wait(elfHelped);
      sem_wait(elfHelped);

      *elfsHelped += 3;

      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: going to sleep\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);
    }

    // All RDs ready for hitching
    if (*readyRDCount == params.nr)
    {
      *shopClosed = 1;
      sem_wait(writeOutLock);
      fprintf(outputFile, "%d: Santa: closing workshop\n", *actionId);
      *actionId += 1;
      sem_post(writeOutLock);

      // Release rest of elfs for holidays
      // TODO: Rework to signal
      int restOfQueue = params.ne - *elfsHelped;
      for (int i = 0; i < restOfQueue; i++)
      {
        sem_post(waitForHelp);
        sem_post(getHelp);
      }

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
}
