#include "core_util.h"

void add_function(Env *env, char *name, Value *(*function)(Value *args)) {
	Value *key = value_create_chars(TEXT_POS_NONE, VALUE_SYMBOL, name, strlen(name));
	Value *value = value_create_c_function(TEXT_POS_NONE, function);
	env_set(env, key, value);
	value_destroy(key);
	value_destroy(value);
}