#pragma once

#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "static_constructions.h"
#include "shared_resources.h"

void handleUsrSignal();
ReturnCode parseArguments(int argc, char *argv[]);