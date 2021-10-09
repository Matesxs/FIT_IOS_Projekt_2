/**
 * @file process_handlers.h
 * @author Martin Douša
 * @date April 2021
 * @brief Definitions for worker functions
 */

#ifndef IOS_PROJECT2_PROCESS_HANDLERS_H
#define IOS_PROJECT2_PROCESS_HANDLERS_H

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "static_constructions.h"
#include "shared_resources.h"
#include "error_handling.h"
#include "utils.h"

void addElves();
void handle_elf(size_t id);
void handle_rd(size_t id);
void handle_santa();

#endif //IOS_PROJECT2_PROCESS_HANDLERS_H
