/**
 * @file static_constructions.c
 * @author Martin Douša
 * @date April 2021
 * @brief Holds definitions for basic program structures
 */

#ifndef IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
#define IOS_PROJECT2_STATIC_CONSTRUCTIONS_H

#include <stdbool.h>

/**
 * @brief Holds all available return codes
 */
typedef enum returnCode
{
  NO_ERROR,
  ARGUMENT_COUNT_ERROR = 1,
  INVALID_ARGUMENT_ERROR = 2,
  SEMAPHOR_CREATION_ERROR = 4,
  SEMAPHOR_INIT_FAILED = 8,
  SEMAPHOR_DESTROY_ERROR = 16,
  SM_CREATE_ERROR = 32,
  SM_MAP_ERROR = 64,
  SM_DESTROY_ERROR = 128,
  SM_UNLINK_ERROR = 256,
  PROCESS_CREATE_ERROR = 512,
  OF_OPEN_ERROR = 1024,
  PID_ALLOCATION_ERROR = 2048,
  UNEXPECTED_ERROR = 4096,
} ReturnCode;

/**
 * @brief Holds all parameters extracted from arguments
 */
typedef struct
{
  int ne, nr, te, tr;
  bool pflag;
} Params;

#endif //IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
