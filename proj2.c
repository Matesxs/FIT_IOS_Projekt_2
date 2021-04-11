/**
 * @file proj2.c
 * @author Martin Douša
 * @date April 2021
 * @brief Entry point for Santa Claus problem solver
 */

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>

#include "shared_resources.h"
#include "static_constructions.h"
#include "resource_allocation.h"
#include "error_handling.h"

/**
 * @brief Get values from arguments
 *
 * @param argc length of argument array
 * @param argv array of arguments
 * @param params pointer to Params class that will contain extracted values
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode parseArguments(int argc, char *argv[], Params *params)
{
  if (argc != 5) return ARGUMENT_COUNT_ERROR;

  char *rest = NULL;
  params->ne = strtol(argv[1], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params->nr = strtol(argv[2], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params->te = strtol(argv[3], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params->tr = strtol(argv[4], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  if (params->ne < 0 || params->ne > 1000 ||
      params->nr < 0 || params->nr > 20 ||
      params->te < 0 || params->te > 1000 ||
      params->tr < 0 || params->tr > 1000)
  {
    return ARGUMENT_OUT_OF_RANGE;
  }

  return NO_ERROR;
}

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

/**
 * @brief Entrypoint of program
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return returncode of program
 */
int main (int argc, char *argv[])
{
  Params params;
  handleErrors(parseArguments(argc, argv, &params));

  signal(SIGTERM, terminate);
  signal(SIGINT, terminate);

  if ((outputFile = fopen("proj2.out", "w")) == NULL)
    handleErrors(OF_OPEN_ERROR);
  setbuf(outputFile, NULL);

  pid_t elf_processes[params.ne];
  pid_t rd_processes[params.nr];
  pid_t santa_process;

  // Split main process
  pid_mainprocess = getpid();
  srand(time(NULL) * pid_mainprocess);

  handleErrors(allocateResources());
  *readyRDCount = 0;
  *elfReadyQueue = 0;
  *elfsHelped = 0;
  *shopClosed = 0;
  *actionId = 1;

  {
    pid_t tmp_proc = fork();

    if (tmp_proc < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (tmp_proc == 0)
    {
      handle_santa(params);
      exit(0);
    }
    else
    {
      santa_process = tmp_proc;
    }
  }

  for (int i = 0; i < params.ne; i++)
  {
    pid_t tmp_proc = fork();

    if (tmp_proc < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (tmp_proc == 0)
    {
      handle_elf(i + 1, params);
      exit(0);
    }
    else
    {
      elf_processes[i] = tmp_proc;
    }
  }

  for (int i = 0; i < params.nr; i++)
  {
    pid_t tmp_proc = fork();

    if (tmp_proc < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (tmp_proc == 0)
    {
      handle_rd(i + 1, params);
      exit(0);
    }
    else
    {
      rd_processes[i] = tmp_proc;
    }
  }

  // Wait for all processes to finish
  for (int i = 0; i < params.ne; i++)
  {
    waitpid(elf_processes[i], NULL, 0);
  }

  for (int i = 0; i < params.nr; i++)
  {
    waitpid(rd_processes[i], NULL, 0);
  }

  waitpid(santa_process, NULL, 0);

  handleErrors(deallocateResources());

  return NO_ERROR;
}
