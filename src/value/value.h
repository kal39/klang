#pragma once

#include "common.h"
#include "env/env.h"
#include "textpos.h"

typedef enum ValueType {
	VALUE_KEYWORD,
	VALUE_NIL,
	VALUE_TRUE,
	VALUE_FALSE,
	VALUE_PAIR,
	VALUE_SYMBOL,
	VALUE_NUMBER,
	VALUE_STRING,
	VALUE_FUNCTION,
	VALUE_C_FUNCTION,
	VALUE_ERROR,
} ValueType;

typedef enum KeywordType {
	KEYWORD_DEF,
	KEYWORD_LET,
	KEYWORD_DO,
	KEYWORD_IF,
	KEYWORD_FN,
	KEYWORD_EVAL,
	KEYWORD_QUOTE,
} KeywordType;

typedef struct Value Value;
typedef struct Env Env;

typedef struct Value {
	TextPos pos;
	ValueType type;
	union {
		KeywordType keyword;
		struct {
			Value *first;
			Value *rest;
		} pair;
		double number;
		char *chars;
		struct {
			Env *outer;
			Value *args;
			Value *body;
		} function;
		Value *(*cFunction)(Value *args);
	} as;
} Value;

#define IS_NIL(value) (value->type == VALUE_NIL)
#define IS_TRUE(value) (value->type == VALUE_TRUE)
#define IS_FALSE(value) (value->type == VALUE_FALSE)
#define IS_KEYWORD(value) (value->type == VALUE_KEYWORD)
#define IS_PAIR(value) (value->type == VALUE_PAIR)
#define IS_SYMBOL(value) (value->type == VALUE_SYMBOL)
#define IS_NUMBER(value) (value->type == VALUE_NUMBER)
#define IS_STRING(value) (value->type == VALUE_STRING)
#define IS_FUNCTION(value) (value->type == VALUE_FUNCTION)
#define IS_C_FUNCTION(value) (value->type == VALUE_C_FUNCTION)
#define IS_ERROR(value) (value->type == VALUE_ERROR)

#define ERROR(pos, message) value_create_chars(pos, VALUE_ERROR, message, strlen(message))

Value *value_create(TextPos pos, ValueType type);
Value *value_create_keyword(TextPos pos, KeywordType keyword);
Value *value_create_pair(TextPos pos, Value *first, Value *rest);
Value *value_create_number(TextPos pos, double number);
Value *value_create_chars(TextPos pos, ValueType type, char *string, int stringLength);
Value *value_create_function(TextPos pos, Env *outer, Value *args, Value *body);
Value *value_create_c_function(TextPos pos, Value *(*cFunction)(Value *args));

Value *value_copy(Value *value);
void value_destroy(Value *value);

int value_print(Value *value);