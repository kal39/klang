#include "eval.h"
#include "common.h"
#include "value/listutil.h"
#include "value/value.h"

#define RETURN_IF_ERROR(value)                                                                                         \
	if (IS_ERROR(value)) return value
#define EVAL(env, ast) _eval(env, ast, depth + 1, line)
#define EXPECT(condition, message, pos)                                                                                \
	if (!(condition)) {                                                                                                \
		result = ERROR(pos, message);                                                                                  \
		break;                                                                                                         \
	}

static Value *_eval(Env *env, Value *ast, int depth, int *line) {
#ifdef PRINT_EVALUATION_STEPS
	*line = *line + 1;
	int startLine = *line;
	printf("\n");
	for (int i = 0; i < depth; i++) printf(" ║ ");
	value_print(ast);
#endif

	Value *result = value_copy(ast);

	switch (ast->type) {
		case VALUE_PAIR: {
			switch (FIRST(ast)->type) {
				case VALUE_PAIR:
				case VALUE_SYMBOL: {
					Value *tmpAst = value_create_pair(FIRST(ast)->pos, EVAL(env, FIRST(ast)), REST(ast));
					RETURN_IF_ERROR(FIRST(tmpAst));
					result = EVAL(env, tmpAst);
					value_destroy(tmpAst);
					RETURN_IF_ERROR(result);
					break;
				}

				case VALUE_KEYWORD: {
					switch (FIRST(ast)->as.keyword) {
						case KEYWORD_DEF: {
							EXPECT(list_length(REST(ast)) % 2 == 0, "expected even number arguments", REST(ast)->pos);

							for (Value *i = REST(ast); !IS_NIL(i); i = REST(REST(i))) {
								Value *key = FIRST(i);
								result = EVAL(env, SECOND(i));
								RETURN_IF_ERROR(result);
								env_set(env, key, result);
							}
							break;
						}

						case KEYWORD_LET: {
							EXPECT(list_length(REST(ast)) == 2, "expected 2+ arguments", REST(ast)->pos);
							EXPECT(list_length(SECOND(ast)) % 2 == 0, "expected even number arguments", REST(ast)->pos);

							Env *newEnv = env_create(env);
							for (Value *i = SECOND(ast); !IS_NIL(i); i = REST(REST(i))) {
								Value *key = FIRST(i);
								Value *value = EVAL(env, SECOND(i));
								RETURN_IF_ERROR(value);
								env_set(newEnv, key, value);
								value_destroy(value);
							}

							result = EVAL(newEnv, THIRD(ast));
							RETURN_IF_ERROR(result);
							env_destroy(newEnv);
							break;
						}

						case KEYWORD_DO: {
							Value *results = list_create(ast->pos);
							ITERATE_LIST(i, REST(ast)) {
								Value *value = EVAL(env, FIRST(i));
								RETURN_IF_ERROR(value);
								list_add(results, value);
							}
							result = LAST(results);
							break;
						}

						case KEYWORD_IF: {
							EXPECT(list_length(REST(ast)) == 3, "expected 3 arguments", REST(ast)->pos);
							Value *condition = EVAL(env, SECOND(ast));
							RETURN_IF_ERROR(condition);
							result = EVAL(env, IS_FALSE(condition) ? FOURTH(ast) : THIRD(ast));
							RETURN_IF_ERROR(result);
							value_destroy(condition);
							break;
						}

						case KEYWORD_FN: {
							EXPECT(list_length(REST(ast)) == 2, "expected 2 arguments", REST(ast)->pos);
							result = value_create_function(ast->pos, env_create(env), SECOND(ast), THIRD(ast));
							break;
						}

						case KEYWORD_EVAL: {
							EXPECT(list_length(REST(ast)) == 1, "expected 1 arguments", REST(ast)->pos);
							Value *value = EVAL(env, FIRST(REST(ast)));
							RETURN_IF_ERROR(value);
							result = EVAL(env, value);
							RETURN_IF_ERROR(result);
							value_destroy(value);
							break;
						}

						case KEYWORD_QUOTE: {
							EXPECT(list_length(REST(ast)) == 1, "expected 1 arguments", REST(ast)->pos);
							result = FIRST(REST(ast));
							break;
						}
					}
					break;
				}

				case VALUE_FUNCTION: {
					Value *function = FIRST(ast);
					Value *argNames = function->as.function.args;

					Value *argValues = list_create(REST(ast)->pos);
					ITERATE_LIST(i, REST(ast)) {
						Value *arg = EVAL(env, FIRST(i));
						RETURN_IF_ERROR(arg);
						list_add(argValues, arg);
					}

					int argNamesLen = list_length(argNames);
					int argValuesLen = list_length(argValues);

					EXPECT(argNamesLen == argValuesLen, "incorrect argument count", argValues->pos);
					Env *funEnv = env_create(function->as.function.outer);

					for (int i = 0; i < list_length(argValues) + 1; i++) {
						env_set(funEnv, FIRST(argNames), FIRST(argValues));
						argNames = REST(argNames);
						argValues = REST(argValues);
					}

					result = EVAL(funEnv, function->as.function.body);
					RETURN_IF_ERROR(result);

					value_destroy(argValues);
					break;
				}

				case VALUE_C_FUNCTION: {
					Value *argValues = list_create(REST(ast)->pos);
					ITERATE_LIST(i, REST(ast)) {
						Value *arg = EVAL(env, FIRST(i));
						RETURN_IF_ERROR(arg);
						list_add(argValues, arg);
					}

					result = FIRST(ast)->as.cFunction(argValues);
					if (IS_TEXT_POS_NONE(result->pos)) result->pos = ast->pos;
					RETURN_IF_ERROR(result);

					value_destroy(argValues);
					break;
				}
				case VALUE_NIL:
				case VALUE_TRUE:
				case VALUE_FALSE:
				case VALUE_NUMBER:
				case VALUE_STRING:
				case VALUE_ERROR: result = ERROR(FIRST(ast)->pos, "expected function"); break;
			}
			break;
		}
		case VALUE_SYMBOL: {
			result = env_get(env, ast);
			if (IS_ERROR(result)) {
				result->pos = ast->pos;
				return result;
			}
			break;
		}
		case VALUE_KEYWORD:
		case VALUE_NIL:
		case VALUE_TRUE:
		case VALUE_FALSE:
		case VALUE_NUMBER:
		case VALUE_STRING:
		case VALUE_FUNCTION:
		case VALUE_C_FUNCTION:
		case VALUE_ERROR: break;
	}

#ifdef PRINT_EVALUATION_STEPS
	if (*line == startLine) {
		printf(" ═ ");
		value_print(result);
	} else {
		*line = *line + 1;
		printf("\n");
		for (int i = 0; i < depth; i++) printf(" ║ ");
		printf(" ╚═ ");
		value_print(result);
	}
#endif

	return result;
}

Value *eval(Env *env, Value *ast) {
	int line = 0;
	Value *result = _eval(env, ast, 0, &line);
#ifdef PRINT_EVALUATION_STEPS
	printf("\n\n");
	env_print(env);
#endif
	return result;
}
