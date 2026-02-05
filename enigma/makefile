CC := gcc

SRC_DIR := src
BUILD_DIR := build
TESTS_DIR := tests

CFLAGS :=
DEBUG_FLAGS := -fsanitize-address -g

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

ENIGMA_BIN := $(BUILD_DIR)/enigma

.PHONY: all debug clean encrypt decrypt

all: $(ENIGMA_BIN)
debug: $(BUILD_DIR) $(C_SOURCES)
	$(CC) $(DEBUG_FLAGS) $(C_SOURCES) -o $@


$(ENIGMA_BIN): $(BUILD_DIR) $(C_SOURCES)
	$(CC) $(CFLAGS) $(C_SOURCES) -o $@


$(BUILD_DIR):
	mkdir -p $@



# Encrypt all .txt files in tests with their corresponding .key files to .enc files
encrypt: all
	@for keyfile in $(TESTS_DIR)/*.key; do \
		if [ -f "$$keyfile" ]; then \
			prefix=$$(basename "$$keyfile" .key); \
			txtfile=$(TESTS_DIR)/$${prefix}.txt; \
			encfile=$(TESTS_DIR)/$${prefix}.enc; \
			if [ -f "$$txtfile" ]; then \
				echo "Encrypting $$txtfile -> $$encfile with key $$keyfile"; \
				$(ENIGMA_BIN) -i "$$txtfile" -o "$$encfile" -k "$$keyfile"; \
			fi; \
		fi; \
	done

# Decrypt all .enc files in tests with their corresponding .key files to .dec files
decrypt: all
	@for encfile in $(TESTS_DIR)/*.enc; do \
		if [ -f "$$encfile" ]; then \
			prefix=$$(basename "$$encfile" .enc); \
			keyfile=$(TESTS_DIR)/$${prefix}.key; \
			decfile=$(TESTS_DIR)/$${prefix}.dec; \
			if [ -f "$$keyfile" ]; then \
				echo "Decrypting $$encfile -> $$decfile with key $$keyfile"; \
				$(ENIGMA_BIN) -i "$$encfile" -o "$$decfile" -k "$$keyfile" -d; \
			fi; \
		fi; \
	done



clean:
	rm -rf ${BUILD_DIR}
	rm -f $(TESTS_DIR)/*.enc $(TESTS_DIR)/*.dec
