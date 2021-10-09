/**
 * @file shared_resources.h
 * @author Martin Douša
 * @date April 2021
 * @brief Holds definitions shared global variables and structures
 */

#ifndef IOS_PROJECT2_SHARED_RESOURCES_H
#define IOS_PROJECT2_SHARED_RESOURCES_H

#include <stdio.h>
#include <semaphore.h>

#include "static_constructions.h"

extern SemHolder *semHolder;
extern volatile SharedMemory *sharedMemory;

// Mic
extern Params params;
extern FILE *outputFile;
extern ProcessHolder processHolder;

#endif //IOS_PROJECT2_SHARED_RESOURCES_H
