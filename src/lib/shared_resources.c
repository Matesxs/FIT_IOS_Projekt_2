/**
 * @file shared_resources.c
 * @author Martin Douša
 * @date April 2021
 * @brief Holder for all shared variables
 */

#include "shared_resources.h"

ProcessHolder processHolder = { 0 };            /**< Holder for all information about process ids and its count */

Params params;                                  /**< Holder for parsed arguments */

SemHolder *semHolder = NULL;                    /**< Pointer to shared holder for semaphores */
volatile SharedMemory *sharedMemory = NULL;              /**< Pointer to shared memory holder */

FILE *outputFile = NULL;                        /**< Output stream pointer */