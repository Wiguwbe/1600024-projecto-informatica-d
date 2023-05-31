CC := gcc
CFLAGS := -Wall -Wextra -I./include -I../astar_sequencial/include -Wno-unused-parameter -O3
LDFLAGS := -L../astar_sequencial/lib -lastar_seq -L../astar_common/lib -lastar_common -lcheck -lm -lpthread 

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SRCS))
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

TARGET := $(BIN_DIR)/8puzzle_seq

.PHONY: all clean tests

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MM -MT '$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$<)' $< -MF $@

tests: $(TEST_BINS)

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: clean
