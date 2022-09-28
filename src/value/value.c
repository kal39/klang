#include "value.h"
#include "listutil.h"
#include "value_tracker.h"

Value *_value_create(TextPos pos, ValueType type, char *file, int line) {
	Value *value = value_alloc(file, line);
	value->pos = pos;
	value->type = type;
	return value;
}

Value *_value_create_keyword(TextPos pos, KeywordType keyword, char *file, int line) {
	Value *value = _value_create(pos, VALUE_KEYWORD, file, line);
	value->as.keyword = keyword;
	return value;
}

Value *_value_create_pair(TextPos pos, Value *first, Value *rest, char *file, int line) {
	Value *value = _value_create(pos, VALUE_PAIR, file, line);
	value->as.pair.first = first;
	value->as.pair.rest = rest;
	return value;
}

Value *_value_create_number(TextPos pos, double number, char *file, int line) {
	Value *value = _value_create(pos, VALUE_NUMBER, file, line);
	value->as.number = number;
	return value;
}

Value *_value_create_chars(TextPos pos, ValueType type, char *string, int stringLength, char *file, int line) {
	Value *value = _value_create(pos, type, file, line);
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

Value *_value_create_function(TextPos pos, Env *outer, Value *args, Value *body, char *file, int line) {
	Value *value = _value_create(pos, VALUE_FUNCTION, file, line);
	value->as.function.outer = outer;
	value->as.function.args = args;
	value->as.function.body = body;
	return value;
}

Value *_value_create_c_function(TextPos pos, Value *(*cFunction)(Value *args), char *file, int line) {
	Value *value = _value_create(pos, VALUE_C_FUNCTION, file, line);
	value->as.cFunction = cFunction;
	return value;
}

Value *_value_copy(Value *value, char *file, int line) {
	switch (value->type) {
		case VALUE_KEYWORD: return _value_create_keyword(value->pos, value->as.keyword, file, line);
		case VALUE_NIL: return _value_create(value->pos, VALUE_NIL, file, line);
		case VALUE_TRUE: return _value_create(value->pos, VALUE_TRUE, file, line);
		case VALUE_FALSE: return _value_create(value->pos, VALUE_FALSE, file, line);
		case VALUE_PAIR:
			return _value_create_pair(value->pos,
									  _value_copy(value->as.pair.first, file, line),
									  _value_copy(value->as.pair.rest, file, line),
									  file,
									  line);
		case VALUE_SYMBOL:
		case VALUE_STRING:
		case VALUE_ERROR:
			return _value_create_chars(value->pos, value->type, value->as.chars, strlen(value->as.chars), file, line);
		case VALUE_NUMBER: return _value_create_number(value->pos, value->as.number, file, line);
		case VALUE_FUNCTION:
			return _value_create_function(
				value->pos, value->as.function.outer, value->as.function.args, value->as.function.body, file, line);
		case VALUE_C_FUNCTION: return _value_create_c_function(value->pos, value->as.cFunction, file, line);
	}
	return NULL;
}

void _value_destroy(Value *value, char *file, int line) {
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

	value_free(value, file, line);
}

// TODO: handle pairs
int value_print(Value *value) {
	if (value == NULL) return printf("NULL");

	switch (value->type) {
		case VALUE_NIL: return printf("NIL");
		case VALUE_TRUE: return printf("TRUE");
		case VALUE_FALSE: return printf("FALSE");
		case VALUE_KEYWORD:
			switch (value->as.keyword) {
				case KEYWORD_DEF: return printf("DEF");
				case KEYWORD_LET: return printf("LET");
				case KEYWORD_DO: return printf("DO");
				case KEYWORD_IF: return printf("IF");
				case KEYWORD_FN: return printf("FN");
				case KEYWORD_EVAL: return printf("EVAL");
				case KEYWORD_QUOTE: return printf("QUOTE");
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
		case VALUE_FUNCTION: {
			int charsPrinted = printf("<FUNCTION, args: ");
			charsPrinted += value_print(value->as.function.args);
			charsPrinted += printf(", body: ");
			charsPrinted += value_print(value->as.function.body);
			charsPrinted += printf(">");
			return charsPrinted;
		}
		case VALUE_C_FUNCTION: return printf("<C_FUNCTION, address: %p>", value->as.cFunction);
		case VALUE_ERROR:
			if (IS_TEXT_POS_NONE(value->pos))
				return printf("<ERROR: %s at UNKNOWN:%d:%d>", value->as.chars, value->pos.row, value->pos.col);
			else
				return printf(
					"<ERROR: %s at %s:%d:%d>", value->as.chars, value->pos.fileName, value->pos.row, value->pos.col);
	}

	return 0;
}