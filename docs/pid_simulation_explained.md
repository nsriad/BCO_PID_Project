# PID Simulation Explained
### Understanding PID Simulation and Plant Models for BCO Optimization

---

## 1. PID Controller Overview

A PID (Proportional–Integral–Derivative) controller adjusts a system’s input using:

$$
u(t) = K_p\, e(t) + K_i \int e(t)\, dt + K_d\, \frac{de(t)}{dt},
$$

where:

- \( e(t) = r(t) - y(t) \) is the tracking error  
- \( r(t) \) is a unit step  
- \( y(t) \) is the plant output  

In our C++ PID simulator:

- The goal is to **minimize tracking error**
- The optimization target is **Mean Squared Error (MSE)**

---

## 2. Plant Models

We evaluate the PID controller on three transfer functions.

### **First-order plant**

$$
G_1(s) = \frac{1}{s + 1}
$$

### **Second-order plant**

$$
G_2(s) = \frac{5}{s^2 + 2s + 5}
$$

### **Third-order plant**

$$
G_3(s) = \frac{10}{(s + 1)(s^2 + 2s + 10)}
$$

---

## 3. Converting Transfer Functions to Time-Domain Equations

A transfer function:

$$
G(s) = \frac{Y(s)}{U(s)}
$$

must be converted into a **differential equation** to simulate numerically.

---

### ### 3.1 First-Order System \(G_1(s)\)

$$
G_1(s) = \frac{1}{s + 1}
$$

Multiply:

$$
(s + 1)Y(s) = U(s)
$$

Inverse Laplace:

$$
y'(t) + y(t) = u(t)
$$

Which becomes the numerical update:

```
y += dt * (-1*y + 1*u);
```

---

### ### 3.2 Second-Order System \(G_2(s)\)

$$
G_2(s) = \frac{5}{s^2 + 2s + 5}
$$

Time domain:

$$
y'' + 2y' + 5y = 5u
$$

State variables:

- \(x_1 = y\)
- \(x_2 = y'\)

Then:

$$
x_1' = x_2
$$

$$
x_2' = 5u - 2x_2 - 5x_1
$$

C++ implementation:

```
x2 += dt * (5*u - 2*x2 - 5*x1);
x1 += dt * x2;
y = x1;
```

---

### ### 3.3 Third-Order System \(G_3(s)\)

Denominator expanded:

$$
(s+1)(s^2 + 2s + 10) = s^3 + 3s^2 + 12s + 10
$$

Time domain:

$$
y''' + 3y'' + 12y' + 10y = 10u
$$

State form:

- \(x_1 = y\)
- \(x_2 = y'\)
- \(x_3 = y''\)

Then:

$$
x_3' = 10u - 3x_3 - 12x_2 - 10x_1
$$

C++ implementation:

```
x3 += dt * (10*u - 3*x3 - 12*x2 - 10*x1);
x2 += dt * x3;
x1 += dt * x2;
y = x1;
```

---

## 4. Euler Numerical Integration

We use the simple Euler method:

$$
x(t + dt) = x(t) + dt \cdot x'(t)
$$

Values used:

- **dt = 0.01**
- **simTime = 40 seconds**

This is fast and stable for PID tuning.

---

## 5. Fitness Function (MSE)

After simulating the response:

$$
e(t) = 1 - y(t)
$$

MSE is:

$$
\text{MSE} = \frac{1}{N} \sum e(t)^2
$$

Lower MSE → better PID controller.

---

## 6. Summary Table

| Plant | Order | Differential Equation |
|------|-------|------------------------|
| \(G_1(s)\) | 1st | \(y' = -y + u\) |
| \(G_2(s)\) | 2nd | \(y'' = 5u - 2y' - 5y\) |
| \(G_3(s)\) | 3rd | \(y''' = 10u - 3y'' - 12y' - 10y\) |

---

## 7. Block Diagram

```
        +-------------+        +-----------+
 r(t) → |    PID      |  u(t) → |   Plant  | → y(t)
        +-------------+        +-----------+
                ↑                     |
                |_____________________|
                    Negative feedback
```

---

# END OF FILE
