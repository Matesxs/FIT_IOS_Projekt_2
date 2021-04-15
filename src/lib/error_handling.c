/**
 * @file error_handling.c
 * @author Martin Douša
 * @date April 2021
 * @brief Solves error cases of program
 */

#include "error_handling.h"

bool killSend = false;

/**
 * @brief Deallocate all used memory, kill processes and exit
 */
void terminate()
{
  deallocateResources();
  kill(pid_mainprocess, SIGQUIT);
  kill(getpid(), SIGQUIT);
  exit(1);
}

/**
 * Print message to console based on provided ReturnCode @p code and terminate program
 *
 * @param code ReturnCode to test
 */
void handleErrors(ReturnCode code)
{
  switch (code) {
    case NO_ERROR:
      return;

    case ARGUMENT_COUNT_ERROR:
      fprintf(stderr,"Invalid number of arguments\n");
      break;

    case INVALID_ARGUMENT_ERROR:
      fprintf(stderr,"Invalid characters in argument\n");
      break;

    case ARGUMENT_OUT_OF_RANGE:
      fprintf(stderr,"Argument value out of defined range\n");
      break;

    case SEMAPHOR_CREATION_ERROR:
      fprintf(stderr,"Failed to create semaphores\n");
      break;

    case SEMAPHOR_INIT_FAILED:
      fprintf(stderr,"Failed to initialize semaphors\n");
      break;

    case SEMAPHOR_DESTROY_ERROR:
      fprintf(stderr,"Failed to destroy semaphores\n");
      break;

    case SM_CREATE_ERROR:
      fprintf(stderr,"Failed to create shared memory\n");
      break;

    case SM_MAP_ERROR:
      fprintf(stderr,"Failed to map shared memory\n");
      break;

    case SM_DESTROY_ERROR:
      fprintf(stderr,"Failed to destroy shared memory\n");
      break;

    case PROCESS_CREATE_ERROR:
      fprintf(stderr,"Failed to create new process\n");
      break;

    case OF_OPEN_ERROR:
      fprintf(stderr,"Failed to open output file\n");
      break;

    case PID_ALLOCATION_ERROR:
      fprintf(stderr,"Failed to allocate memory for pid arrays\n");
      break;

    case UNEXPECTED_ERROR:
    default:
      fprintf(stderr,"Unexpected error\n");
      break;
  }

  terminate();
}
