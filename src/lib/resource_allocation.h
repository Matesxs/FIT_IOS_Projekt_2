/**
 * @file resource_allocation.h
 * @author Martin Douša
 * @date April 2021
 * @brief Definitions for resource allocation
 */

#ifndef IOS_PROJECT2_RESOURCE_ALLOCATION_H
#define IOS_PROJECT2_RESOURCE_ALLOCATION_H

#include "static_constructions.h"

#include <sys/mman.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "shared_resources.h"

ReturnCode deallocateResources();
ReturnCode allocateResources();

#endif //IOS_PROJECT2_RESOURCE_ALLOCATION_H
