#define _USE_MATH_DEFINES
#include <complex>
#include <valarray>

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

void fft(CArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    CArray even = x[std::slice(0, N / 2, 2)]; //split the inputs in even / odd indices subarrays
    CArray  odd = x[std::slice(1, N / 2, 2)];

    // conquer
    fft(even); 
    fft(odd);

    // combine
    for (size_t k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar<float>(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}
