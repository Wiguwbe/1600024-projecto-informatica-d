FOLDERS := astar_common astar_sequential astar_parallel 8puzzle_gen 8puzzle numberlink maze

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests

ifdef NUM_WORKERS
WORKERS := ${NUM_WORKERS}
else
WORKERS := 6
endif


.PHONY: all clean $(FOLDERS) tests measurements

all: $(FOLDERS)

$(FOLDERS):
	@$(MAKE) -C $@

tests:
	for dir in $(FOLDERS); do \
		$(MAKE) -C $$dir tests; \
	done

run_tests: tests
	@echo "Running tests..."
	@for dir in $(FOLDERS); do \
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

measurements: 
	@rm measurements/*.csv
	@./run_measurements.py -d 

generate_solutions: 
	@rm reports/*.png
	@./generate_solutions.py -r 5 -i -d
