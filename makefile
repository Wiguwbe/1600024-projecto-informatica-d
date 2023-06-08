FOLDERS := astar_common astar_sequential astar_parallel 8puzzle_gen 8puzzle_seq 8puzzle_par numberlink_seq numberlink_par

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

run_tests: tests
	@echo "Running tests..."
	for dir in $(FOLDERS); do \
		if [ -d ./$$dir/tests ]; then \
			for test in ./$$dir/$(BIN_DIR)/test*; do \
				$$test; \
			done \
		fi \
	done

clean:
	for dir in $(FOLDERS); do \
		$(MAKE) -C $$dir clean; \
	done
