CC := gcc

SRC_DIR := src
BUILD_DIR := build

CFLAGS :=
DEBUG_FLAGS := -fsanitize-address -g

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

ENIGMA := $(BUILD_DIR)/enigma

.PHONY: all debug clean

all: $(ENIGMA)
debug: $(BUILD_DIR) $(C_SOURCES)
	$(CC) $(DEBUG_FLAGS) $(C_SOURCES) -o $@


$(ENIGMA): $(BUILD_DIR) $(C_SOURCES)
	$(CC) $(CFLAGS) $(C_SOURCES) -o $@


$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf ${BUILD_DIR}
