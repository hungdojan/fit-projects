/**
 * @file LineMandelCalculator.cc
 * @author Hung Do <xdohun00@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date 26/10/2023
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <mm_malloc.h>


#include "LineMandelCalculator.h"


LineMandelCalculator::LineMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
    BaseMandelCalculator(matrixBaseSize, limit, "LineMandelCalculator")
{
    this->data = (int *)_mm_malloc(sizeof(int) * height * width, 64);
    this->real = (float *)_mm_malloc(sizeof(float) * width, 64);
    this->imag = (float *)_mm_malloc(sizeof(float) * width, 64);
    #pragma omp simd
    for (int i = 0; i < height * width; i++) {
        data[i] = 0;
    }
}

LineMandelCalculator::~LineMandelCalculator() {
    _mm_free(this->data);
    _mm_free(this->real);
    _mm_free(this->imag);
    data = NULL;
    real = imag = NULL;
}

#pragma omp declare simd notinbranch linear(index) uniform(this) simdlen(64)
int LineMandelCalculator::mandelbrot(size_t index, float real, float imag) {
    float r2 = this->real[index] * this->real[index];
    float i2 = this->imag[index] * this->imag[index];

    this->imag[index] = 2.0f * this->imag[index] * this->real[index] + imag;
    this->real[index] = r2 - i2 + real;
    return r2 + i2 <= 4.0f;
}

int * LineMandelCalculator::calculateMandelbrot () {
    int *res = this->data;
    for (size_t row = 0; row < height / 2; row++) {
        #pragma omp simd simdlen(64)
        for (int i = 0; i < width; i++) {
            this->real[i] = x_start + i * dx;
            this->imag[i] = y_start + row * dy;
        }
        int row_offset = row * width;
        int row_mirror = (height - row - 1) * width;
        for (int i = 0, changed = 0; i < limit; i++, changed = 0) {
            #pragma omp simd reduction(+:changed) simdlen(64) aligned(res: 64)
            for (size_t col = 0; col < width; col++) {
                int value = mandelbrot(col,
                        x_start + col * dx,
                        y_start + row * dy);
                res[row_offset + col] += value;
                changed += value;
            }
            // break when nothing was changed during this iteration
            if (!changed)
                break;
        }
        #pragma omp simd simdlen(64)
        for (int col = 0; col < width; col++) {
            res[row_mirror + col] = res[row_offset + col];
        }
    }
    return res;
}
