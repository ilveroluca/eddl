/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.6
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: April 2020
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/

#ifndef EDDL_TENSOR_H
#define EDDL_TENSOR_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <mutex>

#include "Eigen/Dense"

#include "eddl/utils.h"
#include "eddl/descriptors/tensor_descriptors.h"

// Read/Write Numpy
#include "eddl/tensor/cnpy/cnpy.h"

#define DEV_CPU 0

#define DEV_GPU 1000
#define DEV_GPU_0 1000
#define DEV_GPU_1 1001
#define DEV_GPU_2 1002
#define DEV_GPU_3 1003
#define DEV_GPU_4 1004
#define DEV_GPU_5 1005
#define DEV_GPU_6 1006
#define DEV_GPU_7 1007
#define DEV_GPU_8 1008

#define DEV_FPGA 2000
#define DEV_FPGA_0 2000
#define DEV_FPGA_1 2001
#define DEV_FPGA_2 2002
#define DEV_FPGA_3 2003
#define DEV_FPGA_4 2004
#define DEV_FPGA_5 2005
#define DEV_FPGA_6 2006
#define DEV_FPGA_7 2007
#define DEV_FPGA_8 2008

#define MAX_GPUS 8

using namespace std;

// TODO: Remove this. Don't like here
typedef Eigen::Matrix<float, -1, -1, Eigen::RowMajor> MatrixXRMf;
typedef vector<int> tshape;

class Tensor {
private:
    void updateDevice(int dev);
    void updateShape(const vector<int> &shape);
    void updateSize();
    void updateStrides();
    void updateData(float* ptr);

    // Load methods
    static Tensor* load_from_bin(std::ifstream &ifs);
    static Tensor* load_from_onnx(std::ifstream &ifs);
    static Tensor* load_from_img(const string &filename, const string &format);
    template<typename T> static Tensor* load_from_numpy(const string &filename, const string &format);
    static Tensor* load_from_txt(std::ifstream &ifs, char delimiter, int headerRows);

    // Save methods
    void save2bin(std::ofstream &ofs);
    void save2onnx(std::ofstream &ofs);
    void save2img(const string &filename, string format);
    void save2numpy(const string &filename, string format);
    void save2txt(std::ofstream &ofs, const char delimiter, const vector<string> &header);

public:
    int device;
    int ndim;
    long int size;
    vector<int> shape;
    vector<int> stride;

    // Data pointers
    float *ptr;
    Eigen::MatrixXf *ptr2;  // TODO: I don't like it. float or eigen, not both

    // Aux variables
    int gpu_device;
    mutex *tsem;  // Multithreading. Tensor semaphore

    // Constructors
    Tensor();
    explicit Tensor(const vector<int> &shape, int dev=DEV_CPU);
    Tensor(const vector<int> &shape, float *fptr, int dev=DEV_CPU);
    Tensor(const vector<int> &shape, Tensor *T);

    // Destructors
    ~Tensor();

    // Copy data
    /**
      *  @brief Clone a tensor to the CPU.
    */
    void toCPU(int dev=DEV_CPU);

    /**
      *  @brief Clone a tensor to the GPU.
    */
    void toGPU(int dev=DEV_GPU);

    /**
      *  @brief Clone a tensor (same device).
      *
      *  @return    Tensor
    */
    Tensor* clone();
    void deleteData();
    void reallocate(Tensor* old_t, vector<int> *s = nullptr);

    // Resize
    void resize(int b, float *fptr);
    void resize(int b);
    void resize(int b, Tensor *T);

    // Check device
    /**
      *  @brief Check if the tensor is in CPU.
      *
      *  @return int
    */
    int isCPU();

    /**
      *  @brief Check if the tensor is in GPU.
      *
      *  @return int
    */
    int isGPU();

    /**
      *  @brief Check if the tensor is in FPGA.
      *
      *  @return int
    */
    int isFPGA();

    // View methods

    /**
      *  @brief Print shape, device and size information.
      *
      *  @return    void
    */
    void info();

    /**
      *  @brief Print the tensor values.
      *
      *  @return    void
    */
    void print(int precision=6, bool raw=false);
    string getStrDevice();

    // Core
    vector<int> getShape();
    static int get_mode(string mode);  // TODO: Delete?

    /**
      *  @brief Check if all dimensions in the tensor are the same.
      *
      *  @param A   Tensor
      *  @return    bool
    */
    static bool isSquared(Tensor *A);


    // Serialization *****************************
    /**
      *  @brief Load tensor from filestream.
      *
      *  @param ifs  Filestream
      *  @param format    File format. Accepted formats are: bin, onnx, csv, tsv, txt.
      *  @return    Tensor
    */
    static Tensor* loadfs(std::ifstream &ifs, string format="");

    /**
      *  @brief Load tensor from file.
      *
      *  @param filename  Name of the file to load the tensor from.
      *  @param format    Filetype. The accepted filetypes are the following:
      *                     - Images: jpg, jpeg, png, bmp, hdr, psd, tga, gif, pic, pgm, ppm.
      *                     - Numpy: npy, npz
      *                     - Other: bin, onnx
      *  @return    Tensor
    */
    static Tensor* load(const string& filename, string format="");
    template<typename T> static Tensor* load(const string& filename, string format="");

    /**
      *  @brief Load data from a text file
      *
      *  @param filename  Name of the file to load the tensor from.
      *  @param delimiter    Character used to separate the columns of the file.
      *  @param headerRows   Number of top rows to avoid, generally because they correspond to the header.
      *  @return    Tensor
    */
    static Tensor* load_from_txt(const string& filename, const char delimiter=',', int headerRows=1);

    /**
      *  @brief Save tensor to a filestream.
      *
      *  @param ofs     Filestream.
      *  @param format    Format to use. The accepted formats are the following:
      *                     - Text: csv, tsv, txt
      *                     - Other: bin, onnx
      *  @return    void
    */
    void savefs(std::ofstream &ofs, string format="");

    /**
      *  @brief Save tensor to a file.
      *
      *  @param filename    Name of the file to save the tensor to.
      *  @param format    Filetype. The accepted filetypes are the following:
      *                     - Images: png, bmp, tga, jpg, jpeg, hdr.
      *                     - Numpy: npy, npz
      *                     - Text: csv, tsv, txt
      *                     - Other: bin, onnx
      *  @return    void
    */
    void save(const string& filename, string format="");

    /**
      *  @brief Save tensor to a text file.
      *
      *  @param filename    Name of the file to save the tensor to.
      *  @param delimiter   Character to use to separate the columns of the file.
      *  @param header      Header rows.
      *  @return    void
    */
    void save2txt(const string& filename, const char delimiter=',', const vector<string> &header={});


    // ***** Core *****************************
    static Tensor* permute(Tensor* t, const vector<int>& dims);
    static Tensor* moveaxis(Tensor* t, int source, int destination);
    static Tensor* swapaxis(Tensor* t, int axis1, int axis2);

    void fill_(float v);
//    static Tensor* fill(Tensor *A, float v);

    void reshape_(const vector<int> &new_shape);
    static Tensor* reshape(Tensor *A, const vector<int> &shape);
    static Tensor* flatten(Tensor *A);

    void squeeze_();
    static Tensor* squeeze(Tensor *A);

    void unsqueeze_();
    static Tensor* unsqueeze(Tensor *A);

    /**
      *  @brief Check if the given indices are valid for this tensor.
      *
      *  @param indices
      *  @return    bool
    */
    bool valid_indices(vector<int> indices);

    /**
      *  @brief Translate a set of indices to their corresponding address (row-major).
      *
      *  @param indices
      *  @return    int
    */
    int get_address_rowmajor(vector<int> indices);
    vector<int> get_indices_rowmajor(int address);
    float get_(vector<int> indices);
    void set_(vector<int> indices, float value);


    // ************************************************
    // ****** Tensor operations ***********************
    // ************************************************
    // Creation ops ***********************************

    /**
      *  @brief Create a tensor of the specified shape and fill it with zeros.
      *
      *  @param shape  Shape of the tensor to create.
      *  @param dev    Device to use. The possible values are: ``DEV_CPU`` and ``DEV_GPU``.
      *  @return     Tensor of the specified shape filled with zeros
    */
    static Tensor* zeros(const vector<int> &shape, int dev=DEV_CPU);

    /**
      *  @brief Create a tensor of the specified shape and fill it with ones.
      *
      *  @param shape  Shape of the tensor to create.
      *  @param dev    Device to use. The possible values are: ``DEV_CPU`` and ``DEV_GPU``.
      *  @return     Tensor of the specified shape filled with ones
    */
    static Tensor* ones(const vector<int> &shape, int dev=DEV_CPU);

    /**
      *  @brief Create a tensor of the specified shape and fill it with a specific value.
      *
      *  @param shape  Shape of the tensor to create.
      *  @param value  Value to use to fill the tensor.
      *  @param dev    Device to use. The possible values are: ``DEV_CPU`` and ``DEV_GPU``.
      *  @return     Tensor of the specified shape filled with the value
    */
    static Tensor* full(const vector<int> &shape, float value, int dev=DEV_CPU);

    static Tensor* arange(float start, float end, float step=1.0f, int dev=DEV_CPU);
    static Tensor* range(float start, float end, float step=1.0f, int dev=DEV_CPU);
    static Tensor* linspace(float start, float end, int steps=100, int dev=DEV_CPU);
    static Tensor* logspace(float start, float end, int steps=100, float base=10.0f, int dev=DEV_CPU);
    static Tensor* geomspace(float start, float end, int steps=100, int dev=DEV_CPU);

    /**
      *  @brief
      *
      *  @param rows  Number of rows of the tensor.
      *  @param offset
      *  @param dev    Device to use. The possible values are: ``DEV_CPU`` and ``DEV_GPU``.
      *  @return     Tensor of the specified shape filled with the value
    */
    static Tensor* eye(int rows, int offset=0, int dev=DEV_CPU);

    /**
      *  @brief Create a tensor representing the identity matrix. Equivalent to calling function ``eye`` with ``offset = 0``.
      *
      *  @param shape  Shape of the tensor to create.
      *  @param value  Value to use to fill the tensor.
      *  @param dev    Device to use. The possible values are: ``DEV_CPU`` and ``DEV_GPU``.
      *  @return     Tensor of the specified shape filled with the value
    */
    static Tensor* identity(int rows, int dev=DEV_CPU);
    static Tensor* diag(Tensor* A, int k=0, int dev=DEV_CPU);
    static Tensor* randu(const vector<int> &shape, int dev=DEV_CPU);
    static Tensor* randn(const vector<int> &shape, int dev=DEV_CPU);

    // ***** Transformations *****************************
    static void shift(Tensor *A,Tensor *B, vector<int> shift, string mode="constant", float constant=0.0f);
    static void rotate(Tensor *A, Tensor *B, float angle, vector<int> offset_center={0,0}, string mode="constant", float constant=0.0f);
    static void scale(Tensor *A, Tensor *B, vector<int> new_shape, string mode="nearest", float constant=0.0f);
    static void flip(Tensor *A, Tensor *B, int axis=0);
    static void crop(Tensor *A, Tensor *B, vector<int> coords_from, vector<int> coords_to, float constant=0.0f);
    static void crop_scale(Tensor *A, Tensor *B, vector<int> coords_from, vector<int> coords_to, string mode="nearest", float constant=0.0f);
    static void cutout(Tensor *A, Tensor *B, vector<int> coords_from, vector<int> coords_to, float constant=0.0f);

    // ***** Data augmentation *****************************
    static void shift_random(Tensor *A,Tensor *B, vector<float> factor_x, vector<float> factor_y, string mode="constant", float constant=0.0f);
    static void rotate_random(Tensor *A, Tensor *B, vector<float> factor, vector<int> offset_center={0,0}, string mode="constant", float constant=0.0f);
    static void scale_random(Tensor *A, Tensor *B, vector<float> factor, string mode="nearest", float constant=0.0f);
    static void flip_random(Tensor *A, Tensor *B, int axis);

    static void crop_random(Tensor *A, Tensor *B);
    static void crop_scale_random(Tensor *A, Tensor *B, vector<float> factor, string mode="nearest", float constant=0.0f);
    static void cutout_random(Tensor *A, Tensor *B, vector<float> factor_x, vector<float> factor_y, float constant=0.0f);

    // Math operations ********************************
    static Tensor* interpolate(float factor1, Tensor *A, float factor2, Tensor *B);

    // Math operations: Pointwise ops (in-place)
    void abs_();
    static Tensor* abs(Tensor *A);

    void acos_();
    static Tensor* acos(Tensor *A);

    void add_(float v);
    void add_(Tensor *A);
    static Tensor* add(Tensor *A, Tensor *B);
    static void add(float scA, Tensor *A, float scB, Tensor *B, Tensor *C, int incC);
    static void add(Tensor *A, Tensor *B, Tensor *C);
    static void inc(Tensor *A, Tensor *B);

    void asin_();
    static Tensor* asin(Tensor *A);

    void atan_();
    static Tensor* atan(Tensor *A);

    void ceil_();
    static Tensor* ceil(Tensor *A);

    void clamp_(float min, float max);
    static Tensor* clamp(Tensor *A, float min, float max);

    void clampmax_(float max);
    static Tensor* clampmax(Tensor *A, float max);

    void clampmin_(float min);
    static Tensor* clampmin(Tensor *A, float min);

    void cos_();
    static Tensor* cos(Tensor *A);

    void cosh_();
    static Tensor* cosh(Tensor *A);


    void div_(float v);
    static Tensor* div(Tensor *A, float v);
    static Tensor* div(Tensor *A, Tensor *B);
    static void el_div(Tensor *A, Tensor *B, Tensor *C, int incC);

    void inv_(float v=1.0f);

    void exp_();
    static Tensor* exp(Tensor *A);

    void floor_();
    static Tensor* floor(Tensor *A);

    void log_();
    static Tensor* log(Tensor *A);

    void log2_();
    static Tensor* log2(Tensor *A);

    void log10_();
    static Tensor* log10(Tensor *A);

    void logn_(float n);
    static Tensor* logn(Tensor *A, float n);

    float max();
    float min();

    void mod_(float v);
    static Tensor* mod(Tensor *A, float v);

    void mult_(float v);
    static Tensor* mult(Tensor *A, float v);
    static Tensor* mult(Tensor *A, Tensor *B);
    static void mult2D(Tensor *A, int tA, Tensor *B, int tB, Tensor *C, int incC);
    static void el_mult(Tensor *A, Tensor *B, Tensor *C, int incC);

    void neg_();
    static Tensor* neg(Tensor *A);

    void normalize_(float min=0.0f, float max=1.0f);
    static Tensor* normalize(Tensor *A, float min=0.0f, float max=1.0f);

    void pow_(float exp);
    static Tensor* pow(Tensor *A, float exp);

    void powb_(float base);
    static Tensor* powb(Tensor *A, float base);

    void reciprocal_();
    static Tensor* reciprocal(Tensor *A);

    void remainder_(float v);
    static Tensor* remainder(Tensor *A, float v);

    void round_();
    static Tensor* round(Tensor *A);

    void rsqrt_();
    static Tensor* rsqrt(Tensor *A);

    void sigmoid_();
    static Tensor* sigmoid(Tensor *A);

    void sign_();
    static Tensor* sign(Tensor *A);
    static void sign(Tensor *A, Tensor *B);

    void sin_();
    static Tensor* sin(Tensor *A);

    void sinh_();
    static Tensor* sinh(Tensor *A);

    void sqr_();
    static Tensor* sqr(Tensor *A);

    void sqrt_();
    static Tensor* sqrt(Tensor *A);

    void sub_(float v);
    static Tensor* sub(Tensor *A, Tensor *B);

    float sum();
//    static Tensor* sum(Tensor *A);
    static void sum2D_rowwise(Tensor *A, Tensor *B, Tensor *C);
    static void sum2D_colwise(Tensor *A, Tensor *B, Tensor *C);

    float sum_abs();
    static Tensor* sum_abs(Tensor *A);

    void tan_();
    static Tensor* tan(Tensor *A);

    void tanh_();
    static Tensor* tanh(Tensor *A);

    void trunc_();
    static Tensor* trunc(Tensor *A);

    // Math operations: Reduction ops
    static void reduce_sum2D(Tensor *A, Tensor *B, int axis, int incB);

    // Logic funcions: Truth value testing

    /**
      *  @brief Test whether all elements evaluate to True.
      *
      *  @param A   Tensor to evaluate
      *  @return    bool
    */
    static bool all(Tensor *A);

    /**
      *  @brief Test whether any element evaluates to True.
      *
      *  @param A   Tensor to evaluate
      *  @return    bool
    */
    static bool any(Tensor *A);

    // Logic funcions: Logical ops

    /**
      *  @brief Test element-wise for finiteness (not infinity or not Not a Number).
      *
      *  @param A   Tensor to evaluate
      *  @param B   Tensor to store the results of the test as booleans
      *  @return    void
    */
    static void isfinite(Tensor *A, Tensor* B);

    /**
      *  @brief Test element-wise for positive or negative infinity.
      *
      *  @param A   Tensor to evaluate
      *  @param B   Tensor to store the results of the test as booleans
      *  @return    void
    */
    static void isinf(Tensor *A, Tensor* B);

    /**
      *  @brief Test element-wise for Nan.
      *
      *  @param A   Tensor to evaluate
      *  @param B   Tensor to store the results of the test as booleans
      *  @return    void
    */
    static void isnan(Tensor *A, Tensor* B);

    /**
      *  @brief Test element-wise for negative infinity.
      *
      *  @param A   Tensor to evaluate
      *  @param B   Tensor to store the results of the test as booleans
      *  @return    void
    */
    static void isneginf(Tensor *A, Tensor* B);

    /**
      *  @brief Test element-wise for positive infinity.
      *
      *  @param A   Tensor to evaluate
      *  @param B   Tensor to store the results of the test as booleans
      *  @return    void
    */
    static void isposinf(Tensor *A, Tensor* B);

    // Logic funcions: Logical ops

    /**
      *  @brief Compute the truth value of ``A and B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor to store the results of the operation
      *  @return    void
    */
    static void logical_and(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Compute the truth value of ``A or B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor to store the results of the operation
      *  @return    void
    */
    static void logical_or(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Compute the truth value of ``not A`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor to store the results of the operation
      *  @return    void
    */
    static void logical_not(Tensor *A, Tensor *B);

    /**
      *  @brief Compute the truth value of ``A xor B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor to store the results of the operation
      *  @return    void
    */
    static void logical_xor(Tensor *A, Tensor *B, Tensor *C);

    // Logic funcions: Comparison ops

    /**
      *  @brief Returns True if two arrays are element-wise equal within a tolerance.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param rtol
      *  @param atol
      *  @param equal_nan
      *  @return    void
    */
    static bool allclose(Tensor *A, Tensor *B, float rtol=1e-05, float atol=1e-08, bool equal_nan=false);  // Returns true or false

    /**
      *  @brief Returns a boolean array where two arrays are element-wise equal within a tolerance.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor
      *  @param rtol
      *  @param atol
      *  @param equal_nan
      *  @return    void
    */
    static void isclose(Tensor *A, Tensor *B, Tensor *C, float rtol=1e-05, float atol=1e-08, bool equal_nan=false);  // Returns a boolean tensor

    /**
      *  @brief Return the truth value of ``A > B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void greater(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Return the truth value of ``A >= B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void greater_equal(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Return the truth value of ``A < B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void less(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Return the truth value of ``A <= B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void less_equal(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Return the truth value of ``A == B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void equal(Tensor *A, Tensor *B, Tensor *C);

    /**
      *  @brief Return the truth value of ``A != B`` element-wise.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @param C   Tensor store the results of the operation.
      *  @return    void
    */
    static void not_equal(Tensor *A, Tensor *B, Tensor *C);

    // Legacy
    static int eqsize(Tensor *A, Tensor *B);
    static int equal2(Tensor *A, Tensor *B, float epsilon=1e-3);

    // Math operations: Other ops
    static int cross(Tensor *A, Tensor *B); // TODO
    static int diag(Tensor *A); // TODO
    static int einsum(string subscripts, Tensor *A); // TODO
    static int flip(Tensor *A);  // TODO
    static int trace(Tensor *A);
    static int dot(Tensor *A);  // TODO

    // Indexing, Slicing, Joining, Mutating Ops *******
    static Tensor* concat(const vector<Tensor*> t, unsigned int axis=0, Tensor* output=nullptr);
    static void concat_back(Tensor *A, const vector<Tensor*> t, unsigned int axis);

    /**
      *  @brief Returns an array with the selected indices of the tensor.
      *
      *  @param indices  Vector of strings representing the indices to be selected. These indices must follow a Python-like syntax. Some examples: ``"0"`` , ``":5"`` , ``":"`` , ``"3:6"``.
      *  @return     Tensor
    */
    Tensor* select(const vector<string>& indices);
    static void select(Tensor *A, Tensor *B, SelDescriptor *sd);

    /**
      *  @brief Sets the elements in the array using the selected indices. The indices must be specified as a vector of strings ({“0”, “:5”, “:”, “3:6”}).
      *
      *  @param indices  Vector of strings representing the indices to be selected. These indices must follow a Python-like syntax. Some examples: ``"0"``, ``":5"``, ``":"``, ``"3:6"``.
      *  @param A
      *  @return     void
    */
    void set_select(const vector<string>& indices, Tensor *A);
    static void set_select(Tensor *A, Tensor *B, SelDescriptor *sd);
    static void set_select_back(Tensor *A, Tensor *B, SelDescriptor *sd);

    static void transpose(Tensor *A, Tensor *B, vector<int> dims);

    /**
      *  @brief Copy data from tensor A to B.
      *
      *  @param A   Tensor
      *  @param B   Tensor
      *  @return    void
    */
    static void copy(Tensor *A, Tensor *B);
    static void fill(Tensor *A, int aini, int aend, Tensor *B, int bini, int bend, int inc);
    static void select_back(Tensor *A, Tensor *B, SelDescriptor *sd);
    static void select(Tensor *A, Tensor *B, vector<int> sind, int ini, int end, bool mask_zeros=false);
    static void deselect(Tensor *A, Tensor *B, vector<int> sind, int ini, int end,int inc=0, bool mask_zeros=false);
    static void tile(Tensor *A, Tensor *B);

    // Generators (In-place) *************************************
    // Rethink names
    void rand_bernoulli(); // Todo
    void rand_multinomial(); // Todo
    void rand_uniform(float v);
    void rand_signed_uniform(float v);
    void rand_normal(float m, float s, bool fast_math=true);
    void rand_binary(float v);

    // ***** Overload operators *****************************
    // Tensor and Tensor (Element wise)
    friend Tensor& operator+ (Tensor &A, Tensor &B);
    friend Tensor& operator- (Tensor &A, Tensor &B);
    friend Tensor& operator* (Tensor &A, Tensor &B);
    friend Tensor& operator/ (Tensor &A, Tensor &B);

    // Tensor op= Tensor
    friend void operator+= (Tensor &A, Tensor &B);
    friend void operator-= (Tensor &A, Tensor &B);
    friend void operator*= (Tensor &A, Tensor &B);
    friend void operator/= (Tensor &A, Tensor &B);

    // Tensor op= scalar
    friend void operator+= (Tensor &A, float v);
    friend void operator-= (Tensor &A, float v);
    friend void operator*= (Tensor &A, float v);
    friend void operator/= (Tensor &A, float v);

    // Tensor and scalar
    friend Tensor& operator+ (Tensor &A, float v);
    friend Tensor& operator- (Tensor &A, float v);
    friend Tensor& operator* (Tensor &A, float v);
    friend Tensor& operator/ (Tensor &A, float v);

    // scalar and Tensor
    friend Tensor& operator+ (float v, Tensor &A);
    friend Tensor& operator- (float v, Tensor &A);
    friend Tensor& operator* (float v, Tensor &A);
    friend Tensor& operator/ (float v, Tensor &A);


};



template<typename T>
Tensor* Tensor::load_from_numpy(const string &filename, const string &format){
    Tensor* t = nullptr;

    cnpy::NpyArray arr = cnpy::npy_load(filename);
    auto* loaded_data = arr.data<T>();

    // Get shape
    vector<int> arr_shape;
    for(unsigned long i : arr.shape){
        arr_shape.push_back(i);
    }

    // Initialize tensor
    t = new Tensor(arr_shape, DEV_CPU);

    // Fill tensor
    for(int i=0; i<arr.num_vals; i++){
        t->ptr[i] = (float)loaded_data[i];
    }

    return t;
}

template<typename T>
Tensor* Tensor::load(const string& filename, string format){
    // Infer format from filename
    if(format.empty()){
        format = get_extension(filename);
    }

    // Check if file exists (open file stream)
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary);
    if (!ifs.good()){
        msg("File not found. Check the file name and try again.", "Tensor::load");
    }
    // Load tensor
    Tensor* t;
    if(format=="jpg" || format=="jpeg" || format=="png" || format=="bmp" ||
       format=="hdr" || format=="psd" || format=="tga" || format=="gif" ||
       format=="pic"  || format=="pgm"  || format=="ppm") { // Images
        t = Tensor::load_from_img(filename, format);
    }else if(format=="bin" || format=="onnx"){
        t = Tensor::loadfs(ifs, format);
    }else if(format=="npy" || format=="npz"){
        t = Tensor::load_from_numpy<T>(filename, format);
    }else if(format=="csv" || format=="tsv" || format=="txt"){
        t = Tensor::loadfs(ifs, format);
    }else{
        msg("Format not implemented: *.'" + format + "'", "Tensor::load");
    }

    // Close file stream and return tensor
    ifs.close();
    return t;
}

#endif //EDDL_TENSOR_H
