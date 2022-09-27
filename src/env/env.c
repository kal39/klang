#include "env.h"

Env *env_create(Env *outer) {
	Env *env = malloc(sizeof(Env));
	env->outer = outer;
	env->table = table_create();
	return env;
}

void env_destroy(Env *env) {
	table_destroy(env->table);
	if (env->outer != NULL) env_destroy(env->outer);
	free(env);
}

void env_set(Env *env, Value *key, Value *value) {
	if (!IS_SYMBOL(key)) return;
	table_set(env->table, key->as.chars, value);
}

Value *env_get(Env *env, Value *key) {
	if (!IS_SYMBOL(key)) return ERROR(key->pos, "expected symbol");
	Value *value = table_get(env->table, key->as.chars);

	if (value != NULL) return value_copy(value);
	else if (env->outer != NULL) return env_get(env->outer, key);
	else return value_create(key->pos, VALUE_NIL);
}

void env_print(Env *env) {
	table_print(env->table);
	printf("\n");
	if (env->outer != NULL) env_print(env->outer);
}