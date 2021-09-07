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
 
#include <QChartView>
#include <QLineSeries>
#include <QMainWindow>

using namespace std;
QT_CHARTS_USE_NAMESPACE

const int N = 256; // Samples
const int TIME_RESOLUTION = 32;
const int FREQUENCY_RESOLUTION = 32;

typedef vector<complex<float>> complexSignal;
vector<complexSignal> read_csv(std::string filename) {
    vector<float> data;
    vector<complexSignal> comp;
    // Create an input filestream
    std::ifstream myFile(filename);
    std::string line;
    int counter = 0; // used to keep track of real & imag parts
    while (std::getline(myFile, line))
    {
        std::istringstream s(line);
        std::string field;
        vector<float> lineData;
        complexSignal temp;
        int i = 0;
        while (std::getline(s, field, ',')) {
            //data.push_back(std::stof(field));
            float multiplier = 0.5 * (1 - cos(2*M_PI*i/256)); // Hamming Window
            float value = std::stof(field);
            if (counter < 256)
                temp.push_back({ value * multiplier , 0 });
            else
                temp[counter - 256].imag(value * multiplier);
            counter++;
            i++;
        }
        comp.push_back(temp);
        counter = 0;
        i = 0;
        temp.clear();
    }
    return comp;
}

void frequencyMixer(complexSignal& data, int frequency)
{
    complexSignal chunk(N);
    const float Fs = 176400; // How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs; // At what intervals time points are sampled
    for(int i = 0; i < N; i++)
    {
        chunk[i] = {(float)(1 * cos(2 * M_PI * frequency * (i * T))),(float)(1 * sin(2 * M_PI * frequency * (i * T)))};
        data[i] = data[i] * chunk[i];
    }
} 
vector<float> arange(float start, float stop, float step) {
    vector<float> values;
    for (float value = start; value < stop; value += step)
        values.push_back(value);
    return values;
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
QChartView* plot_freq_magnitude_spectrum(vector<float> freq_vector, vector<float> mag_vector)
{
    QLineSeries *series = new QLineSeries();
    for(int i = 0; i < N; i++)
    {
        series->append(freq_vector[i], mag_vector[i]);
    }
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Frequency-time spectrum");


    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}
 
int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    vector<complexSignal> time_samples = read_csv("complex_samples_fm_am.csv");
    vector<float> magnitude;
    //Complex chunk = *frequencyMixer(time_samples,10000);
    float Fs = 176400; // How many time points are needed i,e., Sampling Frequency
    for (int i = 0; i < N; i++)
    {
        //value2 = (float)(1 * cos(2 * M_PI * 75000 * (i * T)));
        //float multiplier = 0.5 * (1 - cos(2*M_PI*i/256)); // Hamming Window
        //test = {time_samples[0][i] * multiplier, time_samples[0][i+N] * multiplier};// generate (complex) sine waveform
        //chunk2.push_back(chunk[i]);
    }
    //frequencyMixer(time_samples[0],56783);
    //CArray data(chunk, N); // Apply fft for 64 chunk
    fft(time_samples[0]);
    int temp = N;
    int temp2 = Fs;
    for (int i = 0; i < temp; i++)
    {
        magnitude.push_back(abs(time_samples[0][i]));
    }
    float resolution_freq = ((float)temp2 / temp);
    vector<float> freq_vector = arange(0, temp2, resolution_freq);

    QChartView *chartView = plot_freq_magnitude_spectrum(freq_vector,magnitude);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1800, 1000);
    window.show();

    return a.exec();

}
