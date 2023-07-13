CC := clang
CFLAGS := -Wno-c2x-extensions -Wall -Wextra -march=native -flto -I./include -I../astar_parallel/include -I../astar_sequential/include -I../astar_common/include -Wno-unused-parameter 
LDFLAGS := -L../astar_parallel/lib -lastar_parallel -L../astar_sequential/lib -lastar_seq  -L../astar_common/lib -lastar_common -lcheck -lm -lpthread 

ifdef DEBUG_BUILD
CFLAGS_EXTRA := -DDEBUG -g
else
CFLAGS_EXTRA := -O3
endif

ifdef STATS_GEN
CFLAGS_EXTRA += -DSTATS_GEN
endif

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SRCS))
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

TARGET := $(BIN_DIR)/maze_good

.PHONY: all clean tests

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -c $< -o $@

-include $(DEPS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -MM -MT '$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$<)' $< -MF $@

tests: $(TEST_BINS)

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: clean
