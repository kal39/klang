#include "env/env.h"
#include "eval/eval.h"
#include "parser/parser.h"
#include "value/listutil.h"

int main(void) {
	Value *asts = parse_file("tests/scratchpad.klang");

	value_print(asts);
	printf("\n");

	Env *env = env_create(NULL);

	ITERATE_LIST(i, asts) eval(env, FIRST(i));

	value_destroy(asts);

	return 0;
}