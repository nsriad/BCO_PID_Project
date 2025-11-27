# Parallel Bee Colony Optimization for PID Tuning

This repository contains my CPE 512 project, where I use a Bee Colony Optimization (BCO) meta-heuristic to tune a PID controller for several plant models. The project includes:

- A **MATLAB/Simulink** baseline for classic PID tuning and performance metrics.
- A **serial C++ implementation** of BCO-based PID tuning.
- A **parallel C++ implementation using OpenMP**, with experiments on different thread counts to study speedup, efficiency, and cost.

The goal is to compare:
1. Baseline PID gains vs. BCO-optimized PID gains.
2. Serial BCO vs. OpenMP-parallel BCO in terms of execution time and scaling behavior.

## Repository Structure
```
BCO_PID_Project/
├─ include/
│  ├─ bco.h
│  ├─ bco_parallel.h
│  ├─ pid_simulator.h
│  ├─ utils.h
├─ src/
│  ├─ bco.cpp
│  ├─ bco_parallel.cpp
│  ├─ pid_simulator.cpp
│  ├─ utils.cpp
│  ├─ main_serial.cpp
│  ├─ main_parallel.cpp
├─ data/
│  ├─ logs/
├─ docs/
│  ├─ pid_baseline.md
│  ├─ bco_algorithm.md
│  ├─ parallelization.md
├─ run_experiments.sh
├─ parallel_results.csv
├─ README.md
```
## Plant Models

G1(s) = 1/(s+1)
G2(s) = 5/(s^2 + 2s + 5)
G3(s) = 10/((s+1)(s^2 + 2s + 10))

## Building the Code

Parallel version:
```
g++-15 -Iinclude \
    src/bco_parallel.cpp \
    src/main_parallel.cpp \
    src/pid_simulator.cpp \
    src/utils.cpp \
    src/bco.cpp \
    -fopenmp -O2 -o bco_parallel
```
Serial:

```
g++-15 -Iinclude \
    src/bco.cpp \
    src/main_serial.cpp \
    src/pid_simulator.cpp \
    src/utils.cpp \
    -O2 -o bco_serial
```
## Running
```
./bco_parallel <threads> <plant> H
./bco_parallel <threads> <plant> C
```
## Automated Experiments
```
./run_experiments.sh
```
## Notes

BCO is stochastic; thread counts lead to slightly different optimization trajectories.

