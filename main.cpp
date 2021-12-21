#define _USE_MATH_DEFINES
#include <map>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.hpp"
#include "filt.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include "NeuralNetwork/NeuralNetwork.h"
#include "NeuralNetwork/Training.h"

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
complexSignal decimated_array(int M,complexSignal arr)
{
    complexSignal decimated;
    for (size_t i = 0; i < arr.size(); i = i + M) {
        decimated.push_back(arr[i]);
    }
    return decimated;
}
vector<double> doFilter(Filter* my_filter, const complexSignal& samples, int N, bool real_part)
{
    vector<double> to_return;
    float part;
    for (int i = 0; i < N; i++)
    {
        if (real_part)
            part = my_filter->do_sample(samples[i].real());
        else
            part = my_filter->do_sample(samples[i].imag());
        to_return.push_back(part);
    }
    return to_return;

}
string predict_modulation(NeuralNetwork nn, vector<float> inputs)
{
    //This function takes a Neural Network object and vector of inputs
    //It Outputs the type of modulation classified by the NN (AM\FM)

    vector<float> outputs = nn.predict(inputs);
    if(outputs[0] > outputs[1])
        return "FM";
    else
        return "AM";
}
int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    NeuralNetwork nn;
    vector<complexSignal> time_samples_am = read_csv("AM_signal.csv");
    vector<complexSignal> time_samples_fm = read_csv("FM_signal.csv");
    vector<complexSignal> all_samples; // this will store both am+fm
    vector<float> magnitude;
    complexSignal combined_samples;
    float Fs = 1260000;
    int BW = 12600;
    int spectrogram_rate = 60;
    complexSignal final_samples(N);
    int start_point;
    int step = Fs / spectrogram_rate;
    vector<int> ch;

    Filter* BPF_Filter_real;
    Filter* BPF_Filter_imag;

    complexSignal after_BPF;

    complex<double> temp;

    int frequency1 = 252000;
    int frequency2 = 315000;
    int frequency3 = 387000;

    vector<float> inputs;
    int counterFM = 0, counterAM = 0;
    frequencyMixer(time_samples_fm[0],793800,Fs);
    frequencyMixer(time_samples_am[0],37800,Fs);
    
    complexSignal tmp_samples_helper;
    complex<float> temp_value;
    for(int i = 0; i < time_samples_am.size(); i++)
    {
        for(int j = 0; j < time_samples_am[i].size(); j++)
        {
            temp_value.real(time_samples_am[i][j].real() + time_samples_fm[i][j].real());
            temp_value.imag(time_samples_am[i][j].imag() + time_samples_fm[i][j].imag());
            
            tmp_samples_helper.push_back(temp_value);
        }
        all_samples.push_back(tmp_samples_helper);
        tmp_samples_helper.clear();
    }
    /*
    const double  T = 1 / 1260000.0;
    vector<float> inputs;
    int counterFM = 0, counterAM = 0;
    int fcenter = 630000;
    complexSignal filtered_complex;
    frequencyMixer(time_samples[0],fcenter,Fs);
    
    cout << "fcenter at: " << fcenter << ", filtering above " << fcenter + (BW / 2) << " and below " << fcenter - (BW / 2) << endl;
    BPF_Filter_real = new Filter(BPF, 40, Fs, fcenter - (BW / 2), fcenter + (BW / 2));
    BPF_Filter_imag = new Filter(BPF, 40, Fs, fcenter - (BW / 2), fcenter + (BW / 2));

    std::cout << "Error: " << BPF_Filter_real->get_error_flag() << std::endl;
    std::cout << "Error: " << BPF_Filter_imag->get_error_flag() << std::endl;

    vector<double> real_filtered = doFilter(BPF_Filter_real, time_samples[0], 21000, 1);
    vector<double> imag_filtered = doFilter(BPF_Filter_imag, time_samples[0], 21000, 0);

    for(int h = 0; h < 21000; h++)
    {
        filtered_complex.push_back({ real_filtered[h] , imag_filtered[h]});
    }
    
    frequencyMixer(filtered_complex,-fcenter,Fs);

    for(int h = 0; h < 80; h++)
    {
        for(int j = h*256; j < (h*256) + 256; j++)
            inputs.push_back(filtered_complex[j].real());

        for(int j = h*256; j < (h*256) + 256; j++)
            inputs.push_back(filtered_complex[j].imag());

        string prediction = predict_modulation(nn,inputs);
        if (prediction == "FM")
            counterFM++;
        else
            counterAM++;
        inputs.clear();
    }
    cout << counterFM << " AND " << counterAM << endl;

    //FFT
    fft(after_BPF);
    for(int j = 0; j < 100; j++)
    {
        magnitude.push_back(abs(after_BPF[j]));
    }

    /*
    //generate complex sine wave
    /*
    for (int i = 0; i < 1260000; i++)
    {
        temp = { (0.7 * cos(2 * M_PI * frequency1 * (i * T))), (0.7 * sin(2 * M_PI * frequency1 * (i * T))) };
        time_samples[0][i] += temp;

        temp = { (0.7 * cos(2 * M_PI * frequency3 * (i * T))), (0.7 * sin(2 * M_PI * frequency3 * (i * T))) };
        time_samples[0][i] += temp;
    }
    */
    
    //Split each sample by the spectrogram rate
    for(int i = 0; i < spectrogram_rate; i++)
    {
        //Starting from 0, and stepping every 21000 samples
        start_point = (Fs / spectrogram_rate) * i; 
        for(int j = start_point; j < start_point + step; j++)
        {
            //Compute and sum FFT of 100, 210 times so 21000 samples
            combined_samples.push_back(all_samples[0][j]);
            if(combined_samples.size() == N)
            {
                //frequencyMixer(combined_samples,630000,Fs);
                fft(combined_samples);
                for(int h = 0; h < N; h++)
                    final_samples[h] += combined_samples[h]; //SUM
                combined_samples.clear(); //Clear for next FFT
            }
        }
    }
    int temp2 = Fs;
    for (int i = 0; i < N; i++)
    {
        magnitude.push_back(abs(final_samples[i]));
    }
    //Threshold checking
    for (int i = 0; i < N; i++)
    {
        if(magnitude[i] > 6000)
        {
            ch.push_back(i);
            cout << "Found Ch: " << i << endl;
        }
    }
    
    map<int, string> nn_results;
    complexSignal filtered_complex;
    complexSignal to_fft;
    //Classification Loop
    float fcenter = 0;
    for(int i = 0; i < ch.size(); i++)
    {
        fcenter = ch[i] * BW;
        cout << "fcenter at: " << fcenter << ", filtering above " << fcenter + (BW / 2) << " and below " << fcenter - (BW / 2) << endl;
        BPF_Filter_real = new Filter(BPF, 40, Fs, fcenter - (BW / 2), fcenter + (BW / 2));
        BPF_Filter_imag = new Filter(BPF, 40, Fs, fcenter - (BW / 2), fcenter + (BW / 2));

        vector<double> real_filtered = doFilter(BPF_Filter_real, all_samples[0], 21000, 1);
        vector<double> imag_filtered = doFilter(BPF_Filter_imag, all_samples[0], 21000, 0);

        for(int h = 0; h < 21000; h++)
        {
            filtered_complex.push_back({ real_filtered[h] , imag_filtered[h]});
        }
    

        frequencyMixer(filtered_complex,-fcenter,Fs);

        cout << "Splitting 21,000 samples into chunks of 256 and predicting type of modulation for each chunk" << endl;
        for(int h = 0; h < 80; h++)
        {
            for(int j = h*256; j < (h*256) + 256; j++)
                inputs.push_back(filtered_complex[j].real());

            for(int j = h*256; j < (h*256) + 256; j++)
                inputs.push_back(filtered_complex[j].imag());

            string prediction = predict_modulation(nn,inputs);
            if (prediction == "FM")
                counterFM++;
            else
                counterAM++;
            inputs.clear();
        }
        cout << counterFM << " AND " << counterAM << endl;
        if(counterFM > counterAM)
            nn_results.insert(pair<int, string>(ch[i], "FM"));
        else
            nn_results.insert(pair<int, string>(ch[i], "AM"));

        counterFM = 0;
        counterAM = 0;
    }
    std::cout << "Final output: " << std::endl;
    map<int, string>::iterator itr;
    for (itr = nn_results.begin(); itr != nn_results.end(); ++itr) {
        std::cout << itr->first << ':' << itr->second <<  ' ' << std::endl;
    }
    
    float resolution_freq = ((float)Fs / N);
    vector<float> freq_vector = arange(0, Fs, resolution_freq);

    QChartView *chartView = plot_freq_magnitude_spectrum(freq_vector,magnitude);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1800, 1000);
    window.show();

    return a.exec();

}
