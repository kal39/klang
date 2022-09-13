#include "../core_util.h"
#include "parser/parser.h"

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

static Value *_print(Value *args) {
	EXPECT(list_length(args) >= 1, "expected 1+ arguments", args->pos);
	Value *string = _string(args);
	printf("%s", string->as.chars);
	value_destroy(string);
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_println(Value *args) {
	_print(args);
	printf("\n");
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_print_debug(Value *args) {
	EXPECT(list_length(args) >= 1, "expected 1+ arguments", args->pos);
	ITERATE_LIST(i, args) value_print(FIRST(i));
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_println_debug(Value *args) {
	_print_debug(args);
	printf("\n");
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_read_file(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);

	FILE *fp = fopen(FIRST(args)->as.chars, "rb");
	if (fp == NULL) return ERROR(FIRST(args)->pos, "file not found");

	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);

	char *buff = (char *)malloc(fileSize + 1);
	fread(buff, sizeof(char), fileSize, fp);
	buff[fileSize] = '\0';

	fclose(fp);
	return value_create_chars(TEXT_POS_NONE, VALUE_STRING, buff, strlen(buff));
}

static Value *_parse_string(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);
	return FIRST(parse_string(FIRST(args)->as.chars, "NIL"));
}

void add_core_string(Env *core) {
	ADD_FUNCTION(core, "string", _string);
	ADD_FUNCTION(core, "print", _print);
	ADD_FUNCTION(core, "println", _println);
	ADD_FUNCTION(core, "print-dbg", _print_debug);
	ADD_FUNCTION(core, "println-dbg", _println_debug);
	ADD_FUNCTION(core, "read-file", _read_file);
	ADD_FUNCTION(core, "parse-string", _parse_string);
}