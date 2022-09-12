#include "core.h"
#include "value/listutil.h"

#define EXPECT(condition, message, pos)                                                                                \
	if (!(condition)) return ERROR(pos, message)

static bool __equals(Value *a, Value *b) {
	if (a->type != b->type) return false;

	switch (a->type) {
		case VALUE_NIL:
		case VALUE_TRUE:
		case VALUE_FALSE: return true;
		case VALUE_PAIR: {
			int lenA = list_length(a);
			int lenB = list_length(b);
			if (lenA != lenB) return false;
			for (int i = 0; i < lenA; i++) {
				if (!__equals(FIRST(a), FIRST(b))) return false;
			}
			return true;
		}
		case VALUE_NUMBER: return a->as.number == b->as.number;
		case VALUE_STRING: {
			int lenA = strlen(a->as.chars);
			int lenB = strlen(b->as.chars);
			return lenA == lenB && memcmp(a->as.chars, b->as.chars, lenA) == 0 ? true : false;
		}
		default: return false;
	}
}

static Value *_add(Value *args) {
	double result = 0;
	ITERATE_LIST(i, args) {
		EXPECT(IS_NUMBER(FIRST(i)), "expected number", FIRST(i)->pos);
		result += FIRST(i)->as.number;
	}
	return value_create_number(TEXT_POS_NONE, result);
}

static Value *_subtract(Value *args) {
	switch (list_length(args)) {
		case 0: return ERROR(args->pos, "expected 1+ arguments");
		case 1:
			EXPECT(IS_NUMBER(FIRST(args)), "expected number", FIRST(args)->pos);
			return value_create_number(TEXT_POS_NONE, -FIRST(args)->as.number);
		default: {
			double result = FIRST(args)->as.number;
			ITERATE_LIST(i, REST(args)) {
				EXPECT(IS_NUMBER(FIRST(i)), "expected number", FIRST(i)->pos);
				result -= FIRST(i)->as.number;
			}
			return value_create_number(TEXT_POS_NONE, result);
		}
	}
}

static Value *_multiply(Value *args) {
	double result = 1;
	ITERATE_LIST(i, args) {
		EXPECT(IS_NUMBER(FIRST(i)), "expected number", FIRST(i)->pos);
		result *= FIRST(i)->as.number;
	}
	return value_create_number(TEXT_POS_NONE, result);
}

static Value *_divide(Value *args) {
	switch (list_length(args)) {
		case 0: return ERROR(args->pos, "expected 1+ arguments");
		case 1:
			EXPECT(IS_NUMBER(FIRST(args)), "expected number", FIRST(args)->pos);
			return value_create_number(TEXT_POS_NONE, 1.0 / FIRST(args)->as.number);
		default: {
			double result = FIRST(args)->as.number;
			ITERATE_LIST(i, REST(args)) {
				EXPECT(IS_NUMBER(FIRST(i)), "expected number", FIRST(i)->pos);
				result /= FIRST(i)->as.number;
			}
			return value_create_number(TEXT_POS_NONE, result);
		}
	}
}

static Value *_equals(Value *args) {
	EXPECT(list_length(args) >= 2, "expected 2+ arguments", args->pos);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (!__equals(FIRST(i), SECOND(i))) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	}
	return value_create(TEXT_POS_NONE, VALUE_TRUE);
}

static Value *_less_than(Value *args) {
	EXPECT(list_length(args) >= 2, "expected 2+ arguments", args->pos);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (FIRST(i)->as.number >= SECOND(i)->as.number) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	}
	return value_create(TEXT_POS_NONE, VALUE_TRUE);
}

static Value *_less_than_equals(Value *args) {
	EXPECT(list_length(args) >= 2, "expected 2+ arguments", args->pos);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (FIRST(i)->as.number > SECOND(i)->as.number) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	}
	return value_create(TEXT_POS_NONE, VALUE_TRUE);
}

static Value *_greater_than(Value *args) {
	EXPECT(list_length(args) >= 2, "expected 2+ arguments", args->pos);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (FIRST(i)->as.number <= SECOND(i)->as.number) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	}
	return value_create(TEXT_POS_NONE, VALUE_TRUE);
}

static Value *_greater_than_equals(Value *args) {
	EXPECT(list_length(args) >= 2, "expected 2+ arguments", args->pos);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (FIRST(i)->as.number < SECOND(i)->as.number) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	}
	return value_create(TEXT_POS_NONE, VALUE_TRUE);
}

static Value *_not(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 arguments", args->pos);
	if (IS_TRUE(FIRST(args))) return value_create(TEXT_POS_NONE, VALUE_FALSE);
	if (IS_FALSE(FIRST(args))) return value_create(TEXT_POS_NONE, VALUE_TRUE);
	return ERROR(FIRST(args)->pos, "expected boolean");
}

// static Value *_cons(Value *args) {
// 	EXPECT(list_length(args) == 2, "expected 2 arguments", NULL);
// 	return MAKE_PAIR(FIRST(args), SECOND(args));
// }

// static Value *_first(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	return FIRST(FIRST(args));
// }

// static Value *_rest(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	return REST(FIRST(args));
// }

// static Value *_list(Value *args) {
// 	Value *list = MAKE_LIST();
// 	ITERATE_LIST(i, args) ADD_VALUE(list, FIRST(i));
// 	return list;
// }

