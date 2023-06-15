FOLDERS := astar_common astar_sequential astar_parallel 8puzzle_gen 8puzzle numberlink

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

measurements: measurements_numberlink measurements_8puzzle

reports: reports_numberlink reports_8puzzle

measurements_numberlink: all
	@echo "A criar medições para o problema numberlink"
	@mkdir -p measurements
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/numberlink.sequencial.csv
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/numberlink.parallel.best.$(WORKERS)_workers.csv
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/numberlink.parallel.first.$(WORKERS)_workers.csv
	@for TEST in ./instances/numberlink_*; do \
		echo "Problema: numberlink, Instância: $$TEST"; \
		echo "Algoritmo sequencial"; \
		./numberlink/$(BIN_DIR)/numberlink -r $$TEST >> measurements/numberlink.sequencial.csv ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, melhor solução"; \
		./numberlink/$(BIN_DIR)/numberlink -n $(WORKERS) -r $$TEST >> measurements/numberlink.parallel.best.$(WORKERS)_workers.csv ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, primeira solução"; \
		./numberlink/$(BIN_DIR)/numberlink -n $(WORKERS) -p -r $$TEST >> measurements/numberlink.parallel.first.$(WORKERS)_workers.csv ; sleep 2;\
	done

measurements_8puzzle: all
	@echo "A criar medições para o problema 8 puzzle"
	@mkdir -p measurements
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/8puzzle.sequencial.csv
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/8puzzle.parallel.best.$(WORKERS)_workers.csv
	@echo "\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"" > measurements/8puzzle.parallel.first.$(WORKERS)_workers.csv
	@for TEST in ./instances/8puzzle_*; do \
		echo "Problema: 8puzzle, Instância: $$TEST"; \
		echo "Algoritmo sequencial"; \
		./8puzzle/$(BIN_DIR)/8puzzle -r $$TEST >> measurements/8puzzle.sequencial.csv ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, melhor solução"; \
		./8puzzle/$(BIN_DIR)/8puzzle -n $(WORKERS) -r $$TEST >> measurements/8puzzle.parallel.best.$(WORKERS)_workers.csv ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, primeira solução"; \
		./8puzzle/$(BIN_DIR)/8puzzle -n $(WORKERS) -p -r $$TEST >> measurements/8puzzle.parallel.first.$(WORKERS)_workers.csv ; sleep 2;\
	done; \


reports_numberlink: 
	@echo "A criar relatórios problema numberlink"
	@mkdir -p reports
	@echo > reports/numberlink.sequencial.txt
	@echo > reports/numberlink.parallel.best.$(WORKERS)_workers.txt
	@echo > reports/numberlink.parallel.first.$(WORKERS)_workers.txt
	@for TEST in ./instances/numberlink_*; do \
		echo "Problema: numberlink, Instância: $$TEST"; \
		echo "Algoritmo sequencial"; \
		./numberlink/$(BIN_DIR)/numberlink $$TEST >> reports/numberlink.sequencial.txt ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, melhor solução"; \
		./numberlink/$(BIN_DIR)/numberlink -n $(WORKERS) $$TEST >> reports/numberlink.parallel.best.$(WORKERS)_workers.txt ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, primeira solução"; \
		./numberlink/$(BIN_DIR)/numberlink -n $(WORKERS) -p $$TEST >> reports/numberlink.parallel.first.$(WORKERS)_workers.txt ; sleep 2;\
	done

reports_8puzzle: 
	@echo "A criar relatórios problema 8 puzzle"
	@mkdir -p reports
	@echo > reports/8puzzle.sequencial.txt
	@echo > reports/8puzzle.parallel.best.$(WORKERS)_workers.txt
	@echo > reports/8puzzle.parallel.first.$(WORKERS)_workers.txt
	@for TEST in ./instances/8puzzle_*; do \
		echo "Problema: 8puzzle, Instância: $$TEST"; \
		echo "Algoritmo sequencial"; \
		./8puzzle/$(BIN_DIR)/8puzzle $$TEST >> reports/8puzzle.sequencial.txt ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, melhor solução"; \
		./8puzzle/$(BIN_DIR)/8puzzle -n $(WORKERS) $$TEST >> reports/8puzzle.parallel.best.$(WORKERS)_workers.txt ; sleep 2; \
		echo "Algoritmo paralelo - $(WORKERS) trabalhadores, primeira solução"; \
		./8puzzle/$(BIN_DIR)/8puzzle -n $(WORKERS) -p $$TEST >> reports/8puzzle.parallel.first.$(WORKERS)_workers.txt ; sleep 2;\
	done