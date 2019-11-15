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
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include "net.h"
#include <pthread.h>
#include "../utils.h"
#include "../random.h"
#include "../layers/core/layer_core.h"

#define VERBOSE 0

using namespace std;
using namespace std::chrono;

/////////////////////////////////////////////////////////////////
///// NET LEVEL FUNCS
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////
void Net::do_initialize() {
    for (int i = 0; i != layers.size(); i++)
        layers[i]->initialize();
}

/////////////////////////////////////////
void Net::do_reset() {
    for (int i = 0; i != layers.size(); i++)
        layers[i]->reset();
}

void Net::do_forward() {
    for (int i = 0; i < vfts.size(); i++) {
        vfts[i]->forward();
        if (VERBOSE) {
          cout << vfts[i]->name << "\n";
          fprintf(stdout, "  %s In:%f\n", vfts[i]->name.c_str(), vfts[i]->input->sum());
          fprintf(stdout, "  %s Out:%f\n", vfts[i]->name.c_str(), vfts[i]->output->sum());
        }
    }
}

void Net::do_backward() {
    for (int i = 0; i < vbts.size(); i++) {
        vbts[i]->backward();
        if (VERBOSE) cout<<"BACK: "<<vbts[i]->name<<"delta:"<<vbts[i]->delta->sum()<<"\n";
      }

}

void Net::do_delta() {
    for (int i = 0; i < lout.size(); i++)
        losses[i]->delta(lout[i]->target, lout[i]->output, lout[i]->delta);

}

void Net::do_calcloss() {
    int p = 0;
    for (int i = 0; i < lout.size(); i++, p += 2) {
        // loss value
        fiterr[p] = losses[i]->value(lout[i]->target, lout[i]->output);
        // metric value
        fiterr[p + 1] = metrics[i]->value(lout[i]->target, lout[i]->output);
    }
}

void Net::do_applygrads() {
    optimizer->applygrads(batch_size);
}









//////
