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
  pid_t mainId;                   /**< Process id of main process */
  
  pid_t *elfIds;                  /**< Array of process ids for all elves */
  size_t elvesCount;              /**< Length of elf ids array */

  pid_t *rdIds;                   /**< Array of process ids for all raindeers */
  size_t rdCount;                 /**< Length of raindeer ids array */

  pid_t santaId;                  /**< Process id of Santa process */
} ProcessHolder;

/**
 * @struct sem_holder
 * @brief Struct for holding all semaphores
 */
typedef struct sem_holder
{
  sem_t writeOutLock;             /**< Semaphore for writing to output file */
  sem_t rdWaitForHitch;           /**< Semaphore for raindeers to wait for hitch */
  sem_t rdHitched;                /**< Semaphore for indicating that raindeer was hitched */
  sem_t getHelp;                  /**< Semaphore for elves that are on the front of queue and will get help from Santa */
  sem_t waitForHelp;              /**< Semaphore for elves to queue when Santa is helping another 3 elves */
  sem_t elfHelped;                /**< Semaphore for indicating that elf get help */
  sem_t wakeForHelp;              /**< Semaphore for third elf in queue to wake up Santa for helping */
  sem_t wakeForHitch;             /**< Semaphore for last raindeer to wake up Santa for hitching */
  sem_t santaReady;               /**< Semaphore signalizing that Santa is not doing something else and can be woken up */
  sem_t santaFinished;            /**< Semaphore signalizing that Santa process finished */
  sem_t elfFinished;              /**< Semaphore signalizing that elf process finished */
  sem_t rdFinished;               /**< Semaphore signalizing that raindeer process finished */
} SemHolder;

/**
 * @struct shared_memory
 * @brief Struct for holding shared memory
 */
typedef struct shared_memory
{
  int readyRDCount;               /**< Counter for raindeers that returned from vacation */
  int elfReadyQueue;              /**< Counter for elves ready to get help */
  int spawnedElves;               /**< Count every spawned elf */
  bool shopClosed;                /**< Flag representing if workshop is closed */
  int actionId;                   /**< Action counter for output line indexing */
  bool christmasStarted;          /**< Flag representing if Christmas started */
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
  int nr;                         /**< Number of raindeers to generate */
  int te;                         /**< Max work time of elf */
  int tr;                         /**< Max vacation time of raindeer */
  bool pflag;                     /**< Extension flag for generating more elves on USR1 signal */
} Params;

#endif //IOS_PROJECT2_STATIC_CONSTRUCTIONS_H
