/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.6
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: April 2020
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include "eddl/random.h"
#include "eddl/hardware/cpu/cpu_hw.h"

void
cpu_rand_uniform(Tensor * A, float v)
{
    #pragma omp parallel for
    for (int i = 0; i < A->size; ++i) A->ptr[i] = uniform() * v;
}

void
cpu_rand_signed_uniform(Tensor * A, float v)
{
    #pragma omp parallel for
    for (int i = 0; i < A->size; ++i) A->ptr[i] = signed_uniform() * v;
}

void
cpu_rand_binary(Tensor * A, float v)
{
    #pragma omp parallel for
    for (int i = 0; i < A->size; ++i)
        if (uniform() < v) A->ptr[i] = 1.0;
        else A->ptr[i] = 0.0;
}

void cpu_rand_normal(Tensor * A, float m, float s, bool fast_math) {
    int r = rand();

    if (fast_math) {
        for (int i = 0; i < A->size; ++i) A->ptr[i] = fast_randn(m, s, r++);
    } else  {
        for (int i = 0; i < A->size; ++i) A->ptr[i] = slow_randn(m, s);
    }
}
