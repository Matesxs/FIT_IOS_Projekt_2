/**
 * @file error_handling.c
 * @author Martin Douša
 * @date April 2021
 * @brief Solves error cases of program
 */

#include "error_handling.h"

bool killMainCalled = false;

/**
 * @brief Deallocate all used memory, kill processes and exit
 */
void terminate()
{
  if (getpid() == pid_mainprocess)
  {
    for (size_t j = 0; j < elves_count; j++)
    {
      if (elf_processes[j] != 0)
        kill(elf_processes[j], SIGQUIT);
    }

    for (size_t j = 0; j < rd_count; j++)
    {
      if (rd_processes[j] != 0)
        kill(rd_processes[j], SIGQUIT);
    }

    if (santa_process != 0)
    {
      kill(santa_process, SIGQUIT);
    }

    deallocateResources();
  }
  else if (!killMainCalled)
  {
    killMainCalled = true;
    kill(pid_mainprocess, SIGQUIT);
  }

  exit(1);
}

/**
 * @brief Print message to console based on provided ReturnCode @p code and terminate program
 *
 * @param code ReturnCode to test
 */
void handleErrors(ReturnCode code)
{
  if (code == NO_ERROR)
    return;

  if (code & ARGUMENT_COUNT_ERROR)
    fprintf(stderr, "Invalid argument count\n");

  if ((code & INVALID_ARGUMENT_ERROR) >> 1)
    fprintf(stderr, "Invalid argument\n");

  if ((code & SEMAPHOR_CREATION_ERROR) >> 2)
    fprintf(stderr, "Failed to create semaphores\n");

  if ((code & SEMAPHOR_INIT_FAILED) >> 3)
    fprintf(stderr, "Failed to initialize semaphores\n");

  if ((code & SEMAPHOR_DESTROY_ERROR) >> 4)
    fprintf(stderr, "Failed to destroy semaphores\n");

  if ((code & SM_CREATE_ERROR) >> 5)
    fprintf(stderr, "Failed to allocate shared memory\n");

  if ((code & SM_MAP_ERROR) >> 6)
    fprintf(stderr, "Failed to map shared memory\n");

  if ((code & SM_DESTROY_ERROR) >> 7)
    fprintf(stderr, "Failed to deallocate shared memory\n");

  if ((code & SM_UNLINK_ERROR) >> 8)
    fprintf(stderr, "Failed to unlink shared memory\n");

  if ((code & PROCESS_CREATE_ERROR) >> 9)
    fprintf(stderr, "Failed to create new process\n");

  if ((code & OF_OPEN_ERROR) >> 10)
    fprintf(stderr, "Failed to open output file\n");

  if ((code & PID_ALLOCATION_ERROR) >> 11)
    fprintf(stderr, "Failed to allocate memory for pid arrays\n");

  if ((code & UNEXPECTED_ERROR) >> 12)
    fprintf(stderr, "Unexpected error\n");

  terminate();
}