// static Value *_is_list(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	return IS_LIST(FIRST(args)) || IS_NIL(FIRST(args)) ? MAKE_TRUE() : MAKE_FALSE();
// }

// static Value *_is_empty(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	return IS_NIL(FIRST(args)) ? MAKE_TRUE() : MAKE_FALSE();
// }

// static Value *_count(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	if (IS_NIL(FIRST(args))) return MAKE_NUMBER(0);
// 	EXPECT(IS_LIST(FIRST(args)), "expected list", FIRST(args));
// 	return MAKE_NUMBER(list_length(FIRST(args)));
// }

// static Value *_string(Value *args) {
// 	EXPECT(list_length(args) >= 1, "expected 1+ arguments", NULL);

// 	int totalLen = 0;
// 	ITERATE_LIST(i, args) {
// 		switch (TYPE(FIRST(i))) {
// 			case VALUE_NIL: totalLen += snprintf(NULL, 0, "nil"); break;
// 			case VALUE_TRUE: totalLen += snprintf(NULL, 0, "true"); break;
// 			case VALUE_FALSE: totalLen += snprintf(NULL, 0, "false"); break;
// 			case VALUE_NUMBER: totalLen += snprintf(NULL, 0, "%g", AS_NUMBER(FIRST(i))); break;
// 			case VALUE_STRING: totalLen += snprintf(NULL, 0, "%s", AS_STRING(FIRST(i))); break;
// 			default: return MAKE_ERROR("expected printable", NULL, FIRST(i));
// 		}
// 	}

// 	char *string = malloc(totalLen + 1);
// 	string[0] = '\0';

// 	ITERATE_LIST(i, args) {
// 		switch (TYPE(FIRST(i))) {
// 			case VALUE_NIL: sprintf(string, "nil"); break;
// 			case VALUE_TRUE: sprintf(string + strlen(string), "true"); break;
// 			case VALUE_FALSE: sprintf(string + strlen(string), "false"); break;
// 			case VALUE_NUMBER: sprintf(string + strlen(string), "%g", AS_NUMBER(FIRST(i))); break;
// 			case VALUE_STRING: sprintf(string + strlen(string), "%s", AS_STRING(FIRST(i))); break;
// 			default: return MAKE_ERROR("expected printable", NULL, FIRST(i));
// 		}
// 	}

// 	Value *value = value_create(VALUE_STRING);
// 	AS_STRING(value) = string;
// 	return value;
// }

// static Value *_print(Value *args) {
// 	EXPECT(list_length(args) >= 1, "expected 1+ arguments", NULL);
// 	Value *string = _string(args);
// 	printf("%s", AS_STRING(string));
// 	return MAKE_NIL();
// }

// static Value *_println(Value *args) {
// 	_print(args);
// 	printf("\n");
// 	return MAKE_NIL();
// }

// static Value *_print_debug(Value *args) {
// 	EXPECT(list_length(args) >= 1, "expected 1+ arguments", NULL);
// 	ITERATE_LIST(i, args) value_print(FIRST(i));
// 	return MAKE_NIL();
// }

// static Value *_println_debug(Value *args) {
// 	_print_debug(args);
// 	printf("\n");
// 	return MAKE_NIL();
// }

// static Value *_read_file(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args));
// 	char *string = read_file(AS_STRING(FIRST(args)));
// 	if (string == NULL) return MAKE_ERROR("file not found", NULL, FIRST(args));
// 	else return MAKE_STRING(string);
// }

// static Value *_parse_string(Value *args) {
// 	EXPECT(list_length(args) == 1, "expected 1 arguments", NULL);
// 	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args));
// 	return FIRST(parse_string(AS_STRING(FIRST(args))));
// }

Env *make_core() {
	Env *core = env_create(NULL);

	ADD_FUNCTION(core, "+", _add);
	ADD_FUNCTION(core, "-", _subtract);
	ADD_FUNCTION(core, "*", _multiply);
	ADD_FUNCTION(core, "/", _divide);
	ADD_FUNCTION(core, "=", _equals);
	ADD_FUNCTION(core, "<", _less_than);
	ADD_FUNCTION(core, "<=", _less_than_equals);
	ADD_FUNCTION(core, ">", _greater_than);
	ADD_FUNCTION(core, ">=", _greater_than_equals);
	ADD_FUNCTION(core, "!", _not);
	// ADD_FUNCTION(core, "cons", _cons);
	// ADD_FUNCTION(core, "first", _first);
	// ADD_FUNCTION(core, "rest", _rest);
	// ADD_FUNCTION(core, "list", _list);
	// ADD_FUNCTION(core, "list?", _is_list);
	// ADD_FUNCTION(core, "empty?", _is_empty);
	// ADD_FUNCTION(core, "count", _count);
	// ADD_FUNCTION(core, "string", _string);
	// ADD_FUNCTION(core, "print", _print);
	// ADD_FUNCTION(core, "println", _println);
	// ADD_FUNCTION(core, "print-dbg", _print_debug);
	// ADD_FUNCTION(core, "println-dbg", _println_debug);
	// ADD_FUNCTION(core, "read-file", _read_file);
	// ADD_FUNCTION(core, "parse-string", _parse_string);

	return core;
}
