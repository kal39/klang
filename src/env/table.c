#include "table.h"

#define TABLE_MAX_LOAD 0.5
#define STRING_EQUALS(a, b) (strlen(a) == strlen(b) && memcmp((a), (b), strlen(a)) == 0)

static unsigned int _hash(char *string) {
	int length = strlen(string);

	unsigned int hash = 2166136261;
	for (int i = 0; i < length; i++) {
		hash ^= string[i];
		hash *= 16777619;
	}

	return hash;
}

static Entry *_find(Table *table, char *key) {
	for (unsigned int i = _hash(key) % table->capacity;; i = (i + 1) % table->capacity) {
		Entry *entry = &table->entries[i];
		if (entry->key == NULL || STRING_EQUALS(entry->key, key)) return entry;
	}
}

static void _resize(Table *table, int newCapacity) {
	Entry *oldEntries = table->entries;
	int oldCapacity = table->capacity;

	table->capacity = newCapacity;
	table->entries = malloc(sizeof(Entry) * table->capacity);
	for (int i = 0; i < table->capacity; i++) table->entries[i].key = NULL;

	for (int i = 0; i < oldCapacity; i++) {
		Entry *entry = &oldEntries[i];
		if (entry->key != NULL) {
			Entry *dest = _find(table, entry->key);
			dest->key = entry->key;
			dest->value = entry->value;
		}
	}

	if (oldEntries != NULL) free(oldEntries);
}

static Entry _entry_create(char *key, Value *value) {
	Entry entry;
	int keyLen = strlen(key) + 1;
	entry.key = malloc(keyLen);
	memcpy(entry.key, key, keyLen);
	entry.value = value_copy(value);
	return entry;
}

static void _entry_destroy(Entry entry) {
	free(entry.key);
	value_destroy(entry.value);
}

Table *table_create() {
	Table *table = malloc(sizeof(Table));
	table->capacity = 0;
	table->size = 0;
	table->entries = NULL;

	_resize(table, 8);
	return table;
}

void table_destroy(Table *table) {
	for (int i = 0; i < table->capacity; i++) {
		Entry *entry = &table->entries[i];
		if (entry->key != NULL) _entry_destroy(*entry);
	}

	free(table->entries);
	free(table);
}

void table_set(Table *table, char *key, Value *value) {
	if (table->size == table->capacity * TABLE_MAX_LOAD) _resize(table, table->capacity * 2);

	Entry *entry = _find(table, key);
	if (entry->key == NULL) table->size++;

	*entry = _entry_create(key, value);
}

Value *table_get(Table *table, char *key) {
	Entry *entry = _find(table, key);
	return entry->key == NULL ? NULL : entry->value;
}

void table_print(Table *table) {
	int maxLen = 3;
	for (int i = 0; i < table->capacity; i++) {
		Entry *entry = &table->entries[i];
		if (entry->key != NULL) {
			int len = strlen(entry->key);
			if (len > maxLen) maxLen = len;
		}
	}

	printf("??? key");
	for (int j = 0; j < maxLen - 3; j++) printf(" ");
	printf(" ??? value\n??????");
	for (int j = 0; j < maxLen; j++) printf("???");
	printf("?????????");
	for (int j = 0; j < maxLen; j++) printf("???");
	printf("\n");

	for (int i = 0; i < table->capacity; i++) {
		Entry *entry = &table->entries[i];
		if (entry->key != NULL) {
			int len = strlen(entry->key);
			printf("??? %s", entry->key);
			for (int j = 0; j < maxLen - len; j++) printf(" ");
			printf(" ??? ");
			value_print(entry->value);
			printf("\n");
		}
	}
}