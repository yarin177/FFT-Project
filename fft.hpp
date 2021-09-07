#define _USE_MATH_DEFINES
#pragma once
#include <cmath>
#include <vector>
#include <complex>
#include <valarray>


using std::vector;
using std::complex;
 
vector<complex<float>> slicing(vector<complex<float>> vec, unsigned int X, unsigned int Y, unsigned int stride)
{
    // To store the sliced vector
    vector<complex<float>> result;
 
    // Copy vector using copy function()
    int i = X;
    while (result.size() < Y)
    {
        result.push_back(vec[i]);
        i = i + stride;
    }
    // Return the final sliced vector
    return result;
}
 
void fft(vector<complex<float>>& x)
{
    // Check if it is splitted enough
 
    const size_t N = x.size();
    if (N <= 1) 
        return;
 
    vector<complex<float>> even = slicing(x, 0, N / 2,2); //split the inputs in even / odd indices subarrays
    vector<complex<float>>  odd = slicing(x, 1, N / 2,2);
 
    // conquer
    fft(even);
    fft(odd);
 
    // combine
    for (size_t k = 0; k < N / 2; ++k)
    {
        complex<float> t = std::polar<float>(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}
