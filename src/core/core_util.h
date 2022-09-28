#pragma once
#include "value/listutil.h"

#define EXPECT(condition, message, pos)                                                                                \
	if (!(condition)) return ERROR(pos, message)

void add_function(Env *env, char *name, Value *(*function)(Value *args));