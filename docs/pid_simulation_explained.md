# PID Simulation Explained
### Understanding PID Simulation and Plant Models for BCO Optimization

---

## 1. PID Controller Overview

A PID (Proportional–Integral–Derivative) controller adjusts a system’s input using:

\[
u(t) = K_p e(t) + K_i \int e(t)\, dt + K_d \frac{de(t)}{dt},
\]

where  
\(e(t) = r(t) - y(t)\) is the tracking error.

In the C++ PID simulator:

- r(t) is a unit step  
- y(t) is the output  
- Goal is to minimize tracking error  
- Optimization target is **Mean Squared Error (MSE)**

---

## 2. Plant Models

### First-order:
\[
G_1(s) = \frac{1}{s + 1}
\]

### Second-order:
\[
G_2(s) = \frac{5}{s^2 + 2s + 5}
\]

### Third-order:
\[
G_3(s) = \frac{10}{(s + 1)(s^2 + 2s + 10)}
\]

---

## 3. From Transfer Functions to Differential Equations

We convert Laplace-domain models into time-domain ODEs.

### First-order:
\[
y' + y = u
\]
C++:
```
y += dt * (-a * y + b * u);
```

### Second-order:
\[
y'' + 2y' + 5y = 5u
\]

State form:
\[
x_1' = x_2
\]
\[
x_2' = 5u - 2x_2 - 5x_1
\]

C++:
```
double y_ddot = b*u - a1*x2 - a0*x1;
```

### Third-order:
Expanded:
\[
y''' + 3y'' + 12y' + 10y = 10u
\]

State form:
\[
x_1' = x_2, \quad x_2' = x_3
\]
\[
x_3' = 10u - 3x_3 - 12x_2 - 10x_1
\]

C++:
```
double y_dddot = b*u - a2*x3 - a1*x2 - a0*x1;
```

---

## 4. Numerical Simulation (Euler Method)

We use:
\[
x(t + dt) = x(t) + dt \cdot x'(t)
\]

`dt = 0.01`, `simTime = 40`.

---

## 5. Fitness (MSE)

\[
\text{MSE} = \frac{1}{N}\sum (1 - y(t))^2
\]

Used as fitness for optimization.

---

## 6. Summary Table

| Plant | Order | ODE |
|-------|--------|----------------|
| G1 | 1st | y' = -y + u |
| G2 | 2nd | y'' = 5u - 2y' - 5y |
| G3 | 3rd | y''' = 10u - 3y'' - 12y' - 10y |

---

## 7. Diagram
```
        +-------------+        +-----------+
 r(t) → |    PID      |  u(t) →|   Plant   | → y(t)
        +-------------+        +-----------+
                ↑                     |
                |_____________________|
                    Negative feedback
```

