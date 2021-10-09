/**
 * @file utils.h
 * @author Martin Douša
 * @date April 2021
 * @brief Holds definitions for utility functions
 */

#pragma once

#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "static_constructions.h"
#include "shared_resources.h"
#include "error_handling.h"

#define NO_ID -1

void handleUsrSignal();
void initSignals();
ReturnCode parseArguments(int argc, char *argv[]);
void printToOutput(char *entityName, int id, char *message);