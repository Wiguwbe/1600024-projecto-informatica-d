FOLDERS := astar_common astar_sequential astar_parallel 8puzzle_gen 8puzzle numberlink maze_bad maze_good

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests
MAKE_FLAGS := 

.PHONY: all $(FOLDERS) tests run_tests clean all_with_stats measurements measurements_with_solutions solutions_only videos 

all: $(FOLDERS)

$(FOLDERS):
	@echo ${MAKE_FLAGS}
	@$(MAKE) ${MAKE_FLAGS} -C $@

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

all_with_stats:
	@$(MAKE) STATS_GEN=TRUE all

measurements: clean all
	@echo "A correr medições"
	@mkdir -p measurements
	@./run_measurements.py -d 

measurements_with_solutions: clean all
	@echo "A correr medições com geração de soluções"
	@mkdir -p measurements
	@mkdir -p reports
	@./run_measurements.py -d -i 

solutions_only: clean all
	@echo "A gerar soluções apenas"
	@mkdir -p reports
	@./run_measurements.py -d -i -c -r 5 

generate_videos: clean all_with_stats 
	@echo "A gerar videos"
	@mkdir -p reports
	@echo "A gerar video reports/maze2_bad_heuristic.mp4"
	@./generate_video.py -s 3 -o reports/maze2_bad_heuristic.mp4 maze_bad 2
	@echo "A gerar video reports/maze3_bad_heuristic.mp4"
	@./generate_video.py -s 3 -o reports/maze3_bad_heuristic.mp4 maze_bad 3
	@echo "A gerar video reports/maze4_bad_heuristic.mp4"
	@./generate_video.py -s 3 -o reports/maze4_bad_heuristic.mp4 maze_bad 4
	@echo "A gerar video reports/maze2_good_heuristic.mp4 "
	@./generate_video.py -s 3 -o reports/maze2_good_heuristic.mp4 maze_good 2
	@echo "A gerar video reports/maze3_good_heuristic.mp4 "
	@./generate_video.py -s 3 -o reports/maze3_good_heuristic.mp4 maze_good 3
	@echo "A gerar video reports/maze4_good_heuristic.mp4 "
	@./generate_video.py -s 3 -o reports/maze4_good_heuristic.mp4 maze_good 4
	@echo "A gerar video reports/search_visualization.mp4 m"
	@./generate_video.py -s 3 -o reports/search_visualization.mp4 maze_good 11
