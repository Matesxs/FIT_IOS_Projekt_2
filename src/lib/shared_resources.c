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

sem_t *writeOutLock = NULL;                     /**< Semaphore for writing to output file */
sem_t *rdWaitForHitch = NULL;                   /**< Semaphore for raindeers to wait for hitch */
sem_t *rdHitched = NULL;                        /**< Semaphore for indicating that raindeer was hitched */
sem_t *wakeForHitch = NULL;                     /**< Semaphore for last raindeer to wake up Santa for hitching */
sem_t *waitForHelp = NULL;                      /**< Semaphore for elves to queue when Santa is helping another 3 elves */
sem_t *getHelp = NULL;                          /**< Semaphore for elves that are on the front of queue and will get help from Santa */
sem_t *elfHelped = NULL;                        /**< Semaphore for indicating that elf get help */
sem_t *wakeForHelp = NULL;                      /**< Semaphore for third elf in queue to wake up Santa for helping */
sem_t *santaReady = NULL;                       /**< Semaphore signalizing that Santa is not doing something else and can be woken up */
sem_t *santaFinished = NULL;                    /**< Semaphore signalizing that Santa process finished */
sem_t *elfFinished = NULL;                      /**< Semaphore signalizing that elf process finished */
sem_t *rdFinished = NULL;                       /**< Semaphore signalizing that raindeer process finished */

int shm_readyRDCount_id = 0;                    /**< Shrared memory id for readyRDCount */
int *readyRDCount = NULL;                       /**< Counter for raindeers that returned from vacation */
int shm_elfReadyQueue_id = 0;                   /**< Shrared memory id for elfReadyQueue */
int *elfReadyQueue = NULL;                      /**< Counter for elves ready to get help */
int shm_shopClosed_id = 0;                      /**< Shrared memory id for shopClosed */
int *shopClosed = NULL;                         /**< Flag representing if workshop is closed */
int shm_actionId_id = 0;                        /**< Shrared memory id for actionId */
int *actionId = NULL;                           /**< Action counter for output line indexing */
int shm_christmasStarted_id = 0;                /**< Shrared memory id for christmasStarted */
int *christmasStarted = NULL;                   /**< Flag representing if Christmas started */

FILE *outputFile = NULL;                        /**< Output stream pointer */