#!/bin/bash
echo "processes, run, time_s" > ./reports/benchmark.csv

for p in 1 2 4; do 
    for run in 1 2 3; do
        time=$(mpirun -np "$p" ./conway_mpi 600 200 1 | awk '{print $(NF-1)}')
        echo "$p,$run,$time" >> ./reports/benchmark.csv
    done
done