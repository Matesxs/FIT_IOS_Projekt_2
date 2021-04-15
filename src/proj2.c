/**
 * @file proj2.c
 * @author Martin Douša
 * @date April 2021
 * @brief Entry point for Santa Claus problem solver
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#include "lib/shared_resources.h"
#include "lib/static_constructions.h"
#include "lib/resource_allocation.h"
#include "lib/error_handling.h"
#include "lib/process_handlers.h"

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
  if (argc < 5 || argc > 6) return ARGUMENT_COUNT_ERROR;
  char *rest = NULL;

  switch (argc)
  {
  case 5:
    params->pflag = false;
    params->ne = (int)strtol(argv[1], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->nr = (int)strtol(argv[2], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->te = (int)strtol(argv[3], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->tr = (int)strtol(argv[4], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;
    break;

  case 6:
    if (strcmp(argv[1], "-p") != 0) return INVALID_ARGUMENT_ERROR;

    params->pflag = true;
    params->ne = (int)strtol(argv[2], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->nr = (int)strtol(argv[3], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->te = (int)strtol(argv[4], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params->tr = (int)strtol(argv[5], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;
    break;
  
  default:
    return INVALID_ARGUMENT_ERROR;
  }

  if (params->ne <= 0 || params->ne >= 1000 ||
      params->nr <= 0 || params->nr >= 20 ||
      params->te < 0 || params->te > 1000 ||
      params->tr < 0 || params->tr > 1000)
  {
    return ARGUMENT_OUT_OF_RANGE;
  }

  return NO_ERROR;
}

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
      handle_elf(i + 1, params);
      exit(0);
    }
    else
    {
      elf_processes[i] = tmp_proc;
    }
  }

  elves_count = newElvesCount;
}

void handleUsrSignal()
{
  if (processHandlers[1] != 0)
    kill(processHandlers[1], SIGUSR1);
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
  pid_mainprocess = getpid();

  handleErrors(parseArguments(argc, argv, &params));

  signal(SIGUSR1, handleUsrSignal);
  signal(SIGQUIT, terminate);
  signal(SIGINT, terminate);
  signal(SIGTERM, terminate);

  if ((outputFile = fopen("proj2.out", "w")) == NULL)
    handleErrors(OF_OPEN_ERROR);
  setbuf(outputFile, NULL);

  // Create holder for process creators
  pid_t processCreatorProcess;

  // Init random generator
  srand(time(NULL) * getpid());

  // Allocate shared resources
  handleErrors(allocateResources());

  // Init shared variables
  *readyRDCount = 0;
  *elfReadyQueue = 0;
  *shopClosed = 0;
  *actionId = 1;
  *christmasStarted = 0;

  // Create Santa process
  processCreatorProcess = fork();
  if (processCreatorProcess < 0)
  {
    // printf("[DEBUG] Santa creator fork");
    handleErrors(PROCESS_CREATE_ERROR);
  }
  else if (processCreatorProcess == 0)
  {
    pid_t santa_process = fork();

    if (santa_process < 0)
    {
      // printf("[DEBUG] Santa fork");
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (santa_process == 0)
    {
      handle_santa(params);
      exit(0);
    }

    waitpid(santa_process, NULL, 0);
    exit(0);
  }

  processHandlers[0] = processCreatorProcess;

  // Create elves processes
  processCreatorProcess = fork();
  if (processCreatorProcess < 0)
  {
    // printf("[DEBUG] Elves creator fork");
    handleErrors(PROCESS_CREATE_ERROR);
  }
  else if (processCreatorProcess == 0)
  {
    signal(SIGUSR1, SIG_IGN);

    elf_processes = (pid_t*)malloc(sizeof(pid_t) * params.ne);
    if (elf_processes == NULL)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    elves_count = params.ne;

    for (size_t i = 0; i < elves_count; i++)
    {
      pid_t tmp_proc = fork();

      if (tmp_proc < 0)
      {
        // printf("[DEBUG] Elves fork");
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

    // If there is pflag
    if (params.pflag)
    {
      // Add handler for usr signal 1
      signal(SIGUSR1, addElves);

      // Wait for signals before waiting for elves
      while (true)
      {
        if (globalElvesReturncode != NO_ERROR) handleErrors(globalElvesReturncode);
        if (*christmasStarted) break;
      }

      // Remove handler for usr signal 1
      signal(SIGUSR1, SIG_IGN);
      if (globalElvesReturncode != NO_ERROR) handleErrors(globalElvesReturncode);
    }

    for (size_t i = 0; i < elves_count; i++)
      waitpid(elf_processes[i], NULL, 0);

    if (elf_processes != NULL)
    {
      free(elf_processes);
      elf_processes = NULL;
      elves_count = 0;
    }
    exit(0);
  }

  processHandlers[1] = processCreatorProcess;

  // Create raindeer processes
  processCreatorProcess = fork();
  if (processCreatorProcess < 0)
  {
    // printf("[DEBUG] Raindeer creator fork");
    handleErrors(PROCESS_CREATE_ERROR);
  }
  else if (processCreatorProcess == 0)
  {
    pid_t rd_processes[params.nr];

    for (int i = 0; i < params.nr; i++)
    {
      pid_t tmp_proc = fork();

      if (tmp_proc < 0)
      {
        // printf("[DEBUG] Raindeer fork");
        handleErrors(PROCESS_CREATE_ERROR);
      }
      else if (tmp_proc == 0)
      {
        handle_rd(i + 1, params);
        exit(0);
      }

      rd_processes[i] = tmp_proc;
    }

    for (int i = 0; i < params.nr; i++)
      waitpid(rd_processes[i], NULL, 0);
    exit(0);
  }

  processHandlers[2] = processCreatorProcess;

  usleep(1000);
  kill(pid_mainprocess, SIGUSR1);
  usleep(3000);
  kill(pid_mainprocess, SIGUSR1);

  // Wait for all processes to finish
  for (int i = 0; i < 3; i++)
  {
    waitpid(processHandlers[i], NULL, 0);
  }

  // Clear shared resources
  handleErrors(deallocateResources());

  return NO_ERROR;
}
