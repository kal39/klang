#include "../core_util.h"

static Value *_cons(Value *args) {
	EXPECT(list_length(args) == 2, "expected 2 argument", args->pos);
	return value_create_pair(TEXT_POS_NONE, FIRST(args), SECOND(args));
}

static Value *_first(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	return FIRST(FIRST(args));
}

static Value *_rest(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	return REST(FIRST(args));
}

static Value *_list(Value *args) {
	Value *list = list_create(TEXT_POS_NONE);
	ITERATE_LIST(i, args) list_add(list, FIRST(i));
	return list;
}

static Value *_is_list(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	return IS_PAIR(FIRST(args)) || IS_NIL(FIRST(args)) ? value_create(TEXT_POS_NONE, VALUE_TRUE) :
														 value_create(TEXT_POS_NONE, VALUE_FALSE);
}

static Value *_is_empty(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	return IS_NIL(FIRST(args)) ? value_create(TEXT_POS_NONE, VALUE_TRUE) : value_create(TEXT_POS_NONE, VALUE_FALSE);
}

static Value *_count(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	if (IS_NIL(FIRST(args))) return value_create_number(TEXT_POS_NONE, 0);
	EXPECT(IS_PAIR(FIRST(args)), "expected list", FIRST(args)->pos);
	return value_create_number(TEXT_POS_NONE, list_length(FIRST(args)));
}

void add_core_list(Env *core) {
	add_function(core, "cons", _cons);
	add_function(core, "first", _first);
	add_function(core, "rest", _rest);
	add_function(core, "list", _list);
	add_function(core, "list?", _is_list);
	add_function(core, "empty?", _is_empty);
	add_function(core, "count", _count);
}