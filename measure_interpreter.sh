#!/bin/bash

# This script measures and compares the performance of direct interpretation
# versus transpilation+compilation for edu code files

# Exit on error
set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <edu_file>"
    exit 1
fi

EDU_FILE=$1

if [ ! -f "$EDU_FILE" ]; then
    echo "Error: File $EDU_FILE not found"
    exit 1
fi

echo "Measuring performance for $EDU_FILE"

# Ensure we have a test directory
mkdir -p test_output

# Run direct interpreter (AST interpreter)
echo "Running with direct AST interpretation..."
time ./build/edu "$EDU_FILE" > test_output/direct_output.txt

# Run with transpilation to C++ and compilation
echo "Running with transpilation and compilation..."
time ./build/edu --compile "$EDU_FILE" > test_output/complex_output.txt

echo "Done! Output written to test_output/direct_output.txt and test_output/complex_output.txt"