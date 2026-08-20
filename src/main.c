/*
 * main.c
 * Created by Matheus Leme da Silva
 * */
#include <stdio.h>
#include <complex.h>

#ifndef PI
#define PI 3.1415926535
#endif // PI

void dft(const double *x, double complex *X, int N)
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
            double angle = (2.0 * PI * k * n) / N;
            X[k] += x[n] * cexp(-angle * I);
        }
    }
}

int main(void)
{
    double samples[8] = {
         1.0000,
         0.7071,
         0.0000,
        -0.7071,
        -1.0000,
        -0.7071,
         0.0000,
         0.7071
    };

    double complex buffer[8];
    
    dft(samples, buffer, sizeof(samples) / sizeof(double));
    
    for (int k = 0; k < 8; k++)
    {
        double mag = cabs(buffer[k]);
        printf("%lf\r\n", mag);
    }

    printf("Hello World\r\n");
    return 0;
}
