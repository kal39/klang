#pragma once

#include "table.h"

typedef struct Env Env;
typedef struct Table Table;
typedef struct Value Value;

typedef struct Env {
	Env *outer;
	Table *table;
} Env;

Env *env_create(Env *outer);
void env_destroy(Env *env);

void env_set(Env *env, Value *key, Value *value);
Value *env_get(Env *env, Value *key);
void env_print(Env *env);