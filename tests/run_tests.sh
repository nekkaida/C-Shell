#!/bin/bash
# Test runner script for C Shell

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$ROOT_DIR/bin"
FIXTURE_DIR="$SCRIPT_DIR/fixtures"

# Ensure we have a shell to test
if [ ! -f "$BIN_DIR/shell" ]; then
    echo -e "${RED}Error: Shell executable not found at $BIN_DIR/shell${NC}"
    echo "Make sure you've built the project with 'make' or through CMake."
    exit 1
fi

# Run unit tests
run_unit_tests() {
    echo -e "${BLUE}Running unit tests...${NC}"
    
    # Find and run all unit test executables
    find "$BIN_DIR" -name "test_*" -type f -executable | while read -r test_exec; do
        test_name=$(basename "$test_exec")
        echo -n "Running $test_name... "
        
        # Run the test and capture output
        output=$("$test_exec" 2>&1)
        status=$?
        
        if [ $status -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED${NC}"
            echo "$output"
        fi
    done
    
    echo ""
}

# Run integration tests
run_integration_tests() {
    echo -e "${BLUE}Running integration tests...${NC}"
    
    # Basic command test
    echo -n "Testing basic command execution... "
    echo "echo test_output" | "$BIN_DIR/shell" > /tmp/shell_output
    if grep -q "test_output" /tmp/shell_output; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
        cat /tmp/shell_output
    fi
    
    # Built-in commands test
    echo -n "Testing built-in commands... "
    echo -e "pwd\ncd /tmp\npwd\nexit" | "$BIN_DIR/shell" > /tmp/shell_output
    if grep -q "/tmp" /tmp/shell_output; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
        cat /tmp/shell_output
    fi
    
    # Redirection test
    echo -n "Testing output redirection... "
    echo "echo redirection_test > /tmp/redirection_test.txt" | "$BIN_DIR/shell"
    if [ -f "/tmp/redirection_test.txt" ] && grep -q "redirection_test" /tmp/redirection_test.txt; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
        if [ -f "/tmp/redirection_test.txt" ]; then
            cat /tmp/redirection_test.txt
        else
            echo "File not created"
        fi
    fi
    
    # Clean up
    rm -f /tmp/shell_output /tmp/redirection_test.txt
    
    echo ""
}

# Run script tests from fixtures
run_script_tests() {
    echo -e "${BLUE}Running script tests...${NC}"
    
    # Run each test script in fixtures directory
    find "$FIXTURE_DIR" -name "*.sh" -type f | while read -r script_file; do
        test_name=$(basename "$script_file")
        echo -n "Running $test_name... "
        
        # Execute the script
        bash "$script_file" "$BIN_DIR/shell"
        status=$?
        
        if [ $status -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED${NC}"
        fi
    done
    
    echo ""
}

# Print summary
print_summary() {
    echo -e "${BLUE}Test Summary:${NC}"
    echo -e "${GREEN}Passed tests: $passed${NC}"
    echo -e "${RED}Failed tests: $failed${NC}"
    
    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
        return 0
    else
        echo -e "${RED}Some tests failed.${NC}"
        return 1
    fi
}

# Main execution
echo -e "${YELLOW}C Shell Test Runner${NC}"
echo "======================================="

# Run all test types
run_unit_tests
run_integration_tests
run_script_tests

# Count results
passed=$(grep -c "PASSED" /tmp/shell_output 2>/dev/null || echo 0)
failed=$(grep -c "FAILED" /tmp/shell_output 2>/dev/null || echo 0)

# Print summary and exit with appropriate code
print_summary
exit $?