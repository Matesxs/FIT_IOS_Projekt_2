/**
 * @file proj2.c
 * @author Martin Douša
 * @date April 2021
 * @brief Entry point for Santa Claus problem solver
 */

#include <stdio.h>
#include <time.h>
#include <sys/wait.h>

#include "shared_resources.h"
#include "static_constructions.h"
#include "resource_allocation.h"
#include "error_handling.h"
#include "process_handlers.h"

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

  // Create holder for process creators
  pid_t processCreatorProcess;
  pid_t processHandlers[3];

  // Split main process
  pid_mainprocess = getpid();
  srand(time(NULL) * pid_mainprocess);

  // Allocate shared resources
  handleErrors(allocateResources());

  // Init shared variables
  *readyRDCount = 0;
  *elfReadyQueue = 0;
  *elfsHelped = 0;
  *shopClosed = 0;
  *actionId = 1;

  // Create Santa process
  processCreatorProcess = fork();
  if (processCreatorProcess < 0)
  {
    handleErrors(PROCESS_CREATE_ERROR);
  }
  else if (processCreatorProcess == 0)
  {
    pid_t santa_process = fork();

    if (santa_process < 0)
    {
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
    handleErrors(PROCESS_CREATE_ERROR);
  }
  else if (processCreatorProcess == 0)
  {
    pid_t elf_processes[params.ne];

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

    for (int i = 0; i < params.ne; i++)
      waitpid(elf_processes[i], NULL, 0);
    exit(0);
  }

  processHandlers[1] = processCreatorProcess;

  // Create raindeer processes
  processCreatorProcess = fork();
  if (processCreatorProcess < 0)
  {
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

  // Wait for all processes to finish
  for (int i = 0; i < 3; i++)
  {
    waitpid(processHandlers[i], NULL, 0);
  }

  // Clear shared resources
  handleErrors(deallocateResources());

  return NO_ERROR;
}
