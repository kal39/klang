#include "listutil.h"

Value *list_create(TextPos pos) {
	return value_create(pos, VALUE_NIL);
}

void list_add(Value *list, Value *value) {
	if (IS_NIL(list)) {
		list->type = VALUE_PAIR;
		FIRST(list) = value;
		REST(list) = value_create(TEXT_POS_NONE, VALUE_NIL);
	} else if (IS_PAIR(list)) {
		if (IS_NIL(REST(list))) {
			REST(list)->type = VALUE_PAIR;
			SECOND(list) = value;
			REST(REST(list)) = value_create(TEXT_POS_NONE, VALUE_NIL);
		} else {
			list_add(REST(list), value);
		}
	}
}

int list_length(Value *list) {
	int count = 0;
	ITERATE_LIST(i, list) count++;
	return count;
}

Value *list_last(Value *list) {
	Value *last;
	for (last = list; !IS_NIL(REST(last)); last = REST(last)) {
	}
	return FIRST(last);
}