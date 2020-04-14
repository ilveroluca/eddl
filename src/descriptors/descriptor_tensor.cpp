
/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.3
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include "eddl/descriptors/tensor_descriptors.h"
#include "eddl/utils.h"

#ifdef cGPU
#include "eddl/hardware/gpu/gpu_tensor.h"
#include "eddl/hardware/gpu/gpu_hw.h"
#endif

TensorDescriptor::TensorDescriptor(int dev){
    // Set device
    this->device = dev;  // Currently ignored

    // Initialize addresses
    cpu_addresses = nullptr;
    gpu_addresses = nullptr;
    fpga_addresses = nullptr;
}

TensorDescriptor::~TensorDescriptor() {
    this->free_memory();
}

void TensorDescriptor::free_memory() {
    if (this->cpu_addresses != nullptr) {
        delete this->cpu_addresses;
    }

#ifdef cGPU
    if (this->gpu_addresses != nullptr){
        gpu_delete_tensor_int(1000, this->gpu_addresses);  // Ugly hotfix!
      }
#endif

#ifdef cFPGA
    if (this->fpga_addresses != nullptr){
      // delete FPGA addresses
    }
#endif
}
