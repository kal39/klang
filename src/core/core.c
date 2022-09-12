#include "core.h"
#include "value/listutil.h"

void add_core_basic(Env *core);
void add_core_list(Env *core);
void add_core_string(Env *core);

Env *make_core() {
	Env *core = env_create(NULL);
	add_core_basic(core);
	add_core_list(core);
	add_core_string(core);
	return core;
}
