/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.6
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: April 2020
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <utility>

#include "eddl/layers/core/layer_core.h"


using namespace std;

int LPermute::total_layers = 0;

/**
  @brief Computes the absolute value of a Layer

  @param l a Layer.
  @param name a name for the operation (predefined as 'abs+TotaLPermuteLayers')
  @param dev which computing service utilize

  @returns the absolute value of each element in l

  */
LPermute::LPermute(Layer *parent, vector<int> dims, string name, int dev, int mem) : LinLayer(name, dev, mem) {
    // Set default name
    if(name.empty()) this->name = "permute_" + to_string(++total_layers);

    // Set input
    input=parent->output;

    // Add batch to dims (if needed)
    vector<int> dims_batch = {0};
    for(auto &d : dims){ dims_batch.emplace_back(d + 1); }

    // Build descriptor
    sd = new PermuteDescriptor(dims_batch);
    sd->build(input->shape);

    // Set flow tensors
    output=new Tensor(sd->oshape, dev);
//    delta=new Tensor(sd->oshape, dev);

    parent->addchild(this);
    addparent(parent);
}

void LPermute::resize(int b){
    Layer::resize(b);
    sd->resize(b);
}

void LPermute::forward(){
    Tensor::select(this->input, this->output, sd);
}

void LPermute::backward(){
    Tensor::select_back(this->delta, this->parent[0]->delta, sd);
}

Layer *LPermute::share(int c, int bs, vector<Layer *> p) {
    return clone(c,bs,p,dev);
}

Layer *LPermute::clone(int c, int bs, vector<Layer *> p, int todev) {
    // Remove batch index
    vector<int> dims_batch = vector<int>(sd->dims.begin()+1, sd->dims.end());
    for(auto &d : dims_batch){ d-=1; }

    auto *n = new LPermute(p[0], dims_batch,  name, todev, this->mem_level);
    n->orig = this;
    return n;
}
