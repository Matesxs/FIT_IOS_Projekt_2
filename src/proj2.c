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
int main(int argc, char *argv[])
{
  initSignals();
  pid_mainprocess = getpid();

  handleErrors(parseArguments(argc, argv));

  if ((outputFile = fopen("proj2.out", "w")) == NULL)
    handleErrors(OF_OPEN_ERROR);
  setbuf(outputFile, NULL);

  // Allocate shared resources
  handleErrors(allocateResources());

  // Init shared variables
  *readyRDCount = 0;
  *elfReadyQueue = 0;
  *shopClosed = 0;
  *actionId = 1;
  *christmasStarted = 0;

  // Create Santa
  {
    santa_process = fork();

    if (santa_process < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (santa_process == 0)
    {
      handle_santa();
      exit(0);
    }
  }

  // Create elves
  {
    elf_processes = (pid_t *)malloc(sizeof(pid_t) * params.ne);
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
  }

  // Create raindeers
  {
    rd_processes = (pid_t *)malloc(sizeof(pid_t) * params.nr);
    if (rd_processes == NULL)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    rd_count = params.nr;

    for (size_t i = 0; i < rd_count; i++)
    {
      pid_t tmp_proc = fork();

      if (tmp_proc < 0)
      {
        handleErrors(PROCESS_CREATE_ERROR);
      }
      else if (tmp_proc == 0)
      {
        handle_rd(i + 1);
        exit(0);
      }

      rd_processes[i] = tmp_proc;
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
      if (globalElvesReturncode != NO_ERROR)
        handleErrors(globalElvesReturncode);
        
      if (*christmasStarted)
        break;
    }

    // Remove handler for usr signal 1
    signal(SIGUSR1, SIG_IGN);
    if (globalElvesReturncode != NO_ERROR)
      handleErrors(globalElvesReturncode);
  }

  // Wait for all processes to finish
  waitpid(santa_process, NULL, 0);

  // Wait for elves to finish
  for (size_t i = 0; i < elves_count; i++)
    waitpid(elf_processes[i], NULL, 0);

  // Wait for raindeers to finish
  for (int i = 0; i < params.nr; i++)
    waitpid(rd_processes[i], NULL, 0);

  // Clear shared resources
  handleErrors(deallocateResources());

  return NO_ERROR;
}
