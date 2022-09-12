#include "core/core.h"
#include "env/env.h"
#include "eval/eval.h"
#include "parser/parser.h"
#include "value/listutil.h"

// TODO: vectors
// TODO: hashmaps
// TODO: variadic func parameters
// TODO: TCO
// TODO: argv / argc
// TODO: macros

// static void _repl() {
// 	Env *core = make_core();
// 	env_load_file(core, "data/stdlib.mal");

// 	char line[1024];
// 	for (;;) {
// 		printf("\n > ");
// 		fgets(line, sizeof(line), stdin);

// 		Value *asts = parse_string(line);

// 		ITERATE_LIST(i, asts) {
// 			Value *result = eval(core, FIRST(i));
// 			if (IS_ERROR(result)) {
// 				printf("\n");
// 				error_print(result);
// 			} else {
// 				printf("\n = ");
// 				value_print(result);
// 			}
// 			printf("\n");
// 		}
// 	}

// 	env_destroy(core);
// }

// static void _run_file(char *fileName) {
// 	char *string = read_file(fileName);
// 	Value *asts = parse_string(string);
// 	Env *core = make_core();
// 	env_load_file(core, "data/stdlib.h");

// 	ITERATE_LIST(i, asts) {
// 		Value *result = eval(core, FIRST(i));
// 		if (IS_ERROR(result)) {
// 			printf("\n");
// 			error_print(result);
// 		}
// 	}

// 	value_destroy(asts);
// 	env_destroy(core);
// 	free(string);
// }

int main(void) {
	Value *asts = parse_file("tests/scratchpad.klang");

	value_print(asts);
	printf("\n");

	Env *env = make_core();

	ITERATE_LIST(i, asts) {
		printf("> ");
		value_print(FIRST(i));
		printf("\n  ");
		value_print(eval(env, FIRST(i)));
		printf("\n");
	}

	value_destroy(asts);

	return 0;
}