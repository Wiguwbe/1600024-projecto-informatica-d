CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -I./include -O3
LDFLAGS := -lcheck -lm -lpthread

SRC_DIR := src
OBJ_DIR := obj
LIB_DIR := lib
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SRCS))
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%,$(TEST_SRCS))

TARGET := $(LIB_DIR)/libastar_seq.a

.PHONY: all clean tests

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MM -MT '$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$<)' $< > $@

tests: $(TEST_BINS)

$(OBJ_DIR)/%: $(TEST_DIR)/%.c $(TARGET)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(OBJS) $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(LIB_DIR) $(OBJ_DIR)

