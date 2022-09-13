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

static void _repl() {
	Env *core = make_core();

	char line[1024];
	for (;;) {
		printf("\n > ");
		fgets(line, sizeof(line), stdin);

		Value *asts = parse_string(line, "stdin");

		ITERATE_LIST(i, asts) {
			Value *result = eval(core, FIRST(i));
			if (IS_ERROR(result)) {
				printf("\n");
				value_print(result);
			} else {
				printf("\n = ");
				value_print(result);
			}
			printf("\n");
		}
	}

	env_destroy(core);
}

static void _run_file(char *fileName) {
	Env *core = make_core();
	Value *asts = parse_file("tests/scratchpad.klang");

	ITERATE_LIST(i, asts) {
		Value *result = eval(core, FIRST(i));
		if (IS_ERROR(result)) {
			value_print(result);
			printf("\n");
		}
		value_destroy(result);
	}

	value_destroy(asts);
	env_destroy(core);
}

int main(int argc, char **argv) {
	if (argc == 1) _repl();
	else if (argc == 2) _run_file(argv[1]);
	else printf("ERROR: Usage: mal [filename]\n");
	return 0;
}