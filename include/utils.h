#ifndef UTILS_H
#define UTILS_H

// Call once at the beginning of main() to seed the RNG
void initRandom();

// Random double in [min, max]
double randomDouble(double min, double max);

// Random integer in [min, max] (inclusive)
int randomInt(int min, int max);

// Clamp value to [min, max]
double clamp(double value, double min, double max);

#endif // UTILS_H
