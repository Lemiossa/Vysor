/*
 * fft.c
 * Created by Matheus Leme da Silva
 * */
#include <fft.h>
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef PI
#define PI 3.1415926535
#endif // PI

// Executes the recursive FFT
bool fft(const float *x, float complex *X, int N)
{
    if (!x || !X) return false;

    if (N == 1)
    {
        X[0] = x[0];
        return true;
    }

    int N2 = N/2;
    float *even = (float *)malloc(sizeof(float) * N2);
    if (!even)
        return false;

    float *odd = (float *)malloc(sizeof(float) * N2);
    if (!odd)
    {
        free(even);
        return false;
    }

    float complex *E = (float complex *)malloc(sizeof(float complex) * N2);
    if (!E)
    {
        free(even);
        free(odd);
        return false;
    }

    float complex *O = (float complex *)malloc(sizeof(float complex) * N2);
    if (!O)
    {
        free(even);
        free(odd);
        free(E);
        return false;
    }

    for (int i = 0; i < N2; i++)
    {
        even[i] = x[i * 2];
        odd[i]  = x[i * 2 + 1];
    }

    if (!fft(even, E, N2) ||
        !fft(odd, O, N2))
    {
        free(O);
        free(E);
        free(odd);
        free(even);
        return false;
    }

    for (int k = 0; k < N2; k++)
    {
        float complex W = cexpf(-I * 2 * PI * k / N);

        float complex T = W * O[k];

        X[k]       = E[k] + T;
        X[k + N/2] = E[k] - T;
    }

    free(O);
    free(E);
    free(odd);
    free(even);

    return true;
}
