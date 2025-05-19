#!/bin/bash

# This script builds the edu interpreter with direct AST interpretation support

# Exit on error
set -e

echo "Building edu interpreter..."

# Run scons to build the project
scons

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Usage examples:"
    echo "  ./build/edu test_complex.edu                 # Run by interpreting the AST directly (default)"
    echo "  ./build/edu --transpile test_complex.edu     # Transpile to C++ without running"
    echo "  ./build/edu --compile test_complex.edu       # Transpile to C++, compile and run"
    echo "  ./build/edu --debug test_complex.edu         # Run with debug output"
    echo ""
    echo "You can now run: ./measure_interpreter.sh test_complex.edu"
    echo "to measure the performance difference between interpretation and transpilation."
else
    echo "Build failed."
    exit 1
fi