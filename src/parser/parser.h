#pragma once

#include "scanner.h"
#include "value/value.h"

Value *parse_string(char *string, char *fileName);
Value *parse_file(char *fileName);