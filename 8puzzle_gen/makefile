CC := clang
CFLAGS := -Wno-c2x-extensions -Wall -Wextra -march=native -flto -I./include -Wno-unused-parameter
LDFLAGS := -lcheck -lm -lpthread 

ifdef DEBUG_BUILD
CFLAGS_EXTRA := -DDEBUG -g
else
CFLAGS_EXTRA :=
endif


SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SRCS))

TARGET := $(BIN_DIR)/8puzzle_gen

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

$(BIN_DIR)/%: $(TEST_DIR)/%.c  $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: clean
