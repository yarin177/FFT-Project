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
#include <Iir.h>

#include <QApplication>
#include <QImage>
#include <QLabel>
 
#include <QChartView>
#include <QLineSeries>
#include <QMainWindow>


using namespace std;
QT_CHARTS_USE_NAMESPACE

const int N = 100; // Samples
const int TIME_RESOLUTION = 100;
const int FREQUENCY_RESOLUTION = 100;

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
complexSignal frequencyMixerCopy(complexSignal data, const int frequency, const float Fs)
{
    complexSignal chunk(data.size());
    const double  T = 1 / Fs; // At what intervals time points are sampled
    for(int i = 0; i < chunk.size(); i++)
    {
        chunk[i] = {(float)(1 * cos(2 * M_PI * frequency * (i * T))),(float)(1 * sin(2 * M_PI * frequency * (i * T)))};
        data[i] = data[i] * chunk[i];
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
bool mapContainsKey(std::map<int, string>& map, int key)
{
  if (map.find(key) == map.end()) return false;
  return true;
} 
vector<float> normilizeMagnitude(vector<float> magnitude)
{
    for(int i = 0; i < magnitude.size(); i++)
    {
        magnitude[i] = 30 + (20 * log10(magnitude[i]));
    }
    return magnitude;
}
QImage displaySpectrogram(map<int, string> nn_results, vector<float> magnitude)
{
    QImage image(QSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION), QImage::Format_RGB32);
    QRgb value;

    vector<float> normilized_magnitude = normilizeMagnitude(magnitude);

    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            
            if(mapContainsKey(nn_results,j))
            {
                Color color;
                if(nn_results.find(j)->second == "AM")
                    color = {normilized_magnitude[j],0,0};
                else
                    color = {0,normilized_magnitude[j],0};
                value = qRgb(color.R, color.G, color.B);
                image.setPixel(j, i, value);
            }
            else
            {
                value = qRgb(normilized_magnitude[j], normilized_magnitude[j], normilized_magnitude[j]);
                image.setPixel(j, i, value);
            }
            
        }
    }
    QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
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
vector<double> doFilter(Iir::Butterworth::BandPass<3> f, const complexSignal& samples, int N, bool real_part)
{
    vector<double> to_return;
    float part;
    for (int i = 0; i < N; i++)
    {
        if (real_part)
            part = f.filter(samples[i].real());
        else
            part = f.filter(samples[i].imag());
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
struct FFT_RESULTS {
  vector<int> ch; //channels
  vector<float> magnitude;
};

FFT_RESULTS calculateFFT(int spectrogram_rate, int Fs, complexSignal to_test)
{
    vector<int> ch;
    complexSignal combined_samples;
    complexSignal final_samples(N);
    int start_point;
    int step = Fs / spectrogram_rate;
    vector<float> magnitude;
    //Split each sample by the spectrogram rate
    for(int i = 0; i < spectrogram_rate; i++)
    {
        //Starting from 0, and stepping every 21000 samples
        start_point = (Fs / spectrogram_rate) * i; 
        for(int j = start_point; j < start_point + step; j++)
        {
            //Compute and sum FFT of 100, 210 times so 21000 samples
            combined_samples.push_back(to_test[j]);
            if(combined_samples.size() == N)
            {
                //frequencyMixer(combined_samples,630000,Fs);
                fft(combined_samples);
                for(int h = 0; h < N; h++)
                {
                    complex<double> vaaal = abs(combined_samples[h]) / N;
                    final_samples[h] += vaaal; //SUM
                }
                
                combined_samples.clear(); //Clear for next FFT
            }
        }
    }
    int temp2 = Fs;
    for (int i = 0; i < N; i++)
    {
        magnitude.push_back(abs(final_samples[i]));
        if(magnitude[i] > 12000)
           ch.push_back(i);
    }
    FFT_RESULTS results;
    results.ch = ch;
    results.magnitude = magnitude;

    return results;
}
pair<int, string> splitAndPredict(complexSignal filtered_complex, NeuralNetwork nn, int channel)
{
    int counterFM = 0, counterAM = 0;
    pair<int, string> result;
    vector<float> inputs;
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
        result = pair<int, string>(channel, "FM");
    else
        result = pair<int, string>(channel, "AM");

    counterFM = 0;
    counterAM = 0;
    return result;
}
int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    NeuralNetwork nn;
    vector<complexSignal> time_samples_am = read_csv("AM_signal.csv");
    vector<complexSignal> time_samples_fm = read_csv("FM_signal2.csv");

    vector<complexSignal> all_samples; // this will store both am+fm
    vector<float> magnitude;
    complexSignal combined_samples;
    float Fs = 1260000;
    int BW = 12600;
    int spectrogram_rate = 60;
    complexSignal final_samples(N);
    int start_point;

    complexSignal after_BPF;

    complex<float> temp;
    complex<float> temp3;
    int frequency1 = 252000;

    vector<float> inputs;
    const double  T = 1 / Fs;


    frequencyMixer(time_samples_fm[0],793800,Fs); // 30
    frequencyMixer(time_samples_am[0],37800,Fs); // 75

    complexSignal tmp_samples_helper;
    complex<float> temp_value;
    for(int i = 0; i < time_samples_am.size(); i++)
    {
        for(int j = 0; j < time_samples_am[i].size(); j++)
        {
            temp = { (0.2 * cos(2 * M_PI * frequency1 * (j * T))), (0.2 * sin(2 * M_PI * frequency1 * (j * T))) }; // Noise
            temp_value = time_samples_fm[i][j] + time_samples_am[i][j] + temp;
            tmp_samples_helper.push_back(temp_value);
        }
        all_samples.push_back(tmp_samples_helper);
        tmp_samples_helper.clear();
    }
    
    map<int, string> nn_results;
    FFT_RESULTS results = calculateFFT(spectrogram_rate,Fs,all_samples[0]);
    vector<int> ch = results.ch;
    magnitude = results.magnitude;
    
    //Classification Loop
    float fcenter = 0;
    complexSignal filtered_complex;
    int holder;

    for(int i = 0; i < ch.size(); i++)
    {
        //cout << "Working on channel: " << ch[i] << endl;
        fcenter = ch[i] * BW;
        frequencyMixer(all_samples[0],-fcenter+504000,Fs); // Moving to 504KHz
        holder = fcenter-504000;

        Iir::Butterworth::BandPass<3> f_real;
        Iir::Butterworth::BandPass<3> f_imag;
        f_real.setup(1260000, 504000, 12600);
        f_imag.setup(1260000, 504000, 12600);

        //Filtering
        vector<double> real_filtered = doFilter(f_real, all_samples[0], 1260000, 1);
        vector<double> imag_filtered = doFilter(f_imag, all_samples[0], 1260000, 0);

        filtered_complex.clear();
        for(int h = 0; h < 1260000; h++)
        {
            filtered_complex.push_back({ real_filtered[h] , imag_filtered[h]});
        }
        frequencyMixer(filtered_complex,-504000,Fs); // Moving to 0Hz
        frequencyMixer(all_samples[0],holder,Fs); // Restoring default freqs

        nn_results.insert(splitAndPredict(filtered_complex,nn,ch[i]));
    }
    //cout << "fcenter at: " << fcenter << " filtering above " << fcenter + (BW / 2) << " and below " << fcenter - (BW / 2) << endl;

    //results = calculateFFT(spectrogram_rate,Fs,filtered_complex);
    //ch = results.ch;
    //magnitude = results.magnitude;

    std::cout << "Final output: " << std::endl;
    map<int, string>::iterator itr;
    for (itr = nn_results.begin(); itr != nn_results.end(); ++itr) {
        std::cout << itr->first << ':' << itr->second <<  ' ' << std::endl;
    }
    
    
    float resolution_freq = ((float)Fs / N);
    vector<float> freq_vector = arange(0, Fs, resolution_freq);

    
    /*
    QChartView *chartView = plot_freq_magnitude_spectrum(freq_vector,magnitude);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1800, 1000);
    window.show();
    */

    
    QImage image = displaySpectrogram(nn_results,magnitude);

    QLabel window;
    window.setPixmap(QPixmap::fromImage(image));
    window.setScaledContents( true );
    window.setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    window.show();
    

    return a.exec();

}
