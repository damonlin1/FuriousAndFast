#include "mathlib.h"
#include <stdlib.h>

double mathlib_rand_in_range(double min, double max) {
    double range = max - min;
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double mathlib_min(double a, double b) {
    return a < b ? a : b;
}

double mathlib_max(double a, double b) {
    return a > b ? a : b;
}