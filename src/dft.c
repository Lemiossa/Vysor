/*
 * dft.c
 * Created by Matheus Leme da Silva
 * */
#include <complex.h>

#ifndef PI
#define PI 3.1415926535
#endif // PI

void dft(const float *x, float complex *X, int N)
{   
    /*
     *        N-1
     * X[k] = SUM ( x[n] * e^(-j * 2 * pi * k * n / N) )
     *        n=0
     * */
    for (int k = 0; k < N; k++)
    {
        X[k] = 0.0;
        for (int n = 0; n < N; n++)
        {
            float angle = (2.0 * PI * k * n) / N;
            X[k] += x[n] * cexp(-angle * I);
        }
    }
}

