#ifndef DFT_H
#define DFT_H
#include <complex.h>
#include <stdbool.h>

typedef struct 
{
    int N;
    float complex *weights;
} DFTContext;

// Create a DFT Context
DFTContext *dft_create(int N);
// Executes the DFT
bool dft_execute(DFTContext *ctx,
                    const float *x, 
                    float complex *X);
// Destroys the DFT
void dft_destroy(DFTContext *ctx);

#endif // DFT_H
