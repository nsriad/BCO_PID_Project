#include "pid_simulator.h"
#include <cmath>   // for fabs()
#include <vector>

// Simulates a PID controller on a linear plant defined by (num/den)
// num, den = plant coefficients
// dt = simulation step
// simTime = total simulation time
PIDResult simulatePID(const PIDParams& params, const double* num, int numSize,
                      const double* den, int denSize, double dt, double simTime)
{
    double Kp = params.Kp;
    double Ki = params.Ki;
    double Kd = params.Kd;

    // Simulation variables
    double y  = 0.0;  // plant output
    double x1 = 0.0;  // internal plant state
    double x2 = 0.0;  // second state if needed
    double x3 = 0.0;  // third state if needed

    double integral   = 0.0;
    double prevError  = 0.0;

    double mse   = 0.0;
    int steps    = (int)(simTime / dt);
    double reference = 1.0;

    // safety thresholds
    const double MAX_VAL = 1e6;

    for (int i = 0; i < steps; i++) {

        double error = reference - y;

        // If error already non-finite, bail out
        if (!std::isfinite(error)) {
            mse = 1e9;
            break;
        }

        // PID terms
        integral += error * dt;
        double derivative = (error - prevError) / dt;
        double u = Kp * error + Ki * integral + Kd * derivative;

        // Check controller output
        if (!std::isfinite(u) || std::fabs(u) > MAX_VAL ||
            !std::isfinite(integral) || std::fabs(integral) > MAX_VAL) {
            mse = 1e9;
            break;
        }

        // plant simulation 
        if (denSize == 2) {
            // First order: y' = -a*y + b*u
            double a = den[1];
            double b = num[0];
            y += dt * (-a * y + b * u);
        }
        else if (denSize == 3) {
            // Second order: y'' + a1*y' + a0*y = b*u
            double a1 = den[1];
            double a0 = den[2];
            double b  = num[0];

            double y_ddot = b * u - a1 * x2 - a0 * x1;
            x2 += dt * y_ddot;
            x1 += dt * x2;
            y  = x1;
        }
        else if (denSize == 4) {
            // Third order: y''' + a2*y'' + a1*y' + a0*y = b*u
            double a2 = den[1];
            double a1 = den[2];
            double a0 = den[3];
            double b  = num[0];

            double y_dddot = b * u - a2 * x3 - a1 * x2 - a0 * x1;
            x3 += dt * y_dddot;
            x2 += dt * x3;
            x1 += dt * x2;
            y  = x1;
        }

        // Check plant output
        if (!std::isfinite(y) || std::fabs(y) > MAX_VAL) {
            mse = 1e9;
            break;
        }

        mse += error * error;
        prevError = error;
    }

    if (mse < 1e9 && steps > 0) {
        mse /= steps;   // normal case
    }
    // else mse == 1e9 which is unstable candidate penalized

    PIDResult result;
    result.mse = mse;
    result.finalValue = y;
    return result;
}