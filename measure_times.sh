#!/usr/bin/env bash
# filepath: /home/edu/Desktop/edu/measure_times.sh
# Script to measure execution time of edu, python, and node implementations
# With multiple runs to get more accurate results

# Set locale to ensure proper decimal point handling
export LC_NUMERIC="en_US.UTF-8"

set -e

echo "Building edu compiler..."
scons > /dev/null

# Create results directory if it doesn't exist
mkdir -p results

# Function to convert comma to period for decimal numbers
fix_decimal() {
    echo "$1" | tr ',' '.' | tr -d '[:space:]'
}

# Number of runs for each implementation
NUM_RUNS=5

# Number of iterations per timing measurement (adjusted per implementation)
DIRECT_ITERATIONS=100    # For direct interpretation (fastest)
TRANSPILE_ITERATIONS=5   # For transpile+compile (slowest)
PYTHON_ITERATIONS=50     # For Python implementation
NODE_ITERATIONS=50       # For Node.js implementation

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
    # Run multiple iterations to get measurable times
    time_result=$( { time -p bash -c "for i in {1..$DIRECT_ITERATIONS}; do ./build/edu test_complex.edu > /dev/null; done"; } 2>&1 | grep real | awk '{print $2}' )
    time_val=$(fix_decimal "$time_result")
    if [[ -z "$time_val" ]]; then time_val=0; fi
    # Ensure time_val is a valid floating point number
    if [[ "$time_val" =~ ^[0-9]*\.?[0-9]*$ ]]; then
        time_fixed=$(printf "%.6f" "$time_val")
    else
        time_fixed="0.000000"
        echo "Warning: Invalid time format received: '$time_val'"
    fi
    edu_direct_times+=("$time_fixed")
    echo "    Time: $time_fixed seconds"
    
    # Transpile+compile method
    echo "  Transpile+compile method..."
    time_result=$( { time -p bash -c "for i in {1..$TRANSPILE_ITERATIONS}; do ./build/edu --compile test_complex.edu > /dev/null; done"; } 2>&1 | grep real | awk '{print $2}' )
    time_val=$(fix_decimal "$time_result")
    if [[ -z "$time_val" ]]; then time_val=0; fi
    # Ensure time_val is a valid floating point number
    if [[ "$time_val" =~ ^[0-9]*\.?[0-9]*$ ]]; then
        time_fixed=$(printf "%.6f" "$time_val")
    else
        time_fixed="0.000000"
        echo "Warning: Invalid time format received: '$time_val'"
    fi
    edu_transpile_times+=("$time_fixed")
    echo "    Time: $time_fixed seconds"
    
    # Python
    echo "  Python implementation..."
    time_result=$( { time -p bash -c "for i in {1..$PYTHON_ITERATIONS}; do python3 test_complex.py > /dev/null; done"; } 2>&1 | grep real | awk '{print $2}' )
    time_val=$(fix_decimal "$time_result")
    if [[ -z "$time_val" ]]; then time_val=0; fi
    # Ensure time_val is a valid floating point number
    if [[ "$time_val" =~ ^[0-9]*\.?[0-9]*$ ]]; then
        time_fixed=$(printf "%.6f" "$time_val")
    else
        time_fixed="0.000000"
        echo "Warning: Invalid time format received: '$time_val'"
    fi
    python_times+=("$time_fixed")
    echo "    Time: $time_fixed seconds"
    
    # Node.js
    echo "  Node.js implementation..."
    time_result=$( { time -p bash -c "for i in {1..$NODE_ITERATIONS}; do node test_complex.js > /dev/null; done"; } 2>&1 | grep real | awk '{print $2}' )
    time_val=$(fix_decimal "$time_result")
    if [[ -z "$time_val" ]]; then time_val=0; fi
    # Ensure time_val is a valid floating point number
    if [[ "$time_val" =~ ^[0-9]*\.?[0-9]*$ ]]; then
        time_fixed=$(printf "%.6f" "$time_val")
    else
        time_fixed="0.000000"
        echo "Warning: Invalid time format received: '$time_val'"
    fi
    node_times+=("$time_fixed")
    echo "    Time: $time_fixed seconds"
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
# Normalize by dividing by appropriate iteration counts
edu_direct_avg=$(echo "scale=6; $edu_direct_avg / $DIRECT_ITERATIONS" | bc -l)
edu_transpile_avg=$(echo "scale=6; $edu_transpile_avg / $TRANSPILE_ITERATIONS" | bc -l)
python_avg=$(echo "scale=6; $python_avg / $PYTHON_ITERATIONS" | bc -l)
node_avg=$(echo "scale=6; $node_avg / $NODE_ITERATIONS" | bc -l)

# Format for display (remove leading zeros)
edu_direct_display=$(echo "$edu_direct_avg" | sed 's/^0*//')
edu_transpile_display=$(echo "$edu_transpile_avg" | sed 's/^0*//')
python_display=$(echo "$python_avg" | sed 's/^0*//')
node_display=$(echo "$node_avg" | sed 's/^0*//')

echo -e "\nAverage execution times (per run):"
echo "edu (direct):    $edu_direct_display seconds"
echo "edu (transpile): $edu_transpile_display seconds"
echo "python:          $python_display seconds"
echo "node:            $node_display seconds"

# Calculate speedup factor
if (( $(echo "$edu_direct_avg > 0.000001" | bc -l) )) && (( $(echo "$edu_transpile_avg > 0.000001" | bc -l) )); then
    speedup=$(echo "scale=2; $edu_transpile_avg / $edu_direct_avg" | bc -l)
    echo -e "\nSpeedup factor: ${speedup}x faster with direct interpretation"
else
    if (( $(echo "$edu_direct_avg <= 0.000001" | bc -l) )); then
        echo -e "\nDirect interpretation time too small for accurate comparison"
    else
        echo -e "\nCouldn't calculate speedup (timing issue)"
    fi
fi

# Save detailed results to a file
echo "# Performance Comparison ($(date))" > results/timing_results.md
echo "" >> results/timing_results.md
echo "| Implementation | Time (seconds) | Relative Speed |" >> results/timing_results.md
echo "|---------------|----------------|----------------|" >> results/timing_results.md
echo "| Edu (direct interpretation) | $edu_direct_avg | 1.00x |" >> results/timing_results.md

# Calculate relative speeds
if (( $(echo "$edu_direct_avg > 0.000001" | bc -l) )); then
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