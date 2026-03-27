TARGET_EXEC := final_program

BUILD_DIR := build
SRC_DIR := src
THIRD_PARTY_DIR := third-party/cJSON

SRCS := $(shell find $(SRC_DIR) -name '*.c')

THIRD_PARTY_SRCS := $(THIRD_PARTY_DIR)/cJSON.c

FINAL_SRCS := $(SRCS) $(THIRD_PARTY_SRCS)

OBJS := $(FINAL_SRCS:%.c=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_FLAGS := -I$(SRC_DIR) -I$(THIRD_PARTY_DIR)

LDFLAGS := -lcurl

CC := gcc
CFLAGS := -Wall -g $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
