#include <iostream>
#include <vector>
#include "bco.h"
#include "utils.h"

using namespace std;


// Define the three plants
// G1(s) = 1 / (s + 1)
void getG1(vector<double>& num, vector<double>& den)
{
    num = {1.0};        // numerator
    den = {1.0, 1.0};   // s + 1   (1*s + 1)
}

// G2(s) = 5 / (s^2 + 2s + 5)
void getG2(vector<double>& num, vector<double>& den)
{
    num = {5.0};
    den = {1.0, 2.0, 5.0}; // s^2 + 2s + 5
}

// G3(s) = 10 / ((s+1)(s^2 + 2s + 10))
// expanded: 10 / (s^3 + 3s^2 + 12s + 10)
void getG3(vector<double>& num, vector<double>& den)
{
    num = {10.0};
    den = {1.0, 3.0, 12.0, 10.0};
}

int main()
{
    initRandom(12345);

    // --- Select Plant ---
    vector<double> num, den;

    int choice = 1;
    cout << "Choose plant:\n";
    cout << "1. G1(s) = 1/(s+1)\n";
    cout << "2. G2(s) = 5/(s^2+2s+5)\n";
    cout << "3. G3(s) = 10/((s+1)(s^2+2s+10))\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) getG1(num, den);
    else if (choice == 2) getG2(num, den);
    else getG3(num, den);

    // BCO Settings
    BCOSettings settings;
    settings.numBees = 100;        // population size
    settings.maxIterations = 500;  // you can increase later
    settings.limit = 30;          // scout limit

    // PID parameter ranges
    settings.KpMin = -10.0; settings.KpMax = 10.0;
    settings.KiMin = -10.0; settings.KiMax = 10.0;
    settings.KdMin = -10.0; settings.KdMax = 10.0;

    settings.dt = 0.001;
    settings.simTime = 40.0;

    // Output best results
    PIDParams bestPID;
    double bestMSE = 1e9;

    // Create log file name based on plant
    const char* logFile;
    if (choice == 1) logFile = "data/logs/bco_G1_serial.csv";
    else if (choice == 2) logFile = "data/logs/bco_G2_serial.csv";
    else logFile = "data/logs/bco_G3_serial.csv";

    cout << "\nRunning Serial BCO Optimization...\n";

    runBCO(num.data(), num.size(),
           den.data(), den.size(),
           settings,
           bestPID,
           bestMSE,
           logFile);

    cout << "\n===== BCO Optimization Result =====\n";
    cout << "Best MSE: " << bestMSE << "\n";
    cout << "Best Kp: " << bestPID.Kp << "\n";
    cout << "Best Ki: " << bestPID.Ki << "\n";
    cout << "Best Kd: " << bestPID.Kd << "\n";

    cout << "\nLog saved to: " << logFile << "\n";

    return 0;
}
