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

const int N = 2048; // Samples
const int TIME_RESOLUTION = 32;
const int FREQUENCY_RESOLUTION = 32;
 
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
    vector<float> time_samples = read_csv("spec_samples.csv");
    vector<float> magnitude(N);
    Complex chunck[N];

    const float Fs = 44100; // How many time points are needed i,e., Sampling Frequency

    for (int i = 0; i < N; i++)
    {
        if(i < 256 || i > 512)
        {
            chunck[i] = { 0,0 };
        }
        else
        {
            chunck[i] = { time_samples[i],0 };
        }
        
    }
    CArray data(chunck, N); // Apply fft for 64 chunk
    fft(data);
    for (int i = 0; i < N; i++)
    {
        magnitude[i] = abs(data[i]);
    }

    float resolution_freq = ((float)Fs / N);
    vector<float> freq_vector = arange(0, Fs, resolution_freq);
    QChartView *chartView = plot_freq_magnitude_spectrum(freq_vector,magnitude);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();

    return a.exec();

}
