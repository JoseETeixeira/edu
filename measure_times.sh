#!/usr/bin/env bash
# Script to measure execution time of edu, python, and node implementations
# With multiple runs to get more accurate results

set -e

echo "Building edu compiler..."
scons > /dev/null

# Create results directory if it doesn't exist
mkdir -p results

# Function to convert comma to period for decimal numbers
fix_decimal() {
    echo "$1" | tr ',' '.'
}

# Number of runs for each implementation
NUM_RUNS=5

echo -e "\nMeasuring execution times with $NUM_RUNS runs (average shown):"

# Arrays to store times
edu_direct_times=()
edu_transpile_times=()
python_times=()
node_times=()

# Run multiple times and collect measurements
for (( i=1; i<=NUM_RUNS; i++ )); do
    echo -e "\nRun $i of $NUM_RUNS:"
    
    # Direct interpretation
    echo "  Direct interpretation..."
    time_result=$( { time -p ./build/edu test_complex_fixed.edu > /dev/null; } 2>&1 | grep real | awk '{print $2}' )
    time_fixed=$(fix_decimal "$time_result")
    edu_direct_times+=($time_fixed)
    echo "    Time: $time_result seconds"
    
    # Transpile-compile method
    echo "  Transpile+compile method..."
    time_result=$( { time -p ./build/edu --compile test_complex_fixed.edu > /dev/null; } 2>&1 | grep real | awk '{print $2}' )
    time_fixed=$(fix_decimal "$time_result")
    edu_transpile_times+=($time_fixed)
    echo "    Time: $time_result seconds"
    
    # Python
    echo "  Python implementation..."
    time_result=$( { time -p python3 test_complex.py > /dev/null; } 2>&1 | grep real | awk '{print $2}' )
    time_fixed=$(fix_decimal "$time_result")
    python_times+=($time_fixed)
    echo "    Time: $time_result seconds"
    
    # Node.js
    echo "  Node.js implementation..."
    time_result=$( { time -p node test_complex.js > /dev/null; } 2>&1 | grep real | awk '{print $2}' )
    time_fixed=$(fix_decimal "$time_result")
    node_times+=($time_fixed)
    echo "    Time: $time_result seconds"
done

# Calculate averages
calc_average() {
    local sum=0
    local count=0
    for t in "$@"; do
        sum=$(echo "$sum + $t" | bc -l)
        ((count++))
    done
    if [[ $count -gt 0 ]]; then
        echo "scale=6; $sum / $count" | bc -l
    else
        echo "0"
    fi
}

edu_direct_avg=$(calc_average "${edu_direct_times[@]}")
edu_transpile_avg=$(calc_average "${edu_transpile_times[@]}")
python_avg=$(calc_average "${python_times[@]}")
node_avg=$(calc_average "${node_times[@]}")

# Display averages
echo -e "\nAverage execution times:"
echo "edu (direct):    $edu_direct_avg seconds"
echo "edu (transpile): $edu_transpile_avg seconds" 
echo "python:          $python_avg seconds"
echo "node:            $node_avg seconds"

# Calculate speedup factor
if (( $(echo "$edu_direct_avg > 0" | bc -l) )) && (( $(echo "$edu_transpile_avg > 0" | bc -l) )); then
    speedup=$(echo "scale=2; $edu_transpile_avg / $edu_direct_avg" | bc -l)
    echo -e "\nSpeedup factor: ${speedup}x faster with direct interpretation"
else
    echo -e "\nCouldn't calculate speedup (timing issue)"
fi

# Save detailed results to a file
echo "# Performance Comparison ($(date))" > results/timing_results.md
echo "" >> results/timing_results.md
echo "| Implementation | Time (seconds) | Relative Speed |" >> results/timing_results.md
echo "|---------------|----------------|----------------|" >> results/timing_results.md
echo "| Edu (direct interpretation) | $edu_direct_avg | 1.00x |" >> results/timing_results.md

# Calculate relative speeds
if (( $(echo "$edu_direct_avg > 0" | bc -l) )); then
    if (( $(echo "$edu_transpile_avg > 0" | bc -l) )); then
        rel_transpile=$(echo "scale=2; $edu_transpile_avg / $edu_direct_avg" | bc -l)
        echo "| Edu (transpile+compile) | $edu_transpile_avg | ${rel_transpile}x slower |" >> results/timing_results.md
    else
        echo "| Edu (transpile+compile) | $edu_transpile_avg | N/A |" >> results/timing_results.md
    fi
    
    if (( $(echo "$python_avg > 0" | bc -l) )); then
        rel_python=$(echo "scale=2; $python_avg / $edu_direct_avg" | bc -l)
        echo "| Python | $python_avg | ${rel_python}x slower |" >> results/timing_results.md
    else
        echo "| Python | $python_avg | N/A |" >> results/timing_results.md
    fi
    
    if (( $(echo "$node_avg > 0" | bc -l) )); then
        rel_node=$(echo "scale=2; $node_avg / $edu_direct_avg" | bc -l)
        echo "| Node.js | $node_avg | ${rel_node}x slower |" >> results/timing_results.md
    else
        echo "| Node.js | $node_avg | N/A |" >> results/timing_results.md
    fi
else
    echo "| Edu (transpile+compile) | $edu_transpile_avg | N/A |" >> results/timing_results.md
    echo "| Python | $python_avg | N/A |" >> results/timing_results.md
    echo "| Node.js | $node_avg | N/A |" >> results/timing_results.md
fi

echo -e "\nDetailed results saved to results/timing_results.md"