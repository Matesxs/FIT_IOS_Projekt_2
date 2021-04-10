//
// Created by matesxs on 4/9/21.
//

#ifndef IOS_PROJECT2_ERROR_HANDLING_H
#define IOS_PROJECT2_ERROR_HANDLING_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "static_constructions.h"
#include "resource_allocation.h"

void terminate();
void handleErrors(ReturnCode code);

#endif //IOS_PROJECT2_ERROR_HANDLING_H
