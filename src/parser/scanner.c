#include "scanner.h"

static Token _make_end_token() {
	return (Token){.length = -1};
}

static Token _make_token(TextPos pos, char *start, char *end) {
	int len = end - start + 1;
	return (Token){(TextPos){pos.fileName, pos.row, pos.col - len}, start, len};
}

static char *_peek(Scanner *scanner) {
	return scanner->currentChar;
}

static char *_next(Scanner *scanner) {
	scanner->pos.col++;
	return scanner->currentChar++;
}

static bool _whitespace(char c, TextPos *pos) {
	switch (c) {
		case '\n': pos->row += 1; pos->col = 1;
		case ' ':
		case '\t':
		case ',': return true;
		default: return false;
	}
}

static bool _special(char c) {
	switch (c) {
		case '(':
		case ')': return true;
		default: return false;
	}
}

static Token _scan_next_token(Scanner *scanner) {
	char *start = _next(scanner);

	// skip whitespace
	while (_whitespace(*start, &scanner->pos)) start = _next(scanner);

	// skip comments
	if (*start == ';') {
		while (*_peek(scanner) != '\n' && *_peek(scanner) != '\0') start = _next(scanner);
		start = _next(scanner);
		scanner->pos.row++;
		scanner->pos.col = 1;
	}

	if (*start == '\0') {
		return _make_end_token();

	} else if (_special(*start)) {
		return _make_token(scanner->pos, start, start);

	} else if (*start == '"') {
		char *end = start;
		while (*(end = _next(scanner)) != '"') {
			if (*end == '\0') break;		  // check for unterminated string
			if (*end == '\\') _next(scanner); // deal with escaped characters
		}
		return _make_token(scanner->pos, start, end);

	} else {
		char *end = start;
		while (!_whitespace(*_peek(scanner), &scanner->pos) && !_special(*_peek(scanner)) && *_peek(scanner) != '\0')
			end = _next(scanner);
		return _make_token(scanner->pos, start, end);
	}
}

Scanner *scanner_create(char *source, char *fileName) {
	Scanner *scanner = malloc(sizeof(Scanner));

	scanner->pos = (TextPos){fileName, 1, 1};
	scanner->source = source;
	scanner->currentChar = scanner->source;
	scanner->next = _scan_next_token(scanner);

	return scanner;
}

void scanner_destroy(Scanner *scanner) {
	free(scanner);
}

void scanner_start(Scanner *scanner) {
	scanner->pos.row = 1;
	scanner->pos.col = 1;
	scanner->currentChar = scanner->source;
	scanner->next = _scan_next_token(scanner);
}

Token scanner_peek(Scanner *scanner) {
	return scanner->next;
}

Token scanner_next(Scanner *scanner) {
	Token next = scanner->next;
	scanner->next = _scan_next_token(scanner);
	return next;
}