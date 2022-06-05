#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QBuffer>
#include <QChartView>
#include <QLineSeries>
#include <QMainWindow>
#include <map>
#include <thread>
#include "server.h"

QT_CHARTS_USE_NAMESPACE
using namespace std;

const int TIME_RESOLUTION = 100;
const int FREQUENCY_RESOLUTION = 100;
static vector<vector<float>> mag_vector; // queue
static vector<map<int, string>> channels_results; //queue
static Server server;

struct Color {
    float R;
    float G;
    float B;
};

struct FFT_RESULTS {
  vector<int> ch; //channels
  vector<float> magnitude;
};

const int N = 100;
Color GetColorEntry(float x)
{
    Color pixel;
    int R = (int)x + 55;
    int G = 0;
    int B = 128;
    pixel = {R,G,B};
    return pixel;
}
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
QString getBase64Image(QImage image)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG"); // writes the image in PNG format inside the buffer
    QString image_base64 = QString::fromLatin1(byteArray.toBase64().data());
    return image_base64;
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
complexSignal get_random_signals(complexSignal fm, complexSignal am, float Fs)
{
    vector<int> possb;
    vector<int> options;
    for(int i = 1; i < 6; i++)
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
    std::cout << "sent FM: " << std::to_string((float)possb[0] / 1000000) << std::endl;
    for(int i = 1; i < fm_count; i++)
    {
        complexSignal temp_hold = frequencyMixerCopy(fm,possb[i],Fs);
        std::cout << "sent FM: " <<  std::to_string((float)possb[i] / 1000000) << std::endl;
        for(int j = 0; j < 1260000; j++)
        {
            all_samples[j] += temp_hold[j];
        }
    }
    
    for(int i = fm_count; i < fm_count+am_count; i++)
    {
        complexSignal temp_hold = frequencyMixerCopy(am,possb[i],Fs);
        std::cout << "sent AM: " <<  std::to_string((float)possb[i] / 1000000) << std::endl;
        for(int j = 0; j < 1260000; j++)
        {
            all_samples[j] += temp_hold[j];
        }
    }

    for(int i = fm_count + am_count; i < noise_count + fm_count + am_count; i++)
    {
        std::cout << "sent NOISE: " <<  std::to_string((float)possb[i] / 1000000) << std::endl;
        for(int j = 0; j < 1260000; j++)
        {
            temp = {(float)(0.2 * cos(2 * M_PI * possb[i] * (j * T))),(float)(0.2 * sin(2 * M_PI * possb[i] * (j * T))) }; // Noise
            all_samples[j] += temp;
        }
    }
    std::cout << std::endl;
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
QChartView* plot_freq_magnitude_spectrum(vector<float> freq_vector, vector<float> magn_vector)
{
    QLineSeries *series = new QLineSeries();
    for(int i = 0; i < N; i++)
    {
        series->append(freq_vector[i], magn_vector[i]);
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
                else if(channels_results[0].find(j)->second == "FM")
                    color = {0,(int)normilized_magnitude[j],0};
                value = qRgb(color.R, color.G, color.B);
                image.setPixel(j, i, value);
            }
            else
            {
                Color color;
                color = GetColorEntry(mag_vector[0][j]);
                value = qRgb(color.R, color.G, color.B);
                //value = qRgb(normilized_magnitude[j], normilized_magnitude[j], normilized_magnitude[j]);
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
void sendInfoToServer(QImage image, int colorbar, double t_end)
{
    QString image_base64 = getBase64Image(image);
    std::string send_to_server = "{\"image\" : \"" + image_base64.toStdString() + "\", \"colorbar\" : \"" + std::to_string(colorbar) + "\" , \"time\" : \"" + std::to_string((int)t_end) + "\"}";
    server.sendMessage(send_to_server);
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
            value = qRgb(55, 0, 128);
            image.setPixel(j, i, value);
            temp_vec.push_back(value);
        }
        values.push_back(temp_vec);
        temp_vec.clear();
    }
    QLabel window;
    QImage img2;
    img2 = image.scaled(1000, 1000, Qt::KeepAspectRatio);

    sendInfoToServer(img2,76,0);

    vector<vector<QRgb>> on_screen = values;

    QImage final_image = img2;
    QImage black_screen = image;
    int counter;
    QImage to_show;
    auto t_start = std::chrono::high_resolution_clock::now();
    while (true)
    {
        counter = 0;
        while(channels_results.size() == 0 || mag_vector.size() == 0)
        {
            
            if(final_image != image && counter != 100)
            {
                final_image = getUpdatedImageLine(final_image,image,counter);
                to_show = final_image.scaled(1000, 1000, Qt::KeepAspectRatio);

                auto t_end = std::chrono::high_resolution_clock::now();
                double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count() / 1000;

                sendInfoToServer(to_show,76,elapsed_time_ms);
                std::this_thread::sleep_for(25ms);
                counter++;
            }
            
        }
        //cout << "Data recieved!\n" << endl;
        QImage new_image = getUpdatedImage();

        for(int i = 0; i <= 100; i++)
        {
            if(counter < 100)
            {
                final_image = getUpdatedImageLine(final_image,black_screen,counter);
                counter++;
            }
            final_image = getUpdatedImageLine(final_image,new_image,i);
            to_show = final_image.scaled(1000, 1000, Qt::KeepAspectRatio);

            auto t_end = std::chrono::high_resolution_clock::now();
            double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count() / 1000;

            sendInfoToServer(to_show,76,elapsed_time_ms);
            std::this_thread::sleep_for(25ms);
        }
    }
    
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

    //std::cout << counterFM << " " << counterAM << std::endl;
    counterFM = 0;
    counterAM = 0;
    return result;
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
void realTimeAudio(vector<complexSignal> time_samples_fm,vector<complexSignal> time_samples_am, float Fs, int BW, NeuralNetwork nn, int max_delay)
{
    int spectrogram_rate = 60;
    //This function simulates incoming data

    vector<int> seconds;
    for(int i = 5; i < max_delay; i++)
    {
        seconds.push_back(i);
    }
    
    while (true)
    {
        vector<complexSignal> all_samples = generateRandomSamples(time_samples_fm[0],time_samples_am[0],Fs);
        FFT_RESULTS results = calculateFFT(spectrogram_rate,Fs,all_samples[0]);

        vector<int> ch = results.ch;

        map<int, string> nn_results = filterChannels(ch,all_samples,BW,Fs,nn);
        std::cout << "NN Results: " << std::endl;
        map<int, string>::iterator itr;
        for (itr = nn_results.begin(); itr != nn_results.end(); ++itr) {
            std::cout << itr->second << " SIGNAL DETECTED AT FREQUENCY: " << std::to_string(((float)(itr->first*12600)/1000000)) <<  "[MHz]" << std::endl;
        }
        std::cout << "\n";
        mag_vector.push_back(results.magnitude);
        channels_results.push_back(nn_results);
        std::shuffle(seconds.begin(), seconds.end(), std::random_device());
        std::this_thread::sleep_for(std::chrono::seconds(seconds[0]));
    }

}