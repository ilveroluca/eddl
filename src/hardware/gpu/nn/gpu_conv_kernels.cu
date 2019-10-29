/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.1
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda.h>

#include "gpu_nn_kernels.h"
#include "../gpu_kernels.h"


__global__ void  gpu_addbias_k(float *O, int batch, int r,int c,int nk,float *bias)
{
  int size=nk*r*c;
  int thread_id_x=threadIdx.x;

  int p=blockIdx.x*size+thread_id_x*r*c;
  for (int i = 0; i < r*c; i++)
     O[p+i]+=bias[thread_id_x];

}

__global__ void  gpu_deltabias_k(float *D, int batch, int r,int c,int nk,float *bias)
{
  int size=nk*r*c;
  int thread_id_x=threadIdx.x;

  int p=blockIdx.x*size+thread_id_x*r*c;
  for (int i = 0; i < r*c; i++)
    atomicAdd(&(bias[thread_id_x]),D[p+i]);

}


__global__ void gpu_im2col_k(float* I, float *ptrI,int batch,int irows,int icols, int idepth, float* K, int nk, int kr,int kc, float* O,int orows,int ocols,int sr,int sc,int pad,int col2im)
{
  long int ops=batch*orows*ocols*kr*kc*idepth;
  long int thread_id_x = threadIdx.x+blockIdx.x*blockDim.x;


  if (thread_id_x < ops) {
    int iz,ix,iy;

    int ksize=kr*kc*idepth;

    int im=thread_id_x/(ksize*orows*ocols);
    int ioffset=im*irows*icols*idepth;


    int tx=thread_id_x%(ksize*orows*ocols);


    int r=tx/ksize;
    int c=tx%ksize;

    int oy=r/ocols;
    int ox=r%ocols;

    ix=(ox*sc)-pad;
    iy=(oy*sr)-pad;
    iz=c/(kr*kc);

    c=c%(kr*kc);

    iy+=c/kc;
    ix+=c%kc;

    if ((ix>=0)&&(ix<icols)&&(iy>=0)&&(iy<irows)) {
      int p=iz*(irows*icols)+(iy*icols)+ix;
      if (col2im)
        atomicAdd(&(I[p+ioffset]),ptrI[thread_id_x]);
      else
	ptrI[thread_id_x]=I[p+ioffset];
    }
    else
      if (!col2im)
        ptrI[thread_id_x]=0;

  }

}