// The MIT License (MIT)
//
// Copyright (c) 2015-2017 CERN
//
// Author: Przemyslaw Karpinski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
//  This piece of code was developed as part of ICE-DIP project at CERN.
//  "ICE-DIP is a European Industrial Doctorate project funded by the European Community's 
//  7th Framework programme Marie Curie Actions under grant PITN-GA-2012-316596".
//
#pragma once

#include "../utilities/MeasurementHarness.h"
#include "../utilities/UMEScalarToString.h"

#include "AverageTest.h"

//#define UME_SIMD_SHOW_EMULATION_WARNINGS 1
#include <umesimd/UMESimd.h>

template<typename FLOAT_T, int SIMD_STRIDE>
class UmesimdAverageTest : public AverageTest<FLOAT_T> {
public:
    UmesimdAverageTest(int problem_size) : AverageTest<FLOAT_T>(true, problem_size) {}

    UME_NEVER_INLINE virtual void benchmarked_code()
    {
        UME::SIMD::SIMDVec<FLOAT_T, SIMD_STRIDE> x_vec;

        // Calculate loop-peeling division
        int PEEL_OFFSET = (this->problem_size/SIMD_STRIDE)*SIMD_STRIDE;

        UME::SIMD::SIMDVec<FLOAT_T, SIMD_STRIDE> sum_vec(0.0f);
        // Instead of adding single elements, we are using SIMD to add elements
        // with SIMD_STRIDE distance. We then perform reduction using scalar code
        for(int i = 0; i < PEEL_OFFSET; i+= SIMD_STRIDE)
        {
            x_vec.load(&this->x[i]);
            sum_vec.adda(x_vec);
        }

        FLOAT_T sum = sum_vec.hadd();

        // Calculating loop reminder
        for(int i = PEEL_OFFSET; i < this->problem_size; i++)
        {
            sum += this->x[i];
        }

        this->calculated_average = sum/(FLOAT_T)this->problem_size;
    }

    UME_NEVER_INLINE virtual std::string get_test_identifier() {
        std::string retval = "";

        retval += "UME::SIMD<" + ScalarToString<FLOAT_T>::value() + ", "+ std::to_string(SIMD_STRIDE) + "> " + std::to_string(this->problem_size);
        return retval;
    }
};