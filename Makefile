COMPILER := avr-gcc
FLAGS := -W -Wall -Wextra -Werror -pedantic -std=c23 -mmcu=atmega169
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

SRC_DIR := ./src
TARGET_DIR := ./target
OBJ_DIR := $(TARGET_DIR)/obj
BINARY := $(TARGET_DIR)/main

# TODO: make `PART` not required for `clean`.
ifeq ($(PART),)
$(error PART is not set)
endif

COMMON_SOURCES := $(filter-out $(SRC_DIR)/part%.c, $(wildcard $(SRC_DIR)/*.c))
PART_SRC := $(SRC_DIR)/part$(PART).c
SOURCES := $(COMMON_SOURCES) $(PART_SRC)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: all
all: debug

.PHONY: debug
debug: FLAGS += $(DEBUG_FLAGS)
debug: $(BINARY)

.PHONY: release
release: FLAGS += $(RELEASE_FLAGS)
release: $(BINARY)

$(BINARY): $(OBJECTS) | $(TARGET_DIR)
	$(COMPILER) $(FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -c $c $< -o $@

$(TARGET_DIR) $(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(TARGET_DIR)
