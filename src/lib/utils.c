/**
 * @file utility.c
 * @author Martin Douša
 * @date April 2021
 * @brief Utility functions
 */

#include "utils.h"

/**
 * @brief Initialize pandlers for signals
 */
void initSignals()
{
  signal(SIGQUIT, terminate);
  signal(SIGINT, terminate);
  signal(SIGTERM, terminate);
  signal(SIGUSR1, SIG_IGN);
}

/**
 * @brief Get values from arguments
 *
 * @param argc length of argument array
 * @param argv array of arguments
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode parseArguments(int argc, char *argv[])
{
  char *rest = NULL;
  size_t startIndex = 1;

  if (argc == 5 || argc == 6)
  {
    if (argc == 6)
    {
      startIndex += 1;
      if (strcmp(argv[1], "-p") != 0) return INVALID_ARGUMENT_ERROR;
      params.pflag = true;
    }

    params.ne = (int)strtol(argv[startIndex], &rest, 10);
    if (*rest != 0 || params.ne <= 0 || params.ne >= 1000) return INVALID_ARGUMENT_ERROR;
    
    params.nr = (int)strtol(argv[startIndex + 1], &rest, 10);
    if (*rest != 0 || params.nr <= 0 || params.nr >= 20) return INVALID_ARGUMENT_ERROR;

    params.te = (int)strtol(argv[startIndex + 2], &rest, 10);
    if (*rest != 0 || params.te < 0 || params.te > 1000) return INVALID_ARGUMENT_ERROR;

    params.tr = (int)strtol(argv[startIndex + 3], &rest, 10);
    if (*rest != 0 || params.tr < 0 || params.tr > 1000) return INVALID_ARGUMENT_ERROR;
  }
  else
  {
    return ARGUMENT_COUNT_ERROR;
  }

  return NO_ERROR;
}
  
/**
 * @brief Print @p message to output stream
 * 
 * @param entityName name of entity calling this function
 * @param id id of entity calling this function
 * @param message message to print
 */
void printToOutput(char *entityName, int id, char *message)
{
  sem_wait(writeOutLock);

  if (id < 0)
  {
    fprintf(outputFile, "%d: %s: %s\n", *actionId, entityName, message);
  }
  else
  {
    fprintf(outputFile, "%d: %s %d: %s\n", *actionId, entityName, id, message);
  }

  *actionId += 1;
  sem_post(writeOutLock);
}