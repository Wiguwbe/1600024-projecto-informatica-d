CC := clang
AR := ar
CFLAGS := -Wno-c2x-extensions -Wall -Wextra -march=native -flto -I./include
LDFLAGS := -lcheck -lm -lpthread

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
LIB_DIR := lib
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SRCS))
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

TARGET := $(LIB_DIR)/libastar_common.a

.PHONY: all clean tests

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -c $< -o $@

-include $(DEPS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -MM -MT '$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$<)' $< > $@

tests: $(TARGET) $(TEST_BINS)

$(OBJ_DIR)/%: $(TARGET)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< -o $@ $(OBJS) $(TARGET) $(LDFLAGS)

$(BIN_DIR)/%: $(TEST_DIR)/%.c 
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $^ -o $@ $(LDFLAGS) -L$(LIB_DIR) -lastar_common

clean:
	rm -rf $(LIB_DIR) $(OBJ_DIR)  $(BIN_DIR)

