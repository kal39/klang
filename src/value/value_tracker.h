#pragma once

#include "value.h"

#ifdef ENABLE_VALUE_TRACKER
#define value_alloc(file, line) _value_alloc(file, line)
#define value_free(value, file, line) _value_free(value, file, line)
#else
#define value_alloc(file, line) malloc(sizeof(Value))
#define value_free(value, file, line) free(value)
#endif

Value *_value_alloc(char *file, int line);
void _value_free(Value *value, char *file, int line);
void print_value_tracker_status(bool printFree);