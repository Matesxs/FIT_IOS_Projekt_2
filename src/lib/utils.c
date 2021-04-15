/**
 * @file utility.c
 * @author Martin Douša
 * @date April 2021
 * @brief Utility functions
 */

#include "utils.h"

/**
 * @brief Get values from arguments
 *
 * @param argc length of argument array
 * @param argv array of arguments
 * @param params pointer to Params class that will contain extracted values
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode parseArguments(int argc, char *argv[])
{
  if (argc < 5 || argc > 6) return ARGUMENT_COUNT_ERROR;
  char *rest = NULL;

  switch (argc)
  {
  case 5:
    params.pflag = false;
    params.ne = (int)strtol(argv[1], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.nr = (int)strtol(argv[2], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.te = (int)strtol(argv[3], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.tr = (int)strtol(argv[4], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;
    break;

  case 6:
    if (strcmp(argv[1], "-p") != 0) return INVALID_ARGUMENT_ERROR;

    params.pflag = true;
    params.ne = (int)strtol(argv[2], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.nr = (int)strtol(argv[3], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.te = (int)strtol(argv[4], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;

    params.tr = (int)strtol(argv[5], &rest, 10);
    if (*rest != 0) return INVALID_ARGUMENT_ERROR;
    break;
  
  default:
    return INVALID_ARGUMENT_ERROR;
  }

  if (params.ne <= 0 || params.ne >= 1000 ||
      params.nr <= 0 || params.nr >= 20 ||
      params.te < 0 || params.te > 1000 ||
      params.tr < 0 || params.tr > 1000)
  {
    return ARGUMENT_OUT_OF_RANGE;
  }

  return NO_ERROR;
}

/**
 * @brief Forward USR1 signal to elves creator process
 */
void handleUsrSignal()
{
  if (processHandlers[1] != 0)
    kill(processHandlers[1], SIGUSR1);
}