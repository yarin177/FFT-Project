#define _USE_MATH_DEFINES

#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.h"
using namespace std;



int main(void)
{
    int i;
    double y;
    const int N = 1000;
    const double Fs = 100;//How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs;//# At what intervals time points are sampled
    const double f = 4;//frequency
    Complex in[N];
    double mag[N];
    double t[N];//time vector 

    for (int i = 0; i < N; i++)
    {
        t[i] = i * T;
        in[i] = { (0.7 * cos(2 * PI * f * t[i])), (0.7 * sin(2 * PI * f * t[i])) };// generate (complex) sine waveform
        double multiplier = 0.5 * (1 - cos(2 * PI * i / (N)));//Hanning Window
        in[i] = multiplier * in[i];
    }
    CArray data(in, N);


    printf("\n");
    printf("  Input Data:\n");
    printf("\n");

    for (i = 0; i < N; i++)
    {
        printf("  %4d  %12f\n", i, in[i].real());
    }


    fft(data);

    printf("  output Data from FFT implementation:\n");

    for (int i = 0; i < N; ++i)
    {
        std::cout << data[i] << std::endl;

    }

    printf("\n");
    printf("  log magnitude of frequency domain components :\n");
    for (i = 0; i < N; i++)
    {
        mag[i] = std::abs(data[i]);
        std::cout << mag[i] << endl;
    }

    int tpCount = N;
    int values[500];

    for (int i = 0; i < 500; i++)
        values[i] = i;
    float frequencies[500];
    float timePeriod = tpCount / Fs;

    printf("\n");

    for (int i = 0; i < 500; i++)
    {
        frequencies[i] = values[i] / timePeriod;
    }

    return 0;
}