# Parallelization Approach (OpenMP BCO)

## 1. Overview

Each bee in BCO represents a candidate PID controller. Since evaluating fitness (PID simulation + MSE) is independent per bee, loops over bees are parallelized.

## 2. OpenMP Usage

Uses:
- ```#pragma omp parallel for```
- ```#pragma omp parallel for reduction(min:)```
- ```omp_set_num_threads()```
- ```omp_get_wtime()```

## 3. Thread-Local RNG

We use ```thread_local std::mt19937``` to avoid data races.

## 4. Timing and Experiments

```runBCOParallel()``` is timed with ```omp_get_wtime()```. Script ```run_experiments.sh``` sweeps thread counts and logs CSV results.

## 5. Scaling Behavior

Speedup is good until overhead/RNG cost dominates. Because BCO is stochastic, different thread counts produce slightly different PID gains, but runtime scaling is the main focus.

