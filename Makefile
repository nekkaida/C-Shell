# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c11
LDFLAGS :=

# Build type flags
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2 -DNDEBUG

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Files
TARGET := shell
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default build type
BUILD_TYPE ?= release

# Set flags based on build type
ifeq ($(BUILD_TYPE),debug)
    CFLAGS += $(DEBUG_FLAGS)
    BUILD_DIR := $(BUILD_DIR)/debug
    BIN_DIR := $(BIN_DIR)/debug
else
    CFLAGS += $(RELEASE_FLAGS)
    BUILD_DIR := $(BUILD_DIR)/release
    BIN_DIR := $(BIN_DIR)/release
endif

# Final target path
TARGET_EXEC := $(BIN_DIR)/$(TARGET)

# Phony targets
.PHONY: all clean debug release dirs

# Default target
all: release

# Debug build
debug:
	$(MAKE) BUILD_TYPE=debug dirs $(TARGET_EXEC)

# Release build
release:
	$(MAKE) BUILD_TYPE=release dirs $(TARGET_EXEC)

# Create necessary directories
dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

# Link the executable
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf build bin

# Run the shell (convenience target)
.PHONY: run
run: release
	$(TARGET_EXEC)

# Install the shell (optional)
.PHONY: install
install: release
	cp $(TARGET_EXEC) /usr/local/bin/$(TARGET)

# Generate dependencies
DEPS := $(OBJS:.o=.d)
-include $(DEPS)