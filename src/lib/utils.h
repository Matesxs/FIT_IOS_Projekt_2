#pragma once

#include <semaphore.h>

#include "static_constructions.h"
#include "shared_resources.h"
#include "error_handling.h"

ReturnCode parseArguments(int argc, char *argv[]);
void initSignals();