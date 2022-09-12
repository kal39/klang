#pragma once

#include "value.h"

#define REST(list) (list->as.pair.rest)
#define FIRST(list) (list->as.pair.first)
#define SECOND(list) FIRST(REST(list))
#define THIRD(list) FIRST(REST(REST(list)))
#define FOURTH(list) FIRST(REST(REST(REST(list))))
#define LAST(list) list_last(list)

#define ITERATE_LIST(iterator, list) for (Value *iterator = (list); !IS_NIL(iterator); iterator = REST(iterator))

Value *list_create(TextPos pos);
void list_add(Value *list, Value *value);
int list_length(Value *list);
Value *list_last(Value *list);