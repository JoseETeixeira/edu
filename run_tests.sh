#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Building the edu compiler...${NC}"
scons
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Running transpilation test...${NC}"
./build/edu --transpile test.edu
if [ $? -ne 0 ]; then
    echo -e "${RED}Transpilation test failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Compiling transpiled code...${NC}"
g++ -std=c++17 test.edu.cpp -o test_program
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation of transpiled code failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Running transpiled code...${NC}"
./test_program > transpiled_output.txt
if [ $? -ne 0 ]; then
    echo -e "${RED}Execution of transpiled code failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Running direct execution test...${NC}"
./build/edu test.edu > direct_output.txt
if [ $? -ne 0 ]; then
    echo -e "${RED}Direct execution test failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Comparing outputs...${NC}"
diff transpiled_output.txt direct_output.txt
if [ $? -ne 0 ]; then
    echo -e "${RED}Output mismatch between transpiled and direct execution!${NC}"
    exit 1
else
    echo -e "${GREEN}Basic test passed: Transpiled and direct execution produce the same output.${NC}"
fi

echo -e "${YELLOW}Running complex test...${NC}"
./build/edu test_complex.edu > complex_output.txt
if [ $? -ne 0 ]; then
    echo -e "${RED}Complex test execution failed!${NC}"
    exit 1
fi

echo -e "${YELLOW}Complex test output:${NC}"
cat complex_output.txt

# Verify expected output from complex test
expected_outputs=(
    "Hello, edu language!"
    "a + b = 30"
    "Factorial of 5 = 120"
    "Is 17 prime? true"
    "Is 20 prime? false"
    "MathUtils initialized"
    "Addition: 15 + 7 = 22"
    "Subtraction: 15 - 7 = 8"
    "Multiplication: 15 * 7 = 105"
    "Division: 15 / 7 = 2.14"
    "Power: 2^8 = 256"
    "Error: Division by zero"
    "Division by zero test: 0"
)

failures=0
for expected in "${expected_outputs[@]}"; do
    if ! grep -q "$expected" complex_output.txt; then
        echo -e "${RED}Failed to find expected output: $expected${NC}"
        failures=$((failures+1))
    fi
done

if [ $failures -eq 0 ]; then
    echo -e "${GREEN}Complex test passed: All expected outputs found.${NC}"
else
    echo -e "${RED}Complex test failed: $failures expected outputs not found.${NC}"
    exit 1
fi

echo -e "${GREEN}All tests passed!${NC}"

# Clean up
rm -f test_program transpiled_output.txt direct_output.txt complex_output.txt

exit 0