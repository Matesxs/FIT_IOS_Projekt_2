/**
 * @file shared_resources.c
 * @author Martin Douša
 * @date April 2021
 * @brief Holder for all shared variables
 */

#include "shared_resources.h"

pid_t pid_mainprocess = 0;                      /**< Holds process id of main process */

pid_t *elf_processes = NULL;                    /**< Pointer to array of generated elves */
size_t elves_count = 0;                         /**< Counter that represents number of elves created */
ReturnCode globalElvesReturncode = NO_ERROR;    /**< ReturnCode for checking if there was no error when creating new elves */

pid_t *rd_processes = NULL;                     /**< Pointer to array of generated raindeers */
size_t rd_count = 0;                            /**< Counter that represents number of raindeers created */

pid_t santa_process = 0;                        /**< Process id of Santa */

Params params;                                  /**< Holder for parsed arguments */

SemHolder *semHolder = NULL;                    /**< Pointer to shared holder for semaphores */
SharedMemory *sharedMemory = NULL;              /**< Pointer to shared memory holder */

FILE *outputFile = NULL;                        /**< Output stream pointer */