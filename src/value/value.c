#include "value.h"
#include "listutil.h"

Value *value_create(TextPos pos, ValueType type) {
	Value *value = malloc(sizeof(Value));
	value->pos = pos;
	value->type = type;
	return value;
}

Value *value_create_keyword(TextPos pos, KeywordType keyword) {
	Value *value = value_create(pos, VALUE_KEYWORD);
	value->as.keyword = keyword;
	return value;
}

Value *value_create_pair(TextPos pos, Value *first, Value *rest) {
	Value *value = value_create(pos, VALUE_PAIR);
	value->as.pair.first = first;
	value->as.pair.rest = rest;
	return value;
}

Value *value_create_number(TextPos pos, double number) {
	Value *value = value_create(pos, VALUE_NUMBER);
	value->as.number = number;
	return value;
}

Value *value_create_chars(TextPos pos, ValueType type, char *string, int stringLength) {
	Value *value = value_create(pos, type);
	value->as.chars = malloc(stringLength + 1);

	int i = 0, skippedChars = 0;
	for (; i < stringLength; i++) {
		if (string[i] == '\\') {
			skippedChars++;
			i++;
			switch (string[i]) {
				case 'a': value->as.chars[i - skippedChars] = '\a'; break;
				case 'b': value->as.chars[i - skippedChars] = '\b'; break;
				case 'e': value->as.chars[i - skippedChars] = '\e'; break;
				case 'f': value->as.chars[i - skippedChars] = '\f'; break;
				case 'n': value->as.chars[i - skippedChars] = '\n'; break;
				case 'r': value->as.chars[i - skippedChars] = '\r'; break;
				case 't': value->as.chars[i - skippedChars] = '\t'; break;
				case '\\': value->as.chars[i - skippedChars] = '\\'; break;
				case '\'': value->as.chars[i - skippedChars] = '\''; break;
				case '"': value->as.chars[i - skippedChars] = '"'; break;
				case '?': value->as.chars[i - skippedChars] = '\?'; break;
				default: break;
			}
		} else value->as.chars[i - skippedChars] = string[i];
	}

	value->as.chars[i - skippedChars] = '\0';
	return value;
}

Value *value_create_function(TextPos pos, Env *outer, Value *args, Value *body) {
	Value *value = value_create(pos, VALUE_FUNCTION);
	value->as.function.outer = outer;
	value->as.function.args = args;
	value->as.function.body = body;
	return value;
}

Value *value_create_c_function(TextPos pos, Value *(*cFunction)(Value *args)) {
	Value *value = value_create(pos, VALUE_C_FUNCTION);
	value->as.cFunction = cFunction;
	return value;
}

Value *value_copy(Value *value) {
	Value *newValue = malloc(sizeof(Value));
	memcpy(newValue, value, sizeof(Value));

	switch (value->type) {
		case VALUE_PAIR:
			FIRST(newValue) = value_copy(FIRST(value));
			REST(newValue) = value_copy(REST(value));
			break;
		case VALUE_SYMBOL:
		case VALUE_STRING:
		case VALUE_ERROR:
			if (value->as.chars != NULL) {
				int len = strlen(value->as.chars) + 1;
				newValue->as.chars = malloc(len);
				memcpy(newValue->as.chars, value->as.chars, len);
			} else {
				newValue->as.chars = NULL;
			}
			break;
		case VALUE_FUNCTION:
			newValue->as.function.args = value_copy(value->as.function.args);
			newValue->as.function.body = value_copy(value->as.function.body);
			break;
		case VALUE_KEYWORD:
		case VALUE_NIL:
		case VALUE_TRUE:
		case VALUE_FALSE:
		case VALUE_NUMBER:
		case VALUE_C_FUNCTION: break;
	}

	return newValue;
}

void value_destroy(Value *value) {
	if (value == NULL) return;

	switch (value->type) {
		case VALUE_PAIR:
			value_destroy(FIRST(value));
			value_destroy(REST(value));
			break;
		case VALUE_SYMBOL:
		case VALUE_STRING:
		case VALUE_ERROR:
			if (value->as.chars != NULL) {
				free(value->as.chars);
				value->as.chars = NULL;
			}
			break;
		case VALUE_FUNCTION:
			env_destroy(value->as.function.outer);
			value_destroy(value->as.function.args);
			value_destroy(value->as.function.body);
			break;
		case VALUE_KEYWORD:
		case VALUE_NIL:
		case VALUE_TRUE:
		case VALUE_FALSE:
		case VALUE_NUMBER:
		case VALUE_C_FUNCTION: break;
	}

	free(value);
	value = NULL;
}

// TODO: handle pairs
int value_print(Value *value) {
	if (value == NULL) return printf("NULL");

	switch (value->type) {
		case VALUE_NIL: return printf("nil");
		case VALUE_TRUE: return printf("true");
		case VALUE_FALSE: return printf("false");
		case VALUE_KEYWORD:
			switch (value->as.keyword) {
				case KEYWORD_DEF: return printf("def");
				case KEYWORD_LET: return printf("let");
				case KEYWORD_DO: return printf("do");
				case KEYWORD_IF: return printf("if");
				case KEYWORD_FN: return printf("fn");
				case KEYWORD_EVAL: return printf("eval");
				case KEYWORD_QUOTE: return printf("quote");
			}
		case VALUE_PAIR: {
			int charsPrinted = printf("(");
			ITERATE_LIST(i, value) {
				charsPrinted += value_print(FIRST(i));
				if (!IS_NIL(REST(i))) charsPrinted += printf(" ");
			}
			charsPrinted += printf(")");
			return charsPrinted;
		}

		case VALUE_SYMBOL: return printf("'%s'", value->as.chars);
		case VALUE_NUMBER: return printf("%g", value->as.number);
		case VALUE_STRING: return printf("\"%s\"", value->as.chars);
		case VALUE_FUNCTION: return printf("FUNCTION");
		case VALUE_C_FUNCTION: return printf("C_FUNCTION");
		case VALUE_ERROR:
			return printf(
				"<ERROR: %s at %s:%d:%d>", value->as.chars, value->pos.fileName, value->pos.row, value->pos.col);
	}

	return 0;
}