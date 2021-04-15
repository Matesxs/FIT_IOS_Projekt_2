//
// Created by matesxs on 4/11/21.
//

#ifndef IOS_PROJECT2_PROCESS_HANDLERS_H
#define IOS_PROJECT2_PROCESS_HANDLERS_H

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "static_constructions.h"
#include "shared_resources.h"

void handle_elf(int id, Params params);
void handle_rd(int id, Params params);
void handle_santa(Params params);

#endif //IOS_PROJECT2_PROCESS_HANDLERS_H
