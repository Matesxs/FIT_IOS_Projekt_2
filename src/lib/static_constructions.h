/**
 * @file static_constructions.h
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
  NO_ERROR = 0,                   /**< No error detected */
  ARGUMENT_COUNT_ERROR = 1,       /**< Number of arguments is incompatible */
  INVALID_ARGUMENT_ERROR = 2,     /**< Passed arguments are in wrong format or wrong value */
  SEMAPHOR_INIT_FAILED = 4,       /**< Failed to initialize semaphores to default values */
  SEMAPHOR_DESTROY_ERROR = 8,     /**< Failed to destroy semaphores */
  SM_CREATE_ERROR = 16,           /**< Failed to allocate shared memory */
  SM_DESTROY_ERROR = 32,          /**< Failed to deallocate shared memory */
  PROCESS_CREATE_ERROR = 64,      /**< Failed to create subprocess */
  OF_OPEN_ERROR = 128,            /**< Failed to open output file */
  PID_ALLOCATION_ERROR = 256,     /**< Failed to allocate store for process ids */
  UNEXPECTED_ERROR = 512,         /**< Unknown error that should't happen */
} ReturnCode;

/**
 * @brief Holds all parameters extracted from arguments
 */
typedef struct
{
  int ne;     /**< Number of elves to generate */
  int nr;     /**< Number of raindeers to generate */
  int te;     /**< Max work time of elf */
  int tr;     /**< Max vacation time of raindeer */
  bool pflag; /**< Extension flag for generating more elves on USR1 signal */
} Params;

#endif //IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
