#ifndef BCO_H
#define BCO_H

#include "pid_simulator.h"

// One bee = one PID candidate
struct Bee {
    PIDParams pid;   // Kp, Ki, Kd
    double fitness;  // MSE
    int trials;      // how many times it failed to improve
};

// Settings for the BCO algorithm
struct BCOSettings {
    int numBees;        // size of the bee population
    int maxIterations;  // how many iterations to run
    int limit;          // scout limit (stagnation threshold)

    double KpMin, KpMax;
    double KiMin, KiMax;
    double KdMin, KdMax;

    double dt;          // simulation time step
    double simTime;     // total simulation time
};

// Runs BCO for a single plant (given by num/den).
// bestParams and bestMSE will be filled with the best found solution.
// logFilePath: CSV file path for logging
void runBCO(const double* num, int numSize,
            const double* den, int denSize,
            const BCOSettings& settings,
            PIDParams& bestParams,
            double& bestMSE,
            const char* logFilePath);

#endif // BCO_H
