FOLDERS := astar_sequencial

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests

.PHONY: all clean $(FOLDERS) tests

all: $(FOLDERS)

$(FOLDERS):
	@$(MAKE) -C $@

tests:
	for dir in $(FOLDERS); do \
		$(MAKE) -C $$dir tests; \
	done

	@echo "Running tests..."
	for dir in $(FOLDERS); do \
		for test in ./$$dir/$(BIN_DIR)/test*; do \
			$$test; \
		done \
	done

clean:
	for dir in $(FOLDERS); do \
		$(MAKE) -C $$dir clean; \
	done
