#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

#include "bco_parallel.h"
#include "pid_simulator.h"
#include "utils.h"

using namespace std;


// Plant definitions (same as serial version)
void getG1(vector<double>& num, vector<double>& den)
{
    num = {1.0};
    den = {1.0, 1.0};
}

void getG2(vector<double>& num, vector<double>& den)
{
    num = {5.0};
    den = {1.0, 2.0, 5.0};
}

void getG3(vector<double>& num, vector<double>& den)
{
    num = {10.0};
    den = {1.0, 3.0, 12.0, 10.0};
}


// Usage: ./bco_parallel <threads> <plantIndex{1,2,3}> [H|C]
// H = human-readable output
// C = CSV output
int main(int argc, char* argv[])
{
    // Argument Parsing & Validation
    if (argc != 3 && argc != 4) {
        cout << "Usage: " << argv[0]
             << " <threads> <plantIndex{1,2,3}> [H|C]"
             << endl;
        return 1;
    }

    int threads = atoi(argv[1]);
    int plantIndex = atoi(argv[2]);
    char mode = (argc == 4 ? argv[3][0] : 'H');  // default = H

    if (threads <= 0) {
        cout << "Error: thread count must be > 0\n";
        return 1;
    }
    if (plantIndex < 1 || plantIndex > 3) {
        cout << "Error: plant index must be 1, 2, or 3\n";
        return 1;
    }
    if (mode != 'H' && mode != 'C') {
        cout << "Error: Output mode must be H or C\n";
        return 1;
    }

    omp_set_num_threads(threads);

    // Load plant
    vector<double> num, den;
    if (plantIndex == 1) getG1(num, den);
    else if (plantIndex == 2) getG2(num, den);
    else getG3(num, den);

    // BCO Settings
    BCOSettings settings;
    settings.numBees = 100;
    settings.maxIterations = 500;
    settings.limit = 30;

    settings.KpMin = -10.0;  settings.KpMax = 10.0;
    settings.KiMin = -10.0;  settings.KiMax = 10.0;
    settings.KdMin = -10.0;  settings.KdMax = 10.0;

    settings.dt = 0.001;
    settings.simTime = 40.0;

    PIDParams bestPID;
    double bestMSE = 1e9;

    // Output CSV log for optimization trace
    string logname;
    if (plantIndex == 1) logname = "data/logs/bco_G1_parallel.csv";
    else if (plantIndex == 2) logname = "data/logs/bco_G2_parallel.csv";
    else logname = "data/logs/bco_G3_parallel.csv";

    // Run BCO Optimization
    double t0 = omp_get_wtime();

    runBCOParallel(num.data(), num.size(),
                   den.data(), den.size(),
                   settings,
                   bestPID,
                   bestMSE,
                   logname.c_str());

    double t1 = omp_get_wtime();
    double elapsed = t1 - t0;

    // Output Results
    if (mode == 'H') {
        cout << "Threads         : " << threads << "\n";
        cout << "Plant           : G" << plantIndex << "\n";
        cout << "Execution Time  : " << elapsed << " seconds\n";
    }
    else { // CSV mode
        // threads, plantIndex, time
        cout << threads << ","
             << plantIndex << ","
             << elapsed << endl;
    }

    return 0;
}
