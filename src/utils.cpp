#include "utils.h"
#include <cstdlib>
#include <ctime>

static bool g_seeded = false;

void initRandom()
{
    if (!g_seeded) {
        srand(time(nullptr));
        g_seeded = true;
    }
}

double randomDouble(double min, double max)
{
    double r = (double)rand() / RAND_MAX;   // now simpler
    return min + r * (max - min);
}

int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

double clamp(double value, double min, double max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
