#ifndef PID_SIMULATOR_H
#define PID_SIMULATOR_H

//struct to hold PID parameters
struct PIDParams {
    double Kp;
    double Ki;
    double Kd;
};

// Result structure for simulation output
struct PIDResult {
    double mse;
    double finalValue;
};

// Function prototypes
PIDResult simulatePID(const PIDParams& params, const double* num, int numSize,
                      const double* den, int denSize, double dt, double simTime);

#endif
