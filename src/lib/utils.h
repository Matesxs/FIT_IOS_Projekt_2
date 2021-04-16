#pragma once

<<<<<<< HEAD
#include <semaphore.h>
=======
#include <signal.h>
#include <string.h>
#include <stdlib.h>
>>>>>>> 25f1e65be579c361d82efda2a9384e6efd549219

#include "static_constructions.h"
#include "shared_resources.h"
#include "error_handling.h"

<<<<<<< HEAD
ReturnCode parseArguments(int argc, char *argv[]);
void initSignals();
=======
void handleUsrSignal();
void initSignals();
ReturnCode parseArguments(int argc, char *argv[]);
>>>>>>> 25f1e65be579c361d82efda2a9384e6efd549219
