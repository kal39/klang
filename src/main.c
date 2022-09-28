#include <readline/history.h>
#include <readline/readline.h>

#define K_MEM_TRACKER_IMPLEMENTATION
#include "core/core.h"
#include "env/env.h"
#include "eval/eval.h"
#include "parser/parser.h"
#include "value/listutil.h"
#include "value/value_tracker.h"

// TODO: vectors
// TODO: hashmaps
// TODO: variadic func parameters
// TODO: TCO
// TODO: argv / argc
// TODO: macros
// TODO: Value allocation manager

static void _repl() {
	Env *core = make_core();
	rl_bind_key('\t', rl_insert);

	for (;;) {
		char *line = readline(" > ");
		add_history(line);

		char *fileName = "stdin";
		Value *asts = parse_string(line, fileName);

		ITERATE_LIST(i, asts) {
			Value *result = eval(core, FIRST(i));
			if (IS_ERROR(result)) {
				printf("\n");
				value_print(result);
			} else {
				printf("\n = ");
				value_print(result);
			}
			printf("\n\n");
		}

		free(line);
	}

	env_destroy(core);
}

static void _run_file(char *fileName) {
	Env *core = make_core();
	Value *asts = parse_file(fileName);

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

	print_value_tracker_status(false);
	// k_mem_track_print(true);
	return 0;
}