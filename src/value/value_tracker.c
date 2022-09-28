#include "value_tracker.h"
#include "common.h"

typedef struct ValueTrackerEntry {
	bool freed;
	Value *ptr;
	char *allocFileName;
	int allocLine;
	char *freeFileName;
	int freeLine;
} ValueTrackerEntry;

typedef struct ValueTracker {
	int capacity;
	int size;
	ValueTrackerEntry *entries;
} ValueTracker;

static ValueTracker tracker = (ValueTracker){0, 0, NULL};

Value *_value_alloc(char *file, int line) {
	Value *value = malloc(sizeof(Value));
	// printf("+ %p at %s:%d\n", value, file, line);

	if (tracker.capacity == 0) tracker.entries = malloc(sizeof(ValueTrackerEntry) * (tracker.capacity = 8));
	if (tracker.size == tracker.capacity)
		tracker.entries = realloc(tracker.entries, sizeof(ValueTrackerEntry) * (tracker.capacity *= 2));
	tracker.entries[tracker.size++] = (ValueTrackerEntry){false, value, file, line, NULL, -1};

	return value;
}

void _value_free(Value *value, char *file, int line) {
	// printf("- %p at %s:%d\n", value, file, line);

	for (int i = 0; i < tracker.size; i++) {
		if (tracker.entries[i].ptr == value && tracker.entries[i].freed == false) {
			tracker.entries[i].freed = true;
			tracker.entries[i].freeFileName = file;
			tracker.entries[i].freeLine = line;
			free(value);
			break;
		}
	}
}

void print_value_tracker_status(bool printFree) {
	int freedEntries = 0, maxAllocLen = 9, maxFreeLen = 5;

	for (int i = 0; i < tracker.size; i++) {
		int allocLen = snprintf(NULL, 0, "%s:%d", tracker.entries[i].allocFileName, tracker.entries[i].allocLine);
		if (allocLen > maxAllocLen) maxAllocLen = allocLen;

		if (tracker.entries[i].freed == true) {
			freedEntries++;
			int freeLen = snprintf(NULL, 0, "%s:%d", tracker.entries[i].freeFileName, tracker.entries[i].freeLine);
			if (freeLen > maxFreeLen) maxFreeLen = freeLen;
		}
	}

	printf("\e[1mValue Status\e[0m\n\n");

	printf("total values    : %d\n", tracker.size);
	printf("freed values    : %d\n", freedEntries);
	printf("un-freed values : %d\n\n", tracker.size - freedEntries);

	printf(" \e[32m✓\e[0m/\e[31m✖\e[0m ┃ Address        ┃ %*s ┃ freed\n", -maxAllocLen, "allocated");
	printf("━━━━━╋━━━━━━━━━━━━━━━━╋━");
	for (int i = 0; i < maxAllocLen; i++) printf("━");
	printf("━╋━");
	for (int i = 0; i < maxFreeLen; i++) printf("━");
	printf("━\n");

	for (int i = 0; i < tracker.size; i++) {
		if (printFree || !tracker.entries[i].freed) {
			printf(tracker.entries[i].freed == true ? "\e[32m  ✓ \e[0m" : "\e[31m  ✖ \e[0m");
			printf(" ┃ %p ┃ ", tracker.entries[i].ptr);
			int len1 = printf("%s:%d", tracker.entries[i].allocFileName, tracker.entries[i].allocLine);
			for (int i = 0; i < maxAllocLen - len1; i++) printf(" ");
			printf(" ┃ ");
			if (tracker.entries[i].freed) printf("%s:%d", tracker.entries[i].freeFileName, tracker.entries[i].freeLine);
			printf("\n");
		}
	}
}