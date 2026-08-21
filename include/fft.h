#ifndef FFT_H
#define FFT_H
#include <complex.h>
#include <stdbool.h>

// Executes the recursive FFT
bool fft(const float *x, float complex *X, int N);

#endif // FFT_H
