BIN_DIR := bin
ENIGMA_DIR := enigma


.PHONY: enigma clean

all: $(BIN_DIR) enigma
	cp $(ENIGMA_DIR)/build/enigma $<

enigma:
	make -C $(ENIGMA_DIR)



$(BIN_DIR):
	mkdir $@

clean:
	make -C $(ENIGMA_DIR) clean
