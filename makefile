#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

EXECUTABLE     := klang
LIBS           := -lreadline
FLAGS          := -Wall -Wno-missing-braces -g
DEFS           := -DENABLE_VALUE_TRACKER #-DPRINT_EVALUATION_STEPS

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := lib
BUILD_FOLDER   := build
SRC_FOLDER     := src

#======================================================================================================================#

CC            := gcc $(FLAGS) $(DEFS) -I $(INCLUDE_FOLDER) -I $(SRC_FOLDER) -L $(LIB_FOLDER)
MV            := mv
RM            := rm -rf
CP            := cp
MKDIR         := mkdir -p
SRC_FOLDERS   := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES       := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
O_FILES       := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))

define \n


endef

.PHONY: default run clean

default: clean $(EXECUTABLE)

run: clean $(EXECUTABLE)
	./$(EXECUTABLE)

$(BUILD_FOLDER):
	$(MKDIR) $(BUILD_FOLDERS)

$(EXECUTABLE): $(BUILD_FOLDER)
	$(foreach FILE, $(C_FILES), $(CC) -c $(FILE) -o $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(FILE))) $(\n))
	$(CC) $(O_FILES) -o $(EXECUTABLE) $(LIBS)

clean:
	$(RM) $(EXECUTABLE) $(BUILD_FOLDER)
