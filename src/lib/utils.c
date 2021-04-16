#include "utils.h"

/**
 * @brief Get values from arguments
 *
 * @param argc length of argument array
 * @param argv array of arguments
 * @return ReturnCode with NO_ERROR if it was successful or error code
 */
ReturnCode parseArguments(int argc, char *argv[])
{
  if (argc != 5) return ARGUMENT_COUNT_ERROR;

  char *rest = NULL;
  params.ne = (int)strtol(argv[1], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params.nr = (int)strtol(argv[2], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params.te = (int)strtol(argv[3], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  params.tr = (int)strtol(argv[4], &rest, 10);
  if (*rest != 0) return INVALID_ARGUMENT_ERROR;

  if (params.ne <= 0 || params.ne >= 1000 ||
      params.nr <= 0 || params.nr >= 20 ||
      params.te < 0 || params.te > 1000 ||
      params.tr < 0 || params.tr > 1000)
  {
    return ARGUMENT_OUT_OF_RANGE;
  }

  return NO_ERROR;
}

void initSignals()
{
  signal(SIGQUIT, terminate);
  signal(SIGINT, terminate);
  signal(SIGTERM, terminate);
}