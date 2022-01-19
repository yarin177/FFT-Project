#define _USE_MATH_DEFINES
#include <map>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.hpp"
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
#include <thread>


using namespace std;
QT_CHARTS_USE_NAMESPACE

const int N = 100; // Samples
const int TIME_RESOLUTION = 100;
const int FREQUENCY_RESOLUTION = 100;
static vector<vector<float>> mag_vector; // queue
static vector<map<int, string>> channels_results; //queue

struct Color{
    int R;
    int G;
    int B;
};
void frequencyMixerOriginal(complexSignal& data, const int frequency, const float Fs)
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
        if(magnitude[i] < 0)
            magnitude[i] = 0;
    }
    return magnitude;
}
QImage getUpdatedImage()
{
    QImage image(QSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION), QImage::Format_RGB32);
    QRgb value;
    vector<vector<QRgb>> values;
    vector<QRgb> temp_vec;
    vector<float> normilized_magnitude = normilizeMagnitude(mag_vector[0]);
    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            
            if(mapContainsKey(channels_results[0],j))
            {
                Color color;
                if(channels_results[0].find(j)->second == "AM")
                    color = {(int)normilized_magnitude[j],0,0};
                else
                    color = {0,(int)normilized_magnitude[j],0};
                value = qRgb(color.R, color.G, color.B);
                image.setPixel(j, i, value);
            }
            else
            {
                value = qRgb(normilized_magnitude[j], normilized_magnitude[j], normilized_magnitude[j]);
                image.setPixel(j, i, value);
            }
            temp_vec.push_back(value);
            
        }
        values.push_back(temp_vec);
        temp_vec.clear();
    }
    mag_vector.erase(mag_vector.begin()); // remove
    channels_results.erase(channels_results.begin());
    //QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
    return image;
}
vector<QImage> repaintEffect(vector<vector<QRgb>> on_screen, vector<vector<QRgb>> new_image, QImage &image)
{
    QRgb value;
    vector<QImage> images;
    for(int counter = 0; counter <= 100; counter++)
    {
        for (int i = 0; i < TIME_RESOLUTION; i++)
        {
            for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
            {
                if(counter == i)
                {
                    on_screen[i] = new_image[i];
                }
                else
                {
                    value = on_screen[i][j];
                    image.setPixel(j, i, value);
                }
            }
        }
        QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
        images.push_back(img2);
    }
    return images;
}

QImage getUpdatedImageLine(QImage original_image, QImage updated_image, int line)
{
    QRgb value;
    QImage image(QSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION), QImage::Format_RGB32);
    //updated_image = updated_image.scaled(100, 100, Qt::KeepAspectRatio);
    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            //value = updated_image.pixel(j,i);
            //image.setPixel(j,i, value);
            
            if(line == i)
            {
                value = updated_image.pixel(j,i);
                //std::cout << "VAL1: " << value << " VAL2: " << value2 << endl;
                //std::this_thread::sleep_for(10000ms);
                image.setPixel(j,i, value);
            }

            else
            {
                value = original_image.pixel(j,i);
                image.setPixel(j,i, value);
            }
            
            
        }
    }
    //QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
    return image;
}

