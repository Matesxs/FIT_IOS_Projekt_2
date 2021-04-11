/**
 * @file error_handling.c
 * @author Martin Douša
 * @date April 2021
 * @brief Solves error cases of program
 */

#include "error_handling.h"

/**
 * @brief Deallocate all used memory, kill processes and exit
 */
void terminate()
{
  deallocateResources();
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

    case UNEXPECTED_ERROR:
      fprintf(stderr,"Unexpected error\n");
      break;
  }

  terminate();
}
