# Bee Colony Optimization (BCO) Explained
### BCO for PID Controller Tuning

---

## 1. What is BCO?

Bee Colony Optimization is a population-based algorithm inspired by honeybee foraging behavior.  
Each bee represents one candidate PID parameter set:

$$
[K_p,\; K_i,\; K_d]
$$

The goal is to minimize tracking error (MSE).

---

## 2. Why Use BCO for PID Tuning?

- PID tuning is nonlinear  
- Classical tuning formulas fail for complex plants  
- BCO is simple, robust, and naturally parallelizable  
- Works well across first-order, second-order, and higher-order models  

---

## 3. Core Phases of BCO

```
Employed Bees -> Onlooker Bees -> Scout Bees
```

### **Employed Bees (Local Search)**

Each bee modifies its current solution:

$$
\theta' = \theta + \phi(\theta - \theta_k)
$$

where:

- \(\phi\) is a random number in \([-1, 1]\)  
- \(\theta_k\) is a different bee  

If fitness improves → bee updates position.

---

### **Onlooker Bees (Probabilistic Selection)**

Each onlooker selects a solution based on:

$$
p_i = \frac{f_i}{\sum_j f_j}
$$

Better bees get more attention.

---

### **Scout Bees (Random Exploration)**

If a bee stagnates, reset it:

```
if (no improvement for limit iterations)
    reinitialize randomly
```

This avoids local minima.

---

## 4. Fitness Function

We minimize Mean-Squared Error:

$$
\text{MSE} = \frac{1}{N} \sum_{t=0}^{T} (1 - y(t))^2
$$

Computed using C++ PID simulator.

---

## 5. Algorithm Steps

1. Initialize bee population  
2. Evaluate fitness  
3. Employed bee phase  
4. Onlooker selection and update  
5. Scout phase  
6. Repeat for maxIterations  

---

## 6. Parallel BCO (OpenMP)

Fitness evaluation is independent → perfect for parallelism.

### Serial:
```
for (i = 0; i < numBees; i++)
    fitness[i] = simulatePID(...);
```

### Parallel:
```
#pragma omp parallel for
for (i = 0; i < numBees; i++)
    fitness[i] = simulatePID(...);
```

Leads to **5×–30× speedup** depending on CPU.

---

## 7. Logging for Visualization

Each iteration logs:

```
iteration, bestMSE, bestKp, bestKi, bestKd
```

Used to plot:

- convergence curve  
- parameter evolution  
- serial vs parallel performance  

---

## 8. Diagram

```
Initialization
      ↓
+-------------------------+
|     Employed Bees       |
+-------------------------+
      ↓
+-------------------------+
|     Onlooker Bees       |
+-------------------------+
      ↓
+-------------------------+
|       Scout Bees        |
+-------------------------+
      ↓
     Stop?
```

---

# END OF FILE