void displaySpectrogram()
{
    QImage image(QSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION), QImage::Format_RGB32);
    QRgb value;
    vector<vector<QRgb>> values;
    vector<QRgb> temp_vec;
    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            value = qRgb(0, 0, 0);
            image.setPixel(j, i, value);
            temp_vec.push_back(value);
        }
        values.push_back(temp_vec);
        temp_vec.clear();
    }

    QLabel window;
    QImage img2;
    
    img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
    window.setPixmap(QPixmap::fromImage(img2));
    window.setScaledContents( true );
    window.setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    window.show();

    vector<vector<QRgb>> on_screen = values;

    QImage final_image = img2;
    QImage black_screen = image;
    int counter;
    QImage to_show;
    while (true)
    {
        counter = 0;
        while(channels_results.size() == 0 || mag_vector.size() == 0)
        {
            
            if(final_image != image && counter != 100)
            {
                final_image = getUpdatedImageLine(final_image,image,counter);
                to_show = final_image.scaled(1000, 1000, Qt::KeepAspectRatio);
                window.setPixmap(QPixmap::fromImage(to_show));
                window.repaint();
                std::this_thread::sleep_for(25ms);
                counter++;
            }
            
            //QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
            //window.setPixmap(QPixmap::fromImage(img2));
            //window.repaint();
            //repaintEffect()
        }
        cout << "Data recieved!" << endl;
        QImage new_image = getUpdatedImage();
        //repaint effect
        //vector<QImage> images = repaintEffect(on_screen,new_image,image);

        for(int i = 0; i <= 100; i++)
        {
            if(counter < 100)
            {
                final_image = getUpdatedImageLine(final_image,black_screen,counter);
                counter++;
            }
            final_image = getUpdatedImageLine(final_image,new_image,i);
            to_show = final_image.scaled(1000, 1000, Qt::KeepAspectRatio);
            window.setPixmap(QPixmap::fromImage(to_show));
            window.repaint();
            std::this_thread::sleep_for(25ms);
        }

        //final_image = new_image;
        //std::this_thread::sleep_for(25000ms);
        //final_image = images[100];
        //on_screen = new_image;

        //channels_results.clear();
        //mag_vector.clear();
    }
    /*
    vector<vector<QRgb>> values;
    vector<vector<QRgb>> old_values;

    vector<QRgb> temp_vec;
    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            value = qRgb(0, 255, 0);
            image.setPixel(j, i, value);
            temp_vec.push_back(value);
        }
        values.push_back(temp_vec);
        temp_vec.clear();
    }

    for (int i = 0; i < TIME_RESOLUTION; i++)
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            value = qRgb(255, 0, 0);
            temp_vec.push_back(value);
        }
        old_values.push_back(temp_vec);
        temp_vec.clear();
    }
    */
    
    /*
    QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
    QLabel window;
    window.setPixmap(QPixmap::fromImage(img2));
    window.setScaledContents( true );
    window.setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    window.show();
    std::this_thread::sleep_for(3000ms);
    int counter = 0;
    int tf = 0;
    vector<vector<QRgb>> on_screen = values;
    while (true)
    {
        for (int i = 0; i < TIME_RESOLUTION; i++)
        {
            for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
            {
                if(counter == i)
                {
                    on_screen[i] = values[i];
                }
                else
                {
                    value = on_screen[i][j];
                    image.setPixel(j, i, value);
                }
            }
        }
        QImage img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);
        window.setPixmap(QPixmap::fromImage(img2));
        window.repaint();
        std::this_thread::sleep_for(25ms);
        counter++;
        if(counter == 100)
        {
            vector<vector<QRgb>> sheesh = values;
            values = old_values;
            old_values = sheesh;
            counter = 0;
        }
    }
    */
    //return img2;
}
/*
QChartView* plot_freq_magnitude_spectrum(vector<float> freq_vector)
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
*/
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
                //frequencyMixerOriginal(combined_samples,630000,Fs);
                fft(combined_samples);
                for(int h = 0; h < N; h++)
                {
                    complex<double> vaaal = abs(combined_samples[h]) / N;
                    final_samples[h] += vaaal; //SUM
                }
                
                combined_samples.clear(); //Clear for next FFT
            }
        }
        break; // break here?
    }
    for (int i = 0; i < N; i++)
    {
        magnitude.push_back(abs(final_samples[i]));
        if(magnitude[i] > 200) // change to 200?
           ch.push_back(i);
        //td::cout << "I: " << magnitude[i] << std::endl;
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
    //cout << counterFM << " AND " << counterAM << endl;
    if(counterFM > counterAM)
        result = pair<int, string>(channel, "FM");
    else
        result = pair<int, string>(channel, "AM");

    counterFM = 0;
    counterAM = 0;
    return result;
}

