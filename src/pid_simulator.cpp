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
    double y = 0.0;   // plant output
    double x1 = 0.0;  // internal plant state
    double x2 = 0.0;  // second state if needed
    double x3 = 0.0;  // third state if needed

    double integral = 0.0;
    double prevError = 0.0;

    double mse = 0.0;
    int steps = (int)(simTime / dt);
    double reference = 1.0;

    // 1st, 2nd, or 3rd order approximations
    // User provides num/den arrays. We handle 1-2-3 order by checking size.

    for (int i = 0; i < steps; i++) {
        double error = reference - y;

        // PID Controller
        integral += error * dt;
        double derivative = (error - prevError) / dt;
        double u = Kp * error + Ki * integral + Kd * derivative;

        // plant simulation
        // frst order: y' = -a*y + b*u
        if (denSize == 2) {
            double a = den[1];      // s + a
            double b = num[0];      // constant numerator
            y += dt * (-a * y + b * u);
        }

        // CASE 2: Second order: y'' + a1*y' + a0*y = b*u
        else if (denSize == 3) {
            double a1 = den[1];
            double a0 = den[2];
            double b = num[0];

            // x1 = y, x2 = y'
            double y_ddot = b * u - a1 * x2 - a0 * x1;
            x2 += dt * y_ddot;
            x1 += dt * x2;
            y = x1;
        }

        // CASE 3: Third order (approximation for G3)
        else if (denSize == 4) {
            double a2 = den[1];
            double a1 = den[2];
            double a0 = den[3];
            double b = num[0];

            // x1 = y, x2 = y', x3 = y''
            double y_dddot = b * u - a2 * x3 - a1 * x2 - a0 * x1;
            x3 += dt * y_dddot;
            x2 += dt * x3;
            x1 += dt * x2;
            y = x1;
        }

        mse += (error * error);
        prevError = error;
    }

    mse /= steps;

    PIDResult result;
    result.mse = mse;
    result.finalValue = y;
    return result;
}
