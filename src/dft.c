/*
 * dft.c
 * Created by Matheus Leme da Silva
 * */
#include <dft.h>
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef PI
#define PI 3.1415926535
#endif // PI

// Create a DFT Context
DFTContext *dft_create(int N)
{
    if (N <= 0)
        return NULL;

    DFTContext *ctx = (DFTContext *)malloc(sizeof(DFTContext));
    if (!ctx) return NULL;
    ctx->N = N;

    ctx->weights = (float complex *)malloc(sizeof(float complex) * N * N);
    if(!ctx->weights)
    {
        free(ctx);
        return NULL;
    }

    for (int k = 0; k < N; k++)
    {
        for (int n = 0; n < N; n++)
        {
            unsigned int idx = k * N + n;
            ctx->weights[idx] = cexpf(-I * 2 * PI * k * n / N);
        }
    }

    return ctx;
}

// Executes the DFT
bool dft_execute(DFTContext *ctx, 
                 const float *x, 
                 float complex *X)
{
    if (!ctx || !x || !X) return false;

    int N = ctx->N;
    const float complex *w_ptr = ctx->weights;
    for (int k = 0; k < N; k++)
    {
        float complex sum = 0.0f;

        for (int n = 0; n < N; n++)
            sum += x[n] * (*w_ptr++);

        X[k] = sum;
    }

    return true;
}

// Destroys the DFT
void dft_destroy(DFTContext *ctx)
{
    if (!ctx)
        return;

    if (ctx->weights)
        free(ctx->weights);

    free(ctx);
}

