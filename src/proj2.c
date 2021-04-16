/**
 * @file proj2.c
 * @author Martin Douša
 * @date April 2021
 * @brief Entry point for Santa Claus problem solver
 */

#define _GNU_SOURCE

#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>

#include "lib/static_constructions.h"
#include "lib/resource_allocation.h"
#include "lib/error_handling.h"
#include "lib/process_handlers.h"
#include "lib/utils.h"

/**
 * @brief Entrypoint of program
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return returncode of program
 */
int main (int argc, char *argv[])
{
  initSignals();
  pid_mainprocess = getpid();

  handleErrors(parseArguments(argc, argv));

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
      handle_santa();
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
        handle_elf(i + 1);
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
        handle_rd(i + 1);
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
