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

const int N = 100; // Samples
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
    int split_size = 1260000; // 1,260,000 is 1 seconds of sample data
    while (std::getline(myFile, line))
    {
        std::istringstream s(line);
        std::string field;
        vector<float> lineData;
        complexSignal temp;

        while (std::getline(s, field, ',')) {
            //data.push_back(std::stof(field));
            float value = std::stof(field);
            if (counter < split_size)
                temp.push_back({ value , 0 });
            else
                temp[counter - split_size].imag(value);
            counter++;
        }
        comp.push_back(temp);
        counter = 0;
        temp.clear();
    }
    return comp;
}

void frequencyMixer(complexSignal& data, const int frequency, const float Fs)
{
    complexSignal chunk(data.size());
    const double  T = 1 / Fs; // At what intervals time points are sampled
    for(int i = 0; i < chunk.size(); i++)
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
    for(int i = 0; i < 100; i++)
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
complexSignal decimated_array(int M,complexSignal arr)
{
    complexSignal decimated;
    for (size_t i = 0; i < arr.size(); i = i + M) {
        decimated.push_back(arr[i]);
    }
    return decimated;
}
 
int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    vector<complexSignal> time_samples = read_csv("test.csv");
    vector<float> magnitude;
    complexSignal combined_samples;
    float Fs = 1260000; // How many time points are needed i,e., Sampling Frequency
    int BW = 12600;
    int spectrogram_rate = 60;
    complexSignal final_samples(100);
    int start_point;
    int step = Fs / spectrogram_rate;
    vector<int> ch; // this will store the indexs of the samples after Treshold
    for(int i = 0; i < time_samples.size(); i++)
    {
        for(int j = 0; j < 1260000; j++)
        {
            float multiplier = 0.5 * (1 - cos(2*M_PI*j/100)); //Hann window
            time_samples[i][j] *= multiplier;
        }
    }

    //Split each sample by the spectrogram rate
    for(int i = 0; i < spectrogram_rate; i++)
    {
        start_point = (Fs / spectrogram_rate) * i;
        for(int j = start_point; j < start_point + step; j++)
        {
            //Compute and sum FFT of 100, 210 times
            combined_samples.push_back(time_samples[0][j]);
            if(combined_samples.size() == 100)
            {
                frequencyMixer(combined_samples,315000,Fs);
                fft(combined_samples);
                for(int h = 0; h < 100; h++)
                    final_samples[h] += combined_samples[h]; //SUM
                combined_samples.clear(); //Clear for next FFT
            }
        }
    }
    int temp = 100;
    int temp2 = Fs;
    for (int i = 0; i < temp; i++)
    {
        magnitude.push_back(abs(final_samples[i]));
    }

    //Threshold checking
    for (int i = 0; i < temp; i++)
    {
        if(magnitude[i] > 590000)
            ch.push_back(i);
    }
    
    //Classification Loop
    float fcenter = 0;
    for(int i = 0; i < ch.size(); i++)
    {
        fcenter = ch[i] * BW;
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
