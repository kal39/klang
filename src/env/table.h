#pragma once

// TODO: remove elements form table
// TODO: calculate hash at string creation

#include "value/value.h"

typedef struct Value Value;

typedef struct Entry {
	char *key;
	Value *value;
} Entry;

typedef struct Table {
	int capacity;
	int size;
	Entry *entries;
} Table;

Table *table_create();
void table_destroy(Table *table);

void table_set(Table *table, char *key, Value *value);
Value *table_get(Table *table, char *key);

void table_print(Table *table);