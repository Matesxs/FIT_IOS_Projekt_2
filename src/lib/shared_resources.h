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

extern pid_t pid_mainprocess;

extern pid_t *elf_processes;
extern size_t elves_count;
extern ReturnCode globalElvesReturncode;

extern pid_t *rd_processes;
extern size_t rd_count;

extern pid_t santa_process;

#endif //IOS_PROJECT2_SHARED_RESOURCES_H
