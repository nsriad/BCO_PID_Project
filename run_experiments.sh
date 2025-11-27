#!/bin/bash

# ------------------------------
# AUTOMATED PARALLEL RUN SCRIPT
# For: bco_parallel
# Generates: parallel_results.csv
# ------------------------------

OUTPUT_FILE="parallel_results.csv"

# Header
echo "threads,plant,time" > $OUTPUT_FILE

# Thread counts to test
THREADS=(1 2 4 6 8 10)

# Plants to test: G1, G2, G3
PLANTS=(1 2 3)

# Loop over all combinations
for t in "${THREADS[@]}"; do
    for p in "${PLANTS[@]}"; do
        
        echo "Running: threads=$t plant=G$p"

        # Run in CSV mode so it prints:
        # threads,plant,time,checksum
        ./bco_parallel $t $p C >> $OUTPUT_FILE

        sleep 0.5   # small delay to avoid overheating on Mac

    done
done

echo "---------------------------------------"
echo "All experiments completed!"
echo "Results saved to $OUTPUT_FILE"
echo "---------------------------------------"
