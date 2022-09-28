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

#define value_create(pos, type) _value_create(pos, type, __FILE__, __LINE__)
#define value_create_keyword(pos, keyword) _value_create_keyword(pos, keyword, __FILE__, __LINE__)
#define value_create_pair(pos, first, rest) _value_create_pair(pos, first, rest, __FILE__, __LINE__)
#define value_create_number(pos, number) _value_create_number(pos, number, __FILE__, __LINE__)
#define value_create_chars(pos, type, string, stringLength)                                                            \
	_value_create_chars(pos, type, string, stringLength, __FILE__, __LINE__)
#define value_create_function(pos, outer, args, body)                                                                  \
	_value_create_function(pos, outer, args, body, __FILE__, __LINE__);
#define value_create_c_function(pos, function) _value_create_c_function(pos, function, __FILE__, __LINE__)
#define value_copy(value) _value_copy(value, __FILE__, __LINE__)
#define value_destroy(value) _value_destroy(value, __FILE__, __LINE__)

Value *_value_create(TextPos pos, ValueType type, char *file, int line);
Value *_value_create_keyword(TextPos pos, KeywordType keyword, char *file, int line);
Value *_value_create_pair(TextPos pos, Value *first, Value *rest, char *file, int line);
Value *_value_create_number(TextPos pos, double number, char *file, int line);
Value *_value_create_chars(TextPos pos, ValueType type, char *string, int stringLength, char *file, int line);
Value *_value_create_function(TextPos pos, Env *outer, Value *args, Value *body, char *file, int line);
Value *_value_create_c_function(TextPos pos, Value *(*cFunction)(Value *args), char *file, int line);

Value *_value_copy(Value *value, char *file, int line);
void _value_destroy(Value *value, char *file, int line);

int value_print(Value *value);