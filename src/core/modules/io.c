#include "../core_util.h"

static Value *_print(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 argument", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);
	printf("%s", FIRST(args)->as.chars);
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_println(Value *args) {
	_print(args);
	printf("\n");
	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_read_file(Value *args) {
	EXPECT(list_length(args) == 1, "expected 1 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);

	FILE *fp = fopen(FIRST(args)->as.chars, "r");
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

static Value *_write_file(Value *args) {
	EXPECT(list_length(args) == 2, "expected 2 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);
	EXPECT(IS_STRING(SECOND(args)), "expected string", SECOND(args)->pos);

	FILE *fp = fopen(FIRST(args)->as.chars, "w");
	if (fp == NULL) return ERROR(FIRST(args)->pos, "could not create file");
	fprintf(fp, "%s", SECOND(args)->as.chars);
	fclose(fp);

	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

static Value *_append_file(Value *args) {
	EXPECT(list_length(args) == 2, "expected 2 arguments", args->pos);
	EXPECT(IS_STRING(FIRST(args)), "expected string", FIRST(args)->pos);
	EXPECT(IS_STRING(SECOND(args)), "expected string", SECOND(args)->pos);

	FILE *fp = fopen(FIRST(args)->as.chars, "a");
	if (fp == NULL) return ERROR(FIRST(args)->pos, "could not create file");
	fprintf(fp, "%s", SECOND(args)->as.chars);
	fclose(fp);

	return value_create(TEXT_POS_NONE, VALUE_NIL);
}

void add_core_io(Env *core) {
	ADD_FUNCTION(core, "print", _print);
	ADD_FUNCTION(core, "println", _println);
	ADD_FUNCTION(core, "read-file", _read_file);
	ADD_FUNCTION(core, "write-file", _write_file);
	ADD_FUNCTION(core, "append-file", _append_file);
}