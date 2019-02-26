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

#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "layer.h"

int reshape_created=1;

using namespace std;

LReshape::LReshape(Layer *parent):LReshape(parent,"reshape"+to_string(reshape_created),DEV_CPU){}
LReshape::LReshape(Layer *parent,string name):LReshape(parent,name,DEV_CPU){}
LReshape::LReshape(Layer *parent,int dev):LReshape(parent,"reshape"+to_string(reshape_created),dev){}
LReshape::LReshape(Layer *parent,string name,int d):LinLayer(name,d)
{

  reshape_created++;

  input=parent->output;

  ///... add code here

  parent->addchild(this);
  addparent(parent);
}


// virtual
void LReshape::forward()
{
///... add code here

}


void LReshape::backward()
{

  if (parent.size())
    {
      ///... add code here
    }
}


Layer *LReshape::share(int c,int bs,vector<Layer*>p)
{

  LReshape *n=new LReshape(p[0],"share_"+to_string(c)+name,dev);
  n->orig=this;
  n->delta_bp=delta_bp;

  return n;
}
Layer *LReshape::clone(int c,int bs,vector<Layer*>p,int todev)
{

  LReshape *n=new LReshape(p[0],"clone_"+to_string(todev)+name,todev);
  n->orig=this;
  n->delta_bp=delta_bp;

  return n;
}


void LReshape::info()
{
  cout<<"\n===============\n";
  cout<< "Layer reshape "<<name<<"\n";
  cout<< "Parent layer:"<<parent[0]->name<<"\n";
  cout<< "Child layers:\n";
  if (child.size())
    for(int i = 0; i != child.size(); i++)
      cout<< child[i]->name<<"\n";
  else cout<<"None\n";
  cout<<"LInput:\n";
  input->info();
  cout<<"No Params\n";
  cout<<"Output:\n";
  output->info();
  cout<<"===============\n\n";
}

string LReshape::plot(int c)
{
    string s;

    if (c) s=name+" [label="+"\""+name+"\",style=filled,fontsize=12,fillcolor=White,shape=box]";
    else s=name+" [label="+"\""+name+"\",style=filled,fontsize=12,fillcolor=White,shape=box]";

    return s;
}
