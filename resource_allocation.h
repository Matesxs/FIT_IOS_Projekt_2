//
// Created by matesxs on 4/9/21.
//

#ifndef IOS_PROJECT2_RESOURCE_ALLOCATION_H
#define IOS_PROJECT2_RESOURCE_ALLOCATION_H

#include "static_constructions.h"

#include <sys/mman.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>

#include "shared_resources.h"

ReturnCode deallocateResources();
ReturnCode allocateResources();

#endif //IOS_PROJECT2_RESOURCE_ALLOCATION_H
