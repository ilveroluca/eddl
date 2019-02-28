// This file is part of EDDLL an European Distributed Deep Learning Library.
// Developed within the DeepHealth project.
// Boosting AI in Europe.
//
// The MIT License (MIT)
//
// Copyright (c) 2019
//           Roberto Paredes Palacios, <rparedes@dsic.upv.es>
//           Jon Ander Gómez, <jon@dsic.upv.es>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef _EDDL_
#define _EDDL_

#include <initializer_list>
#include <vector>
#include "net.h"

#define tensor LTensor*
#define layer Layer*
#define model Net*
typedef vector<LTensor*> vltensor;

class EDDL {
public:
  // Create Tensors
  static tensor T(const initializer_list<int>& init);
  static tensor T(const initializer_list<int>& init, int dev);
  static tensor T(const shape s);
  static tensor T(const shape s, int dev);
  static tensor T(string fname);

  //Operations tensors
  static void div(tensor t,float v);

  // Create Layers
  static layer Input(tensor t);
  static layer Input(tensor t,int dev);
  static layer Input(const initializer_list<int>& init);
  static layer Input(const initializer_list<int>& init,int dev);

  static layer Dense(layer parent,int dim);
  static layer Dense(layer parent,int dim,int d);
  static layer Dense(layer parent,int dim,string name);
  static layer Dense(layer parent,int dim,string name,int d);

  static layer Activation(layer parent,string act);
  static layer Activation(layer parent,string act,string name);
  static layer Activation(layer parent,string act,int d);
  static layer Activation(layer parent,string act,string name,int d);

  static layer Reshape(layer parent, const initializer_list<int>& init);
  static layer Reshape(layer parent, const initializer_list<int>& init,string name);
  static layer Reshape(layer parent, const initializer_list<int>& init,int d);
  static layer Reshape(layer parent, const initializer_list<int>& init,string name,int d);
  // Create net
  static model Model(vlayer in,vlayer out);
  // Net operations
  static void plot(model m,string fname);
  static void info(model m);
  static void build(model net,optim *opt,const initializer_list<string>& c,const initializer_list<string>& m);
  static void build(model net,optim *opt,const initializer_list<string>& c,const initializer_list<string>& m,int todev);
  static void fit(model m, const initializer_list<LTensor*>& in,const initializer_list<LTensor*>& out,int batch,int epochs);


};

extern EDDL eddl;

#endif