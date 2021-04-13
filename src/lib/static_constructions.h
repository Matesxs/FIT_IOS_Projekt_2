/**
 * @file static_constructions.c
 * @author Martin Douša
 * @date April 2021
 * @brief Holds definitions for basic program structures
 */

#ifndef IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
#define IOS_PROJECT2_STATIC_CONSTRUCTIONS_H

/**
 * @brief Holds all available return codes
 */
typedef enum returnCode
{
  NO_ERROR,
  ARGUMENT_COUNT_ERROR,
  INVALID_ARGUMENT_ERROR,
  ARGUMENT_OUT_OF_RANGE,
  SEMAPHOR_CREATION_ERROR,
  SEMAPHOR_INIT_FAILED,
  SEMAPHOR_DESTROY_ERROR,
  SM_CREATE_ERROR,
  SM_MAP_ERROR,
  SM_DESTROY_ERROR,
  PROCESS_CREATE_ERROR,
  OF_OPEN_ERROR,
  UNEXPECTED_ERROR,
} ReturnCode;

/**
 * @brief Holds all parameters extracted from arguments
 */
typedef struct
{
  int ne, nr, te, tr;
} Params;

#endif //IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
