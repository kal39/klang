#include "core_util.h"

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

void add_core_basic(Env *core) {
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
}