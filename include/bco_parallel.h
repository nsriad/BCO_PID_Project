#ifndef BCO_PARALLEL_H
#define BCO_PARALLEL_H

#include "bco.h"  // reuse BCOSettings, Bee, PIDParams

// Parallel BCO using OpenMP.
// Same interface as runBCO, but with internal parallel loops.
void runBCOParallel(const double* num, int numSize,
                    const double* den, int denSize,
                    const BCOSettings& settings,
                    PIDParams& bestParams,
                    double& bestMSE,
                    const char* logFilePath);

#endif // BCO_PARALLEL_H
