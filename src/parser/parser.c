#include "parser.h"
#include "value/listutil.h"

static bool _is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool _is_number(Token token) {
	if (!(_is_digit(token.start[0]) || (token.start[0] == '-' && _is_digit(token.start[1])))) return false;
	for (int i = 1; i < token.length; i++) {
		if (!_is_digit(token.start[i]) && token.start[i] != '.') return false;
		if (token.start[i] == '.' && !_is_digit(token.start[i + 1])) return false;
	}
	return true;
}

static bool _match_content(Token token, char *content) {
	return token.length == strlen(content) && memcmp(token.start, content, token.length) == 0;
}

static Value *_read_atom(Scanner *scanner) {
	Token token = scanner_next(scanner);
	if (_match_content(token, "nil")) return value_create(token.pos, VALUE_NIL);
	if (_match_content(token, "true")) return value_create(token.pos, VALUE_TRUE);
	if (_match_content(token, "false")) return value_create(token.pos, VALUE_FALSE);
	if (_match_content(token, "def")) return value_create_keyword(token.pos, KEYWORD_DEF);
	if (_match_content(token, "let")) return value_create_keyword(token.pos, KEYWORD_LET);
	if (_match_content(token, "do")) return value_create_keyword(token.pos, KEYWORD_DO);
	if (_match_content(token, "if")) return value_create_keyword(token.pos, KEYWORD_IF);
	if (_match_content(token, "fn")) return value_create_keyword(token.pos, KEYWORD_FN);
	if (_match_content(token, "eval")) return value_create_keyword(token.pos, KEYWORD_EVAL);
	if (_match_content(token, "quote")) return value_create_keyword(token.pos, KEYWORD_QUOTE);
	if (_is_number(token)) return value_create_number(token.pos, strtod(token.start, NULL));
	if (token.start[0] == '\"') return value_create_chars(token.pos, VALUE_STRING, token.start + 1, token.length - 2);
	else return value_create_chars(token.pos, VALUE_SYMBOL, token.start, token.length);
}

static Value *_parse(Scanner *scanner);

static Value *_read_list(Scanner *scanner) {
	Value *list = list_create(scanner_next(scanner).pos);

	for (;;) {
		if (IS_END_TOKEN(scanner_peek(scanner))) return ERROR(scanner_peek(scanner).pos, "unterminated list");
		if (scanner_peek(scanner).start[0] == ')') break;
		list_add(list, _parse(scanner));
	}

	scanner_next(scanner);
	return list;
}

static Value *_parse(Scanner *scanner) {
	return scanner_peek(scanner).start[0] == '(' ? _read_list(scanner) : _read_atom(scanner);
}

Value *parse_string(char *string, char *fileName) {
	Scanner *scanner = scanner_create(string, fileName);
	Value *asts = list_create(TEXT_POS_NONE);
	while (!IS_END_TOKEN(scanner_peek(scanner))) list_add(asts, _parse(scanner));
	scanner_destroy(scanner);
	return asts;
}

Value *parse_file(char *fileName) {
	FILE *fp = fopen(fileName, "rb");
	if (fp == NULL) return ERROR(TEXT_POS_NONE, "file not found");

	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);

	char *buff = (char *)malloc(fileSize + 1);
	fread(buff, sizeof(char), fileSize, fp);
	buff[fileSize] = '\0';

	fclose(fp);

	return parse_string(buff, fileName);
}