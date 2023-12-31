#ifndef __FILTER_H
#define __FILTER_H

// #include <arm_neon.h>
// void fir_filter_neon(const int16_t *input, const int16_t *kernel, int16_t *output, int length);

#include <stdint.h>
void fir_filter_neon(const float *input, const double *kernel, float *output, int length);

#endif  // __FILTER_H
