#pragma once

#include "table.h"

typedef struct Env Env;
typedef struct Table Table;
typedef struct Value Value;

typedef struct Env {
	Env *outer;
	Table *table;
} Env;

#define ADD_FUNCTION(env, symbol, function)                                                                            \
	env_set(env,                                                                                                       \
			value_create_chars(TEXT_POS_NONE, VALUE_SYMBOL, symbol, strlen(symbol)),                                   \
			value_create_c_function(TEXT_POS_NONE, function))

Env *env_create(Env *outer);
void env_destroy(Env *env);

void env_set(Env *env, Value *key, Value *value);
Value *env_get(Env *env, Value *key);
void env_print(Env *env);