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

    else if (N % 2 == 0)
    {
        //Radix-2
        vector<complex<float>> even = slicing(x, 0, N / 2, 2); //split the inputs in even / odd indices subarrays
        vector<complex<float>>  odd = slicing(x, 1, N / 2, 2);

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
    else if (N % 3 == 0)
    {
        //Radix-3
        //factor 3 decimation
        vector<complex<float>> p0 = slicing(x, 0, N / 3, 2);
        vector<complex<float>> p1 = slicing(x, 1, N / 3, 2);
        vector<complex<float>> p2 = slicing(x, 2, N / 3, 2);
        for (int i = 0; i < N; i++)
        {
            complex<float> temp = p0[i % (int)N / 3];
            temp += (p1[i % (int)N / 3] * std::polar<float>(1.0, -2 * M_PI * i / N));
            temp += (p2[i % (int)N / 3] * std::polar<float>(1.0, -4 * M_PI * i / N));
            x[i] = temp;
        }
    }
    else if (N % 5 == 0)
    {
        //Radix-5
        //factor 5 decimation
        vector<complex<float>> p0 = slicing(x, 0, N / 5, 2); 
        vector<complex<float>> p1 = slicing(x, 1, N / 5, 2);
        vector<complex<float>> p2 = slicing(x, 2, N / 5, 2);
        vector<complex<float>> p3 = slicing(x, 3, N / 5, 2);
        vector<complex<float>> p4 = slicing(x, 4, N / 5, 2);
        for (int i = 0; i < N; i++)
        {
            complex<float> temp = p0[i % (int)N / 5];
            temp += (p1[i % (int)N / 5] * std::polar<float>(1.0, -2 * M_PI * i / N));
            temp += (p2[i % (int)N / 5] * std::polar<float>(1.0, -4 * M_PI * i / N));
            temp += (p3[i % (int)N / 5] * std::polar<float>(1.0, -6 * M_PI * i / N));
            temp += (p4[i % (int)N / 5] * std::polar<float>(1.0, -8 * M_PI * i / N));
            x[i] = temp;
        }
    }
}
