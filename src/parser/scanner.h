#pragma once

#include "common.h"
#include "textpos.h"

typedef struct Token {
	TextPos pos;
	char *start;
	int length;
} Token;

typedef struct Scanner {
	TextPos pos;
	char *source;
	char *currentChar;
	Token next;
} Scanner;

#define IS_END_TOKEN(token) ((token).length == -1)

Scanner *scanner_create(char *source, char *fileName);
void scanner_destroy(Scanner *scanner);

void scanner_start(Scanner *scanner);
Token scanner_peek(Scanner *scanner);
Token scanner_next(Scanner *scanner);