#pragma once

typedef struct TextPos {
	char *fileName;
	int row;
	int col;
} TextPos;

#define TEXT_POS_NONE ((TextPos){.fileName = NULL})
#define IS_TEXT_POS_NONE(pos) ((pos).fileName == NULL)