#ifndef __MATHLIB_H__
#define __MATHLIB_H__

/**
 * Returns a random double between min and max
 * 
 * @param min the minimum value
 * @param max the maximum value
 * @return a random double between min and max
 */
double mathlib_rand_in_range(double min, double max);

/**
 * Returns the minimum of two numbers.
 *
 * @param a The first double number
 * @param b The second double number
 * @return the smallest of a and b
 */
double mathlib_min(double a, double b);

/**
 * Returns the maximum of two numbers.
 *
 * @param a The first double number
 * @param b The second double number
 * @return the largest of a and b
 */
double mathlib_max(double a, double b);

#endif // #ifndef __MATHLIB_H__