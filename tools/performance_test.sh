#!/bin/bash
# Performance testing script for C Shell

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Path to the shell executable
SHELL_PATH="../bin/shell"
RESULTS_DIR="performance_results"
mkdir -p "$RESULTS_DIR"

# Verify the shell exists
if [ ! -f "$SHELL_PATH" ]; then
    echo -e "${RED}Error: Shell executable not found at $SHELL_PATH${NC}"
    echo "Make sure you've built the project and are running this script from the tools directory."
    exit 1
fi

# Test parameters
NUM_COMMANDS=1000
COMMAND_FILE="$RESULTS_DIR/commands.txt"

# Generate test commands
generate_commands() {
    echo -e "${BLUE}Generating $NUM_COMMANDS test commands...${NC}"
    
    rm -f "$COMMAND_FILE"
    
    # Mix of different command types
    for i in $(seq 1 $NUM_COMMANDS); do
        case $((i % 10)) in
            0) echo "echo Test command $i" >> "$COMMAND_FILE" ;;
            1) echo "pwd" >> "$COMMAND_FILE" ;;
            2) echo "cd /tmp" >> "$COMMAND_FILE" ;;
            3) echo "cd /" >> "$COMMAND_FILE" ;;
            4) echo "cd .." >> "$COMMAND_FILE" ;;
            5) echo "echo 'String with spaces'" >> "$COMMAND_FILE" ;;
            6) echo "echo \"Double quoted string\"" >> "$COMMAND_FILE" ;;
            7) echo "type echo" >> "$COMMAND_FILE" ;;
            8) echo "echo test > /dev/null" >> "$COMMAND_FILE" ;;
            9) echo "ls -l /proc" >> "$COMMAND_FILE" ;;
        esac
    done
    
    # Add exit at the end
    echo "exit" >> "$COMMAND_FILE"
    
    echo -e "${GREEN}Generated $NUM_COMMANDS commands.${NC}"
}

# Run performance test
run_performance_test() {
    echo -e "${BLUE}Running performance test with $NUM_COMMANDS commands...${NC}"
    
    # Time the execution
    START_TIME=$(date +%s.%N)
    
    cat "$COMMAND_FILE" | "$SHELL_PATH" > /dev/null 2>&1
    
    END_TIME=$(date +%s.%N)
    ELAPSED=$(echo "$END_TIME - $START_TIME" | bc)
    COMMANDS_PER_SEC=$(echo "$NUM_COMMANDS / $ELAPSED" | bc)
    
    echo -e "${GREEN}Test completed:${NC}"
    echo "Total time: $ELAPSED seconds"
    echo "Commands per second: $COMMANDS_PER_SEC"
    
    # Save results
    echo "$(date) - $NUM_COMMANDS commands in $ELAPSED seconds ($COMMANDS_PER_SEC commands/sec)" >> "$RESULTS_DIR/history.txt"
}

# Memory usage test
test_memory_usage() {
    echo -e "${BLUE}Testing memory usage...${NC}"
    
    if ! command -v valgrind &> /dev/null; then
        echo -e "${YELLOW}Valgrind not found, skipping memory test.${NC}"
        return
    fi
    
    # Create a small set of commands for memory testing
    MEMORY_TEST_FILE="$RESULTS_DIR/memory_test.txt"
    echo "pwd" > "$MEMORY_TEST_FILE"
    echo "cd /tmp" >> "$MEMORY_TEST_FILE"
    echo "echo memory test" >> "$MEMORY_TEST_FILE"
    echo "exit" >> "$MEMORY_TEST_FILE"
    
    # Run with valgrind
    echo -e "${YELLOW}Running with Valgrind (this may take a while)...${NC}"
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
             --log-file="$RESULTS_DIR/valgrind_report.txt" \
             "$SHELL_PATH" < "$MEMORY_TEST_FILE" > /dev/null 2>&1
    
    # Check for memory leaks
    if grep -q "no leaks are possible" "$RESULTS_DIR/valgrind_report.txt"; then
        echo -e "${GREEN}No memory leaks detected.${NC}"
    else
        echo -e "${RED}Memory leaks detected. See $RESULTS_DIR/valgrind_report.txt for details.${NC}"
        
        # Extract summary
        echo "Summary of leaks:"
        grep -A 2 "LEAK SUMMARY" "$RESULTS_DIR/valgrind_report.txt"
    fi
}

# CPU profiling
profile_cpu() {
    echo -e "${BLUE}Profiling CPU usage...${NC}"
    
    if ! command -v gprof &> /dev/null; then
        echo -e "${YELLOW}Gprof not found, skipping CPU profiling.${NC}"
        return
    fi
    
    # Check if shell was compiled with profiling enabled
    if ! nm "$SHELL_PATH" | grep -q "mcount"; then
        echo -e "${YELLOW}Shell was not compiled with profiling support.${NC}"
        echo "Recompile with -pg flag to enable profiling."
        return
    fi
    
    # Run with profiling
    "$SHELL_PATH" < "$COMMAND_FILE" > /dev/null 2>&1
    
    # Generate profiling report
    gprof "$SHELL_PATH" gmon.out > "$RESULTS_DIR/gprof_report.txt"
    
    echo -e "${GREEN}CPU profiling completed. See $RESULTS_DIR/gprof_report.txt for details.${NC}"
}

# Main execution
echo -e "${YELLOW}C Shell Performance Test${NC}"
echo "======================================="

# Run the tests
generate_commands
run_performance_test
test_memory_usage
profile_cpu

echo -e "${GREEN}Performance testing completed.${NC}"
echo "Results are available in the $RESULTS_DIR directory."
exit 0