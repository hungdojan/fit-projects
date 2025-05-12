/**
 * @file BatchMandelCalculator.cc
 * @author Hung Do <xdohun00@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date 30/10/2023
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdexcept>
#include <mm_malloc.h>

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
    BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
    data = (int *)_mm_malloc(sizeof(int) * height * width, 64);
    real = (float *)_mm_malloc(sizeof(float) * width, 64);
    imag = (float *)_mm_malloc(sizeof(float) * width, 64);
    #pragma omp simd
    for (int i = 0; i < height * width; i++) {
        data[i] = 0;
    }
}

BatchMandelCalculator::~BatchMandelCalculator() {
    _mm_free(data);
    _mm_free(real);
    _mm_free(imag);
    data = NULL;
    imag = real = NULL;
}

#pragma omp declare simd notinbranch linear(index) uniform(this) simdlen(64)
inline int BatchMandelCalculator::mandelbrot(size_t index, float real, float imag) {
    float r2 = this->real[index] * this->real[index];
    float i2 = this->imag[index] * this->imag[index];

    this->imag[index] = 2.0f * this->imag[index] * this->real[index] + imag;
    this->real[index] = r2 - i2 + real;
    return r2 + i2 <= 4.0f;
}

#define BLOCK_SIZE 64
int * BatchMandelCalculator::calculateMandelbrot () {
    int *res = data;
    for (size_t row = 0; row < height / 2; row++) {
        #pragma omp simd simdlen(64)
        for (size_t i = 0; i < width; i++) {
            this->real[i] = x_start + i * dx;
            this->imag[i] = y_start + row * dy;
        }
        int row_offset = row * width;
        int row_mirror = (height - row - 1) * width;
        for (size_t col = 0; col < width / BLOCK_SIZE; col++) {
            int col_offset = col * BLOCK_SIZE;
            for (size_t l = 0, changed=0; l < limit; l++, changed=0) {
                #pragma omp simd reduction(+:changed) simdlen(64) aligned(res: 64)
                for (size_t i = 0; i < BLOCK_SIZE; i++) {
                    const size_t curr_col = col_offset + i;
                    int value = mandelbrot(curr_col,
                            x_start + curr_col * dx,
                            y_start + row * dy);
                    res[row_offset + col_offset + i] += value;
                    changed += value;
                }
                if (!changed)
                    break;
            }
            #pragma omp simd simdlen(64)
            for (int i = 0; i < BLOCK_SIZE; i++) {
                res[row_mirror + col_offset + i] = res[row_offset + col_offset + i];
            }
        }
    }
    return res;
}
