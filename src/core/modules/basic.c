#include "../core_util.h"
#include "parser/parser.h"

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

static Value *_string(Value *args) {
	EXPECT(list_length(args) >= 1, "expected 1+ arguments", args->pos);

	int totalLen = 0;
	ITERATE_LIST(i, args) {
		switch (FIRST(i)->type) {
			case VALUE_NIL: totalLen += snprintf(NULL, 0, "nil"); break;
			case VALUE_TRUE: totalLen += snprintf(NULL, 0, "true"); break;
			case VALUE_FALSE: totalLen += snprintf(NULL, 0, "false"); break;
			case VALUE_NUMBER: totalLen += snprintf(NULL, 0, "%g", FIRST(i)->as.number); break;
			case VALUE_STRING: totalLen += snprintf(NULL, 0, "%s", FIRST(i)->as.chars); break;
			default: return ERROR(FIRST(i)->pos, "expected printable");
		}
	}

	char *string = malloc(totalLen + 1);
	string[0] = '\0';

	ITERATE_LIST(i, args) {
		switch (FIRST(i)->type) {
			case VALUE_NIL: sprintf(string, "nil"); break;
			case VALUE_TRUE: sprintf(string + strlen(string), "true"); break;
			case VALUE_FALSE: sprintf(string + strlen(string), "false"); break;
			case VALUE_NUMBER: sprintf(string + strlen(string), "%g", FIRST(i)->as.number); break;
			case VALUE_STRING: sprintf(string + strlen(string), "%s", FIRST(i)->as.chars); break;
			default: return ERROR(FIRST(i)->pos, "expected printable");
		}
	}

	Value *value = value_create(TEXT_POS_NONE, VALUE_STRING);
	value->as.chars = string;
	return value;
}

static Value *_parse_string(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);
	return FIRST(parse_string(FIRST(args)->as.chars, "NIL"));
}

void add_core_basic(Env *core) {
	add_function(core, "+", _add);
	add_function(core, "-", _subtract);
	add_function(core, "*", _multiply);
	add_function(core, "/", _divide);
	add_function(core, "=", _equals);
	add_function(core, "<", _less_than);
	add_function(core, "<=", _less_than_equals);
	add_function(core, ">", _greater_than);
	add_function(core, ">=", _greater_than_equals);
	add_function(core, "!", _not);
	add_function(core, "string", _string);
	add_function(core, "parse-string", _parse_string);
}