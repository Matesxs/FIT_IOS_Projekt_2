//
// Created by matesxs on 4/9/21.
//

#ifndef IOS_PROJECT2_SHARED_RESOURCES_H
#define IOS_PROJECT2_SHARED_RESOURCES_H

#include <stdio.h>
#include <semaphore.h>

#include "static_constructions.h"

extern SemHolder *semHolder;
extern SharedMemory *sharedMemory;

// Mic
extern Params params;
extern FILE *outputFile;
extern ProcessHolder processHolder;
extern ReturnCode globalElvesReturncode;

#endif //IOS_PROJECT2_SHARED_RESOURCES_H
