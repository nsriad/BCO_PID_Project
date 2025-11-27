#include "utils.h"
#include <random>
#include <omp.h>

using namespace std;

// Thread-local RNG and state
thread_local mt19937 rng;
thread_local bool rng_ready = false;

// Global base seed (set in initRandom)
static int GLOBAL_SEED = 12345;

// -----------------------------------------------------
// Initialize rng for calling thread only
// Should be called once per thread
// -----------------------------------------------------
void initRandom(int seed)
{
    GLOBAL_SEED = seed;
    rng.seed(GLOBAL_SEED + omp_get_thread_num());
    rng_ready = true;
}


// Ensure RNG is ready for this thread
inline void ensure_rng()
{
    if (!rng_ready) {
        rng.seed(GLOBAL_SEED + omp_get_thread_num());
        rng_ready = true;
    }
}

// Random double in [min, max]
double randomDouble(double min, double max)
{
    ensure_rng();
    uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

// Random integer in [min, max]
int randomInt(int min, int max)
{
    ensure_rng();
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Clamp values
double clamp(double x, double minVal, double maxVal)
{
    if (x < minVal) return minVal;
    if (x > maxVal) return maxVal;
    return x;
}
