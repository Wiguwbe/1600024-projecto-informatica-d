FOLDERS := astar_common astar_sequential astar_parallel 8puzzle_gen 8puzzle numberlink maze

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests
MAKE_FLAGS := 

.PHONY: all all_with_stats $(FOLDERS) tests run_tests clean generate_measurements generate_solutions generate_videos generate_report generate_mazes

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

generate_measurements: clean all
	@echo "A correr medições 8 puzzle"
	@mkdir -p report/measurements
	@rm report/measurements/*.csv
	@./run_measurement.py -d -c -n -o report/measurements/8puzzle.csv 8puzzle easy_1 
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle easy_2
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle easy_3 
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle hard_1 
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle hard_2
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle impossible_1 
	@./run_measurement.py -d -c -o report/measurements/8puzzle.csv 8puzzle impossible_2 
	@echo "A correr medições numberlink"
	@./run_measurement.py -d -c -n -o report/measurements/numberlink.csv numberlink 1
	@./run_measurement.py -d -c -o report/measurements/numberlink.csv numberlink 2
	@./run_measurement.py -d -c -o report/measurements/numberlink.csv numberlink 3
	@./run_measurement.py -d -c -o report/measurements/numberlink.csv numberlink 4
	@./run_measurement.py -d -c -o report/measurements/numberlink.csv numberlink 5
	@echo "A correr medições maze"
	@./run_measurement.py -d -c -n -o report/measurements/maze.csv maze 1
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 2
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 3
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 4
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 5
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 6
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 7
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 8
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 9
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 10
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 11
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 12
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 13
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 14
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 15
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 16
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 17
	@./run_measurement.py -d -c -o report/measurements/maze.csv maze 18
	@./run_measurement.py -d -c -o -r 1 report/measurements/maze.csv maze 19
	@./run_measurement.py -d -c -o -r 1 report/measurements/maze.csv maze 20
	@./run_measurement.py -d -c -o -r 1 report/measurements/maze.csv maze 21
	@./run_measurement.py -d -c -o -r 1 report/measurements/maze.csv maze 22
	@./run_measurement.py -d -c -o -r 1 report/measurements/maze.csv maze 23

generate_solutions: clean all
	@echo "A gerar imagens de soluções"
	@mkdir -p report/solutions
	@rm report/measurements/*.png
	@./generate_solution.py -d -r 5 8puzzle easy_1 
	@./generate_solution.py -d -r 5 8puzzle easy_2
	@./generate_solution.py -d -r 5 8puzzle easy_3 
	@./generate_solution.py -d -r 5 8puzzle hard_1 
	@./generate_solution.py -d -r 5 8puzzle hard_2
	@./generate_solution.py -d -r 5 8puzzle impossible_1 
	@./generate_solution.py -d -r 5 8puzzle impossible_2 
	@echo "A correr medições numberlink"
	@./generate_solution.py -d -r 5 numberlink 1
	@./generate_solution.py -d -r 5 numberlink 2
	@./generate_solution.py -d -r 5 numberlink 3
	@./generate_solution.py -d -r 5 numberlink 4
	@./generate_solution.py -d -r 5 numberlink 5
	@echo "A correr medições maze"
	@./generate_solution.py -d -r 5 maze 1
	@./generate_solution.py -d -r 5 maze 2
	@./generate_solution.py -d -r 5 maze 3
	@./generate_solution.py -d -r 5 maze 4
	@./generate_solution.py -d -r 5 maze 5
	@./generate_solution.py -d -r 5 maze 6
	@./generate_solution.py -d -r 5 maze 7
	@./generate_solution.py -d -r 5 maze 8
	@./generate_solution.py -d -r 5 maze 9

generate_videos: clean all_with_stats 
	@echo "A gerar videos"
	@mkdir -p report/videos
	@rm report/measurements/*.mp4
	@./generate_video.py -s 2 -d -o report/videos/maze_4.mp4 maze 4
	@./generate_video.py -s 10 -d -o report/videos/maze_5.mp4 maze 5
	@./generate_video.py -s 10 -d -o report/videos/maze_7.mp4 maze 7
	@./generate_video.py -s 100 -d -o report/videos/maze_11.mp4 maze 11
	@./generate_video.py -s 200 -d -o report/videos/maze_12.mp4 maze 12
	@./generate_video.py -s 400 -d -o report/videos/maze_13.mp4 maze 13

generate_report: generate_measurements generate_solutions generate_videos

generate_mazes:
	@echo "A gerar instancias de labirintos"
	@echo "A gerar labirinto 10 x 10"
	python3 maze_gen/maze_gen.py -a 10 -l 10 -o instances/maze_1
	@echo "A gerar labirinto 20 x 20"
	python3 maze_gen/maze_gen.py -a 20 -l 20 -o instances/maze_2
	@echo "A gerar labirinto 30 x 30"
	python3 maze_gen/maze_gen.py -a 30 -l 30 -o instances/maze_3
	@echo "A gerar labirinto 40 x 40"
	python3 maze_gen/maze_gen.py -a 40 -l 40 -o instances/maze_4
	@echo "A gerar labirinto 50 x 50"
	python3 maze_gen/maze_gen.py -a 50 -l 50 -o instances/maze_5
	@echo "A gerar labirinto 50 x 50"
	python3 maze_gen/maze_gen.py -a 50 -l 50 -o instances/maze_6
	@echo "A gerar labirinto 80 x 80"
	python3 maze_gen/maze_gen.py -a 80 -l 80 -o instances/maze_7
	@echo "A gerar labirinto 90 x 90"
	python3 maze_gen/maze_gen.py -a 90 -l 90 -o instances/maze_8
	@echo "A gerar labirinto 100 x 100"
	python3 maze_gen/maze_gen.py -a 100 -l 100 -o instances/maze_9
	@echo "A gerar labirinto 200 x 200"
	python3 maze_gen/maze_gen.py -a 200 -l 200 -o instances/maze_10
	@echo "A gerar labirinto 300 x 300"
	python3 maze_gen/maze_gen.py -a 300 -l 300 -o instances/maze_11
	@echo "A gerar labirinto 400 x 400"
	python3 maze_gen/maze_gen.py -a 400 -l 400 -o instances/maze_12
	@echo "A gerar labirinto 500 x 500"
	python3 maze_gen/maze_gen.py -a 500 -l 500 -o instances/maze_13
	@echo "A gerar labirinto 600 x 600"
	python3 maze_gen/maze_gen.py -a 600 -l 600 -o instances/maze_14
	@echo "A gerar labirinto 700 x 700"
	python3 maze_gen/maze_gen.py -a 700 -l 700 -o instances/maze_15
	@echo "A gerar labirinto 800 x 800"
	python3 maze_gen/maze_gen.py -a 800 -l 800 -o instances/maze_16
	@echo "A gerar labirinto 900 x 900"
	python3 maze_gen/maze_gen.py -a 900 -l 900 -o instances/maze_17
	@echo "A gerar labirinto 1000 x 1000"
	python3 maze_gen/maze_gen.py -a 1000 -l 1000 -o instances/maze_18
	@echo "A gerar labirinto 2000 x 2000"
	python3 maze_gen/maze_gen.py -a 2000 -l 2000 -o instances/maze_19
	@echo "A gerar labirinto 3000 x 3000"
	python3 maze_gen/maze_gen.py -a 3000 -l 3000 -o instances/maze_20
	@echo "A gerar labirinto 4000 x 4000"
	python3 maze_gen/maze_gen.py -a 4000 -l 4000 -o instances/maze_21
	@echo "A gerar labirinto 5000 x 5000"
	python3 maze_gen/maze_gen.py -a 5000 -l 5000 -o instances/maze_22
	@echo "A gerar labirinto 10000 x 10000"
	python3 maze_gen/maze_gen.py -a 10000 -l 10000 -o instances/maze_23
