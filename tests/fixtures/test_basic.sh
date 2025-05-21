#!/bin/bash
# Basic shell test script

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# The shell executable is passed as the first argument
SHELL="$1"

# Verify that a shell executable was provided
if [ -z "$SHELL" ] || [ ! -x "$SHELL" ]; then
    echo -e "${RED}Error: Shell executable not provided or not executable${NC}"
    exit 1
fi

# Temporary output file
OUTPUT=$(mktemp)

# Run shell commands
cat << EOF | "$SHELL" > "$OUTPUT" 2>&1
echo "Hello, world!"
pwd
cd /tmp
pwd
echo "Testing complete"
exit 0
EOF

# Check the output
if grep -q "Hello, world!" "$OUTPUT" && \
   grep -q "Testing complete" "$OUTPUT" && \
   grep -q "/tmp" "$OUTPUT"; then
    echo -e "${GREEN}Basic shell test passed!${NC}"
    rm -f "$OUTPUT"
    exit 0
else
    echo -e "${RED}Basic shell test failed!${NC}"
    echo "Output:"
    cat "$OUTPUT"
    rm -f "$OUTPUT"
    exit 1
fi