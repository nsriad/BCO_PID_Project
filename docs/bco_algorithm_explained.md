# Bee Colony Optimization (BCO) Explained
### BCO for PID Controller Tuning

---

## 1. What is BCO?

A nature-inspired optimization algorithm based on foraging behavior of bees.

Each bee = one PID parameter set:
\[
[K_p, K_i, K_d]
\]

---

## 2. Why BCO for PID?

- PID tuning is nonlinear  
- Analytical formulas often insufficient  
- BCO is simple, robust, parallelizable  

---

## 3. BCO Phases

```
Employed Bees → Onlooker Bees → Scout Bees
```

### Employed Bees
Local exploration:
\[
\theta' = \theta + \phi(\theta - \theta_k)
\]

### Onlooker Bees
Probability selection:
\[
p_i = \frac{f_i}{\sum f_j}
\]

### Scout Bees
Random restart when stagnation occurs.

---

## 4. Fitness Function

We minimize **MSE**:

\[
\text{MSE} = \frac{1}{N}\sum (1 - y(t))^2
\]

MSE computed using the C++ PID simulator.

---

## 5. Algorithm Steps

1. Randomly initialize bees  
2. Evaluate fitness  
3. Employed bee update  
4. Onlooker selection  
5. Scout phase  
6. Repeat  

---

## 6. Parallel BCO (OpenMP)

Fitness evaluation is independent for each bee:

Serial:
```
for i:
    fitness[i] = simulate(...)
```

Parallel:
```
#pragma omp parallel for
for i:
    fitness[i] = simulate(...)
```

This gives 5×–30× speedup depending on CPU.

---

## 7. Logging

We log:
```
iteration, bestMSE, bestKp, bestKi, bestKd
```

Plots generated via MATLAB or Python.

---

## 8. Diagram
```
Initialization
      ↓
+------------------------+
|    Employed Bees       |
+------------------------+
      ↓
+------------------------+
|    Onlooker Bees       |
+------------------------+
      ↓
+------------------------+
|     Scout Bees         |
+------------------------+
      ↓
     Check Stop?
```