complexSignal get_random_signals(complexSignal fm, complexSignal am, float Fs)
{
    vector<int> possb;
    vector<int> options;
    for(int i = 5; i < 10; i++)
    {
        options.push_back(i);
    }
    std::shuffle(options.begin(), options.end(), std::random_device());

    complexSignal all_samples;

    int fm_count = options[0];
    int am_count = options[1];
    int noise_count = options[3];

    for(int i = 0; i < 1260000; i = i + 12600)
    {
        possb.push_back(i);
    }
    std::shuffle(possb.begin(), possb.end(), std::random_device());

    complex<float> temp;
    const double  T = 1 / Fs;
    complex<float> temp_value;
    all_samples = frequencyMixerCopy(fm,possb[0],Fs);
    for(int i = 1; i < fm_count; i++)
    {
        complexSignal temp_hold = frequencyMixerCopy(fm,possb[i],Fs);
        for(int j = 0; j < 1260000; j++)
        {
            all_samples[j] += temp_hold[j];
        }
    }
    
    for(int i = fm_count; i < fm_count+am_count; i++)
    {
        complexSignal temp_hold = frequencyMixerCopy(am,possb[i],Fs);
        for(int j = 0; j < 1260000; j++)
        {
            all_samples[j] += temp_hold[j];
        }
    }

    for(int i = fm_count + am_count; i < noise_count + fm_count + am_count; i++)
    {
        for(int j = 0; j < 1260000; j++)
        {
            temp = {(float)(0.2 * cos(2 * M_PI * possb[i] * (j * T))),(float)(0.2 * sin(2 * M_PI * possb[i] * (j * T))) }; // Noise
            all_samples[j] += temp;
        }
    }
    
    return all_samples;

}
vector<complexSignal> generateRandomSamples(complexSignal fm, complexSignal am, float Fs)
{
    vector<complexSignal> all_samples;
    vector<int> possb;
    for(int i = 0; i < 1260000; i = i + 12600) // Generate random channels
    {
        possb.push_back(i);
    }
    std::shuffle(possb.begin(), possb.end(), std::random_device());

    all_samples.push_back(get_random_signals(fm,am,Fs));
    return all_samples;

}
map<int, string> filterChannels(vector<int> ch, vector<complexSignal> all_samples,int BW, float Fs, NeuralNetwork nn)
{
    //Classification Loop
    map<int, string> nn_results;
    float fcenter = 0;
    complexSignal filtered_complex;
    int holder;
    for(int i = 0; i < ch.size(); i++)
    {
        //cout << "Working on channel: " << ch[i] << endl;
        fcenter = ch[i] * BW;
        frequencyMixerOriginal(all_samples[0],-fcenter+504000,Fs); // Moving to 504KHz
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
            filtered_complex.push_back({ (float)real_filtered[h] , (float)imag_filtered[h]});
        }
        frequencyMixerOriginal(filtered_complex,-504000,Fs); // Moving to 0Hz
        frequencyMixerOriginal(all_samples[0],holder,Fs); // Restoring default freqs

        nn_results.insert(splitAndPredict(filtered_complex,nn,ch[i]));
    }
    return nn_results;
}
void realTimeAudio(vector<complexSignal> time_samples_fm,vector<complexSignal> time_samples_am, float Fs, int BW, NeuralNetwork nn)
{
    int spectrogram_rate = 60;
    //This function simulates incoming data

    vector<int> seconds;
    for(int i = 1; i < 5; i++)
    {
        seconds.push_back(i);
    }

    cout << "Press the ENTER key to start generating incoming data";
    if (cin.get() == '\n')
        cout << "Generating data...." << endl;
    while (true)
    {
        vector<complexSignal> all_samples = generateRandomSamples(time_samples_fm[0],time_samples_am[0],Fs);
        FFT_RESULTS results = calculateFFT(spectrogram_rate,Fs,all_samples[0]);

        vector<int> ch = results.ch;

        map<int, string> nn_results = filterChannels(ch,all_samples,BW,Fs,nn);
        std::cout << "Final output: " << std::endl;
        map<int, string>::iterator itr;
        for (itr = nn_results.begin(); itr != nn_results.end(); ++itr) {
            std::cout << itr->first << ':' << itr->second <<  ' ' << std::endl;
        }
        mag_vector.push_back(results.magnitude);
        channels_results.push_back(nn_results);

        std::shuffle(seconds.begin(), seconds.end(), std::random_device());
        //std::cout << "Sleeping for: " <<  seconds[0] << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(seconds[0]));
    }
    //std::this_thread::sleep_for(5000ms);

}
int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    NeuralNetwork nn;
    vector<complexSignal> time_samples_am = readLocalFile("NeuralNetwork/TrainingTestingFiles/AM_signal.csv");
    vector<complexSignal> time_samples_fm = readLocalFile("NeuralNetwork/TrainingTestingFiles/FM_signal.csv");
    vector<float> magnitude;
    complexSignal combined_samples;
    float Fs = 1260000;
    int BW = 12600;
    int spectrogram_rate = 60;
    complexSignal final_samples(N);
    complexSignal after_BPF;
    complex<float> temp3;
    vector<float> inputs;
    
    //realTimeAudio(time_samples_fm,time_samples_am,Fs,BW,nn);
    //float resolution_freq = ((float)Fs / N);
    //vector<float> freq_vector = arange(0, Fs, resolution_freq);
    
    /*
    QChartView *chartView = plot_freq_magnitude_spectrum(freq_vector);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1800, 1000);
    window.show();
    */

    thread t1(displaySpectrogram);
    thread t2(realTimeAudio, time_samples_fm,time_samples_am,Fs,BW,nn);
    //a.exec();
    return a.exec();

}
