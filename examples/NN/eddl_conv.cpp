/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.1
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "apis/eddl.h"
#include "apis/eddlT.h"

using namespace eddl;



layer RCRC(layer l,int filters, vector<int> kernel, vector<int> stride)
{
  return ReLu(Conv(ReLu(Conv(l, filters, kernel,stride)), filters, kernel,stride));
}

layer GB(layer l)
{
  return GaussianNoise(BatchNormalization(l),0.3);
}


layer Block(layer l,int filters)
{
  return MaxPool(GB(RCRC(l,filters,{3,3},{1,1})));
}


layer ResBlock(layer l,int filters)
{
  layer in=l;

  layer l1=ReLu(Conv(l,filters,{1,1},{2,2}));
  layer l2=MaxPool(RCRC(l,filters,{3,3},{1,1}),{2,2});
  l=GB(Sum(l1,l2));
  return l;
}

int main(int argc, char **argv){
  // download CIFAR data
  download_cifar10();

  // Settings
  int epochs = 100;
  int batch_size = 100;
  int num_classes = 10;

  // network
  layer in=Input({3,32,32});
  layer l=in;
//
//    // Transformations
//    l = Shift(l, {2, 2});  // In pixels
//    l = Scale(l, {30, 30}, true); // In pixels
//    // Not implemented => l = Rotate(l, 15, false); // Degrees
//    l = Flip(l, 1);
//    l = Crop(l, {0, 0}, {23, 23}, false);  // In pixels
//    l = CropAndScale(l, {0, 0}, false);  // In pixels
//    l = Cutout(l, {0, 0}, {5, 5}); // In pixels
//
    // Data augmentation
    l = ShiftRandom(l, {-0.1f, +0.1f}, {-0.1f, +0.1f});
    l = ScaleRandom(l, {0.8f, 1.2f});
    l = FlipRandom(l, 1);
    l = CropRandom(l, {28, 28});  //In pixels  // TODO: Needs a fix!
    l = CropAndScaleRandom(l, {0.8f, 0.1f});  // Crop and rescale  // TODO: check!
    l = CutoutRandom(l, {0.1f, 0.3f}, {0.1, 0.3f});


  l=Block(l,32);
  l=ResBlock(l,64);
  l=ResBlock(l,128);
  l=ResBlock(l,256);
  l=ResBlock(l,512);

  l=Reshape(l,{-1});

  l=Activation(Dense(l,128),"relu");

  layer out=Activation(Dense(l,num_classes),"softmax");

  // net define input and output layers list
  model net=Model({in},{out});


  // Build model
  build(net,
    sgd(0.01, 0.9), // Optimizer
    {"soft_cross_entropy"}, // Losses
    {"categorical_accuracy"}, // Metrics
    //CS_CPU(4) // 4 CPU threads
    CS_CPU() // CPU with maximum threads availables
    //CS_GPU({1}) // GPU with only one gpu
  );

  // plot the model
  plot(net,"model.pdf");

  // get some info from the network
  summary(net);

  // Load and preprocess training data
  // Load dataset
  tensor x_train = eddlT::load("cifar_trX.bin");
  tensor y_train = eddlT::load("cifar_trY.bin");
  eddlT::div_(x_train, 255.0);

  tensor img=eddlT::select(x_train,0);


  eddlT::save_png(img,"img.png");

  // Load and preprocess test data
  tensor x_test = eddlT::load("cifar_tsX.bin");
  tensor y_test = eddlT::load("cifar_tsY.bin");
  eddlT::div_(x_test, 255.0);

  for(int i=0;i<epochs;i++) {
    // training, list of input and output tensors, batch, epochs
    fit(net,{x_train},{y_train},batch_size, 1);
    // Evaluate train
    std::cout << "Evaluate test:" << std::endl;
    evaluate(net,{x_test},{y_test});
  }


  // Evaluate test
  //std::cout << "Evaluate test:" << std::endl;
  //evaluate(net,{x_test},{y_test});

}


///////////
