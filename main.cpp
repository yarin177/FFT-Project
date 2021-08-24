#define _USE_MATH_DEFINES
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.hpp"
#include <algorithm>
#include <vector>
#include <fstream>

#include <QApplication>
#include <QImage>
#include <QLabel>

using namespace std;
const int N = 2048; // Samples
const int TIME_RESOLUTION = 32;
const int FREQUENCY_RESOLUTION = 64;

vector<float> read_csv(std::string filename) {
    vector<float> data;

    // Create an input filestream
    std::ifstream myFile(filename);

    std::string line;
    while (std::getline(myFile, line))
    {
        std::istringstream s(line);
        std::string field;
        vector<float> lineData;
        while (getline(s, field, ',')) {
            data.push_back(std::stof(field));
        }
        lineData.clear();
    }
    return data;
}

struct Color { // Represents a pixel color
    float R;
    float G;
    float B;
};

Color GetColorEntry(float x) { // Converts dBm entry to RGB color entry
    x = max(0.0f, 3 * x);
    Color pixel;
    x = 3 * x;
    float R = min(x, 1.0f) * 255;
    float G = min(1.0f, max(x - 1, 0.0f)) * 255;
    float B = min(1.0f, max(x - 2, 0.0f)) * 255;
    pixel = { R,G,B };
    return pixel;
}

Complex ApplyHanningWindow(vector<Complex>& in, int i, int chuck_size)
{
    float multiplier = 0.7 * (1 - cos(2 * M_PI * i / (chuck_size)));//Hanning Window
    return in[i] * multiplier;
}

QImage displaySpectrogram(vector<vector<float>> dBm)
{
    QImage image(QSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION), QImage::Format_RGB32);
    QRgb value;

    Color colors;

    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            colors = GetColorEntry(dBm[i][j]);
            value = qRgb(colors.R, colors.G, colors.B);
            image.setPixel(j, i, value);
        }
    }
    QImage img2 = image.scaled(640, 320, Qt::KeepAspectRatio);
    return img2;
}
vector<vector<float>> NormalizedBm(float max_dBm, vector<float> magnitude, vector<vector<float>> dBm)
{
    int counter = 0;
    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            dBm[i][j] = (30 + (20 * log10(magnitude[counter]))) / max_dBm; // Normalize output
            counter++;
        }
    }
    return dBm;
}

int main(int argc, char** argv)
{
    // GUI Stuff
    QApplication app(argc, argv);

    vector<vector<float>> dBm(TIME_RESOLUTION, std::vector<float>(FREQUENCY_RESOLUTION, 0));
    vector<float> time_samples = read_csv("spec_samples.csv");
    int to_complete = log2(N);
    int number_of_zeros = 0;
    if (pow(2, to_complete) < N)
        number_of_zeros = pow(2, to_complete + 1) - N;

    vector<Complex> in(N + number_of_zeros); // Hold the samples (Complex sine wave)
    vector<float> magnitude(N + number_of_zeros);


    const float Fs = 100; // How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs; // At what intervals time points are sampled
    float f = 10; // Frequency

    int chuck_size = FREQUENCY_RESOLUTION; // Chunk size (N / 64=64 chunks)
    Complex chuck[FREQUENCY_RESOLUTION];
    int j = 0;
    int counter = 0;
    float max_dBm = 0.0;
    for (int i = 0; i < N + number_of_zeros; i++)
    {
        in[i] = { time_samples[i],0 };// generate (complex) sine waveform

        if (i > N)
            in[i] = { {0},{0} };


        in[i] = ApplyHanningWindow(in, i, FREQUENCY_RESOLUTION);

        chuck[j] = in[i]; // take every value and put it in chunk until 64

        //compute FFT for each chunk
        if (i + 1 == chuck_size) // for each set of 64 chunks (i.e. 64,TIME_RESOLUTION,192), apply FFT and save it all in a 1d array (magnitude)
        {
            //f = f + 0.18;
            chuck_size += FREQUENCY_RESOLUTION;
            CArray data(chuck, FREQUENCY_RESOLUTION); // Apply fft for 64 chunk
            fft(data);
            j = 0;
            for (int h = 0; h < FREQUENCY_RESOLUTION; h++)
            {
                magnitude[counter] = abs(data[h]); // save it all in 1d array (we switch it to 2d later on)

                if (30 + (20 * log10(magnitude[counter])) > max_dBm) // already get the max dBm value to normalize later on
                    max_dBm = 30 + (20 * log10(magnitude[counter]));

                counter++;
            }

        }
        else
            j++;
    }
    dBm = NormalizedBm(max_dBm, magnitude, dBm);

    QImage image = displaySpectrogram(dBm);

    QLabel window;
    window.setPixmap(QPixmap::fromImage(image));
    window.show();

    return app.exec();
}
