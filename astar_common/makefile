CFLAGS := -I./include
DFLAGS := -I./include
LDFLAGS := -lcheck -lm -lpthread

ifdef DEBUG_BUILD
CFLAGS_EXTRA := -DDEBUG -g
DFLAGS_EXTRA := -fdebug
else
CFLAGS_EXTRA := -O3
endif

ifdef STATS_GEN
CFLAGS_EXTRA += -DSTATS_GEN
DFLAGS_EXTRA += -fversion=STATS_GEN
endif

include ../makefile.vars

TARGET := $(LIB_DIR)/libastar_common.a

include ../makefile.common

# tests not included in common makefile

tests: $(TARGET) $(TEST_BINS)

$(OBJ_DIR)/%: $(TARGET)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< -o $@ $(OBJS) $(TARGET) $(LDFLAGS)

$(BIN_DIR)/%: $(TEST_DIR)/%.c 
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $^ -o $@ $(LDFLAGS) -L$(LIB_DIR) -lastar_common

