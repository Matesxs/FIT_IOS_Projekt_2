/**
 * @file static_constructions.h
 * @author Martin Douša
 * @date April 2021
 * @brief Holds definitions for basic program structures
 */

#ifndef IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
#define IOS_PROJECT2_STATIC_CONSTRUCTIONS_H

#include <stdbool.h>
#include <stdint.h>
#include <semaphore.h>

/**
 * @struct process_holder
 * @brief Structure for holding information about processes
 */
typedef struct process_holder
{
  pid_t mainId;                   /**< Process id of main process (available for all child processes) */
  
  pid_t *elfIds;                  /**< Array of process ids for all elves */
  size_t elvesCount;              /**< Length of elf ids array */

  pid_t *rdIds;                   /**< Array of process ids for all reindeers */
  size_t rdCount;                 /**< Length of reindeer ids array */

  pid_t santaId;                  /**< Process id of Santa process */
} ProcessHolder;

/**
 * @struct sem_holder
 * @brief Struct for holding all semaphores
 */
typedef struct sem_holder
{
  sem_t writeOutLock;             /**< Semaphore for writing to output file */
  sem_t rdWaitForHitch;           /**< Semaphore for reindeers to wait for hitch */
  sem_t rdHitched;                /**< Semaphore for indicating that reindeer was hitched */
  sem_t waitInQueue;              /**< Semaphore for elves to queue when Santa is helping another 3 elves */
  sem_t waitForHelp;              /**< Semaphore for elves that are on the front of queue and will get help from Santa */
  sem_t elfHelped;                /**< Semaphore for indicating that elf get help */
  sem_t wakeForHelp;              /**< Semaphore for third elf in queue to wake up Santa for helping */
  sem_t santaReady;               /**< Semaphore signalizing that Santa is not doing something else and can be woken up */
  sem_t childFinished;            /**< Semaphore signalizing exiting of child process */
  sem_t rdReadyCountMutex;        /**< Mutex for handling ready reindeers */
  sem_t elfQueueMutex;            /**< Mutex for managing elf queue */
  sem_t christmasStarted;         /**< Semaphore signalizing that Christmas started */
  sem_t numOfElvesStable;         /**< Semaphore to signalize that number of elves will not change */
} SemHolder;

/**
 * @struct shared_memory
 * @brief Struct for holding shared memory
 */
typedef struct shared_memory
{
  int readyRDCount;               /**< Counter for reindeers that returned from vacation */
  int elfReadyQueue;              /**< Counter for elves ready to get help */
  size_t numberOfElves;           /**< Mirror of allocated elves (security reasons) */
  bool shopClosed;                /**< Flag representing if workshop is closed */
  int actionId;                   /**< Action counter for output line indexing */
} SharedMemory;

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
 * @struct prmtrs
 * @brief Holds all parameters extracted from arguments
 */
typedef struct prmtrs
{
  int ne;                         /**< Number of elves to generate */
  int nr;                         /**< Number of reindeers to generate */
  int te;                         /**< Max work time of elf */
  int tr;                         /**< Max vacation time of reindeer */
  bool bflag;                     /**< Extension flag for generating more elves on USR1 signal */
} Params;

#endif //IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
