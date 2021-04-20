/**
 * @file proj2.c
 * @author Martin Douša
 * @date April 2021
 * @brief Entry point for Santa Claus problem solver
 */

#include <signal.h>
#include <stdio.h>

#include "lib/static_constructions.h"
#include "lib/resource_allocation.h"
#include "lib/shared_resources.h"
#include "lib/error_handling.h"
#include "lib/process_handlers.h"

/**
 * @brief Entrypoint of program
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return returncode of program
 */
int main(int argc, char *argv[])
{
  // Init signal handlers and get process id of main process
  initSignals();
  processHolder.mainId = getpid();

  // Load arguments
  handleErrors(parseArguments(argc, argv));

  // Open output stream
  if ((outputFile = fopen("proj2.out", "w")) == NULL)
    handleErrors(OF_OPEN_ERROR);
  setbuf(outputFile, NULL);

  // Allocate shared resources
  handleErrors(allocateResources());

  // Create Santa
  {
    processHolder.santaId = fork();

    if (processHolder.santaId < 0)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    else if (processHolder.santaId == 0)
    {
      handle_santa();
      exit(0);
    }
  }

  // Create elves
  {
    processHolder.elfIds = (pid_t *)malloc(sizeof(pid_t) * params.ne);
    if (processHolder.elfIds == NULL)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    processHolder.elvesCount = params.ne;

    for (size_t i = 0; i < processHolder.elvesCount; i++)
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
  }

  // Create raindeers
  {
    processHolder.rdIds = (pid_t *)malloc(sizeof(pid_t) * params.nr);
    if (processHolder.rdIds == NULL)
    {
      handleErrors(PROCESS_CREATE_ERROR);
    }
    processHolder.rdCount = params.nr;

    for (size_t i = 0; i < processHolder.rdCount; i++)
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

      processHolder.rdIds[i] = tmp_proc;
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
        
      if (sharedMemory->christmasStarted)
        break;
    }

    // Remove handler for usr signal 1
    signal(SIGUSR1, SIG_IGN);
    if (globalElvesReturncode != NO_ERROR)
      handleErrors(globalElvesReturncode);
  }

  // Wait for all processes to finish
  // Wait for Santa to finish
  sem_wait(&semHolder->santaFinished);

  // Wait for elves to finish
  for (size_t i = 0; i < processHolder.elvesCount; i++)
    sem_wait(&semHolder->elfFinished);

  // Wait for raindeers to finish
  for (size_t i = 0; i < processHolder.rdCount; i++)
    sem_wait(&semHolder->rdFinished);

  // Clear shared resources
  handleErrors(deallocateResources());

  return NO_ERROR;
}
