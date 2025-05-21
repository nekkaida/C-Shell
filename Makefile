# C Shell Makefile
# Can be used as an alternative to CMake

CC ?= gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
LDFLAGS = -lm
DEBUGFLAGS = -g -O0 -DDEBUG
RELEASEFLAGS = -O2 -DNDEBUG

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
BUILD_DIR = build
TEST_DIR = tests

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/*/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Make sure build directory structure exists
BUILD_SUBDIRS = $(sort $(dir $(OBJS)))

# Binary name
TARGET = $(BIN_DIR)/shell

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -D_POSIX_C_SOURCE=200809L
endif

# Default target
all: release

# Release build
release: CFLAGS += $(RELEASEFLAGS)
release: prepare $(TARGET)

# Debug build
debug: CFLAGS += $(DEBUGFLAGS)
debug: prepare $(TARGET)

# Create necessary directories
prepare:
	@mkdir -p $(BIN_DIR) $(BUILD_SUBDIRS)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Run the shell
run: $(TARGET)
	$(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Clean and rebuild
rebuild: clean all

# Run tests
test: debug
	cd $(TEST_DIR) && ./run_tests.sh

# Install
install: release
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/

# Uninstall
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/shell

# Static analysis
check:
	cppcheck --enable=warning,performance,portability --suppress=missingIncludeSystem $(SRC_DIR)

.PHONY: all release debug prepare run clean rebuild test install uninstall check