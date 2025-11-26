#include "bco.h"
#include "utils.h"
#include <vector>
#include <fstream>   // for logging
#include <iostream>  
using namespace std;


// evaluate 1 bee
double evaluateBee(Bee& bee, 
                   const double* num, int numSize,
                   const double* den, int denSize,
                   double dt, double simTime)
{
    PIDParams params;
    params.Kp = bee.pid.Kp;
    params.Ki = bee.pid.Ki;
    params.Kd = bee.pid.Kd;

    PIDResult result = simulatePID(params, num, numSize, den, denSize, dt, simTime);
    return result.mse;   // fitness = MSE
}


// initialize population
void initializeBees(vector<Bee>& bees, const BCOSettings& settings)
{
    for (int i = 0; i < settings.numBees; i++) {
        Bee b;

        b.pid.Kp = randomDouble(settings.KpMin, settings.KpMax);
        b.pid.Ki = randomDouble(settings.KiMin, settings.KiMax);
        b.pid.Kd = randomDouble(settings.KdMin, settings.KdMax);

        b.fitness = 1e9;   // large number
        b.trials = 0;

        bees.push_back(b);
    }
}


// log to CSV
void logToCSV(ofstream& out, int iteration,
              const PIDParams& best, double bestMSE)
{
    out << iteration << ","
        << bestMSE << ","
        << best.Kp << ","
        << best.Ki << ","
        << best.Kd << "\n";
}


// Main BCO Algorithm
void runBCO(const double* num, int numSize,
            const double* den, int denSize,
            const BCOSettings& settings,
            PIDParams& bestParams,
            double& bestMSE,
            const char* logFilePath)
{
    initRandom();   // seed once

    // Create population
    vector<Bee> bees;
    bees.reserve(settings.numBees);
    initializeBees(bees, settings);

    // Open log file if path provided
    ofstream logFile;
    if (logFilePath != nullptr) {
        logFile.open(logFilePath);
        if (logFile.is_open()) {
            logFile << "iteration,bestMSE,Kp,Ki,Kd\n";
        }
    }


    // evaluate initial population
    for (int i = 0; i < settings.numBees; i++) {
        bees[i].fitness = evaluateBee(bees[i], num, numSize, den, denSize,
                                      settings.dt, settings.simTime);
    }

    // Find initial best
    int bestIndex = 0;
    bestMSE = bees[0].fitness;
    for (int i = 1; i < settings.numBees; i++) {
        if (bees[i].fitness < bestMSE) {
            bestMSE = bees[i].fitness;
            bestIndex = i;
        }
    }
    bestParams = bees[bestIndex].pid;


    // BCO Iterations
    for (int iter = 0; iter < settings.maxIterations; iter++) {


        // Employed Bees
        for (int i = 0; i < settings.numBees; i++) {

            // Pick another bee index k ≠ i
            int k;
            do {
                k = randomInt(0, settings.numBees - 1);
            } while (k == i);

            // φ in [-1, 1]
            double phi = randomDouble(-1.0, 1.0);

            // New candidate
            Bee candidate = bees[i];

            // Update PID parameters (local search)
            candidate.pid.Kp += phi * (candidate.pid.Kp - bees[k].pid.Kp);
            candidate.pid.Ki += phi * (candidate.pid.Ki - bees[k].pid.Ki);
            candidate.pid.Kd += phi * (candidate.pid.Kd - bees[k].pid.Kd);

            // Clamp values
            candidate.pid.Kp = clamp(candidate.pid.Kp, settings.KpMin, settings.KpMax);
            candidate.pid.Ki = clamp(candidate.pid.Ki, settings.KiMin, settings.KiMax);
            candidate.pid.Kd = clamp(candidate.pid.Kd, settings.KdMin, settings.KdMax);

            // Evaluate
            double newFit = evaluateBee(candidate, num, numSize, den, denSize,
                                        settings.dt, settings.simTime);

            // Greedy selection
            if (newFit < bees[i].fitness) {
                bees[i] = candidate;
                bees[i].fitness = newFit;
                bees[i].trials = 0;   // reset stagnation counter
            } else {
                bees[i].trials++;
            }
        }


        // Onlooker Bees
        for (int i = 0; i < settings.numBees; i++) {

            // Probability proportional to 1 / fitness (because fitness = MSE)
            double prob = 1.0 / (1.0 + bees[i].fitness);

            // Roulette wheel selection
            if (randomDouble(0, 1) < prob) {

                int k;
                do {
                    k = randomInt(0, settings.numBees - 1);
                } while (k == i);

                double phi = randomDouble(-1.0, 1.0);

                Bee candidate = bees[i];
                candidate.pid.Kp += phi * (candidate.pid.Kp - bees[k].pid.Kp);
                candidate.pid.Ki += phi * (candidate.pid.Ki - bees[k].pid.Ki);
                candidate.pid.Kd += phi * (candidate.pid.Kd - bees[k].pid.Kd);

                candidate.pid.Kp = clamp(candidate.pid.Kp, settings.KpMin, settings.KpMax);
                candidate.pid.Ki = clamp(candidate.pid.Ki, settings.KiMin, settings.KiMax);
                candidate.pid.Kd = clamp(candidate.pid.Kd, settings.KdMin, settings.KdMax);

                double newFit = evaluateBee(candidate, num, numSize, den, denSize,
                                            settings.dt, settings.simTime);

                if (newFit < bees[i].fitness) {
                    bees[i] = candidate;
                    bees[i].fitness = newFit;
                    bees[i].trials = 0;
                } else {
                    bees[i].trials++;
                }
            }
        }


        // Scout Bees
        for (int i = 0; i < settings.numBees; i++) {
            if (bees[i].trials > settings.limit) {
                bees[i].pid.Kp = randomDouble(settings.KpMin, settings.KpMax);
                bees[i].pid.Ki = randomDouble(settings.KiMin, settings.KiMax);
                bees[i].pid.Kd = randomDouble(settings.KdMin, settings.KdMax);
                bees[i].fitness = evaluateBee(bees[i], num, numSize, den, denSize,
                                              settings.dt, settings.simTime);
                bees[i].trials = 0;
            }
        }

        // Update global best
        for (int i = 0; i < settings.numBees; i++) {
            if (bees[i].fitness < bestMSE) {
                bestMSE = bees[i].fitness;
                bestParams = bees[i].pid;
            }
        }

        // Log iteration
        if (logFile.is_open()) {
            logToCSV(logFile, iter, bestParams, bestMSE);
        }

        // console output
        //cout << "Iter " << iter << " best MSE = " << bestMSE << "\n";
    }

    if (logFile.is_open()) logFile.close();
}
