#include "bco_parallel.h"
#include "utils.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <omp.h>
using namespace std;


// evaluate one bee
double evaluateBeeParallel(Bee& bee,
                           const double* num, int numSize,
                           const double* den, int denSize,
                           double dt, double simTime)
{
    PIDParams params = bee.pid;
    PIDResult result = simulatePID(params, num, numSize, den, denSize, dt, simTime);
    return result.mse;
}


// initialize bees

void initializeBeesParallel(vector<Bee>& bees, const BCOSettings& settings)
{
    for (int i = 0; i < settings.numBees; i++) {
        Bee b;
        b.pid.Kp = randomDouble(settings.KpMin, settings.KpMax);
        b.pid.Ki = randomDouble(settings.KiMin, settings.KiMax);
        b.pid.Kd = randomDouble(settings.KdMin, settings.KdMax);
        b.fitness = 1e9;
        b.trials  = 0;
        bees.push_back(b);
    }
}


//logging helper
void logToCSVParallel(ofstream& out, int iteration,
                      const PIDParams& best, double bestMSE)
{
    out << iteration << ","
        << bestMSE << ","
        << best.Kp << ","
        << best.Ki << ","
        << best.Kd << "\n";
}


// parallel BCO
void runBCOParallel(const double* num, int numSize,
                    const double* den, int denSize,
                    const BCOSettings& settings,
                    PIDParams& bestParams,
                    double& bestMSE,
                    const char* logFilePath)
{
    initRandom(12345);

    vector<Bee> bees;
    bees.reserve(settings.numBees);
    initializeBeesParallel(bees, settings);

    ofstream logFile;
    if (logFilePath != nullptr) {
        logFile.open(logFilePath);
        if (logFile.is_open()) {
            logFile << "iteration,bestMSE,Kp,Ki,Kd\n";
        }
    }


    // parallel initial evaluation
    #pragma omp parallel for
    for (int i = 0; i < settings.numBees; i++) {
        bees[i].fitness = evaluateBeeParallel(bees[i],
                                              num, numSize,
                                              den, denSize,
                                              settings.dt,
                                              settings.simTime);
    }

    // Find initial best
    bestMSE = 1e9;
    int bestIndex = 0;

    #pragma omp parallel for reduction(min:bestMSE)
    for (int i = 0; i < settings.numBees; i++) {
        if (bees[i].fitness < bestMSE) {
            bestMSE = bees[i].fitness;
            bestIndex = i;
        }
    }
    bestParams = bees[bestIndex].pid;


    // main BCO loop
    for (int iter = 0; iter < settings.maxIterations; iter++) {
        // 1. employed bees phase
        #pragma omp parallel for
        for (int i = 0; i < settings.numBees; i++) {
            // random partner index
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

            double newFit = evaluateBeeParallel(candidate,
                                                num, numSize,
                                                den, denSize,
                                                settings.dt,
                                                settings.simTime);

            if (newFit < bees[i].fitness) {
                bees[i] = candidate;
                bees[i].fitness = newFit;
                bees[i].trials = 0;
            } else {
                bees[i].trials++;
            }
        }


        // 2) parallel onlooker bees phase
        #pragma omp parallel for
        for (int i = 0; i < settings.numBees; i++) {

            double prob = 1.0 / (1.0 + bees[i].fitness);
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

                double newFit = evaluateBeeParallel(candidate,
                                                    num, numSize,
                                                    den, denSize,
                                                    settings.dt,
                                                    settings.simTime);

                if (newFit < bees[i].fitness) {
                    bees[i] = candidate;
                    bees[i].fitness = newFit;
                    bees[i].trials = 0;
                } else {
                    bees[i].trials++;
                }
            }
        }


        // 3) scout bees phase
        #pragma omp parallel for
        for (int i = 0; i < settings.numBees; i++) {
            if (bees[i].trials > settings.limit) {
                bees[i].pid.Kp = randomDouble(settings.KpMin, settings.KpMax);
                bees[i].pid.Ki = randomDouble(settings.KiMin, settings.KiMax);
                bees[i].pid.Kd = randomDouble(settings.KdMin, settings.KdMax);

                bees[i].fitness = evaluateBeeParallel(bees[i],
                                                      num, numSize,
                                                      den, denSize,
                                                      settings.dt,
                                                      settings.simTime);
                bees[i].trials = 0;
            }
        }

        // 4) global best update
        double newBest = bestMSE;
        int newBestIndex = bestIndex;

        #pragma omp parallel for reduction(min:newBest)
        for (int i = 0; i < settings.numBees; i++) {
            if (bees[i].fitness < newBest) {
                newBest = bees[i].fitness;
                newBestIndex = i;
            }
        }

        bestMSE = newBest;
        bestParams = bees[newBestIndex].pid;

        if (logFile.is_open()) {
            logToCSVParallel(logFile, iter, bestParams, bestMSE);
        }
    }

    if (logFile.is_open()) logFile.close();
}
