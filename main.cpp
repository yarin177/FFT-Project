#define _USE_MATH_DEFINES
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
#include "Helpful_functions.cpp"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    NeuralNetwork nn; // Load Neural network from local file
    vector<complexSignal> time_samples_am = readLocalFile("NeuralNetwork/TrainingTestingFiles/AM_signal.csv",1260000); // load samples
    vector<complexSignal> time_samples_fm = readLocalFile("NeuralNetwork/TrainingTestingFiles/FM_signal.csv",1260000);
    float Fs = 1260000; // define Frequency sample rate and Bandwidth
    int BW = 12600;
    int max_delay = 10; // max delay between signals
    thread t1(displaySpectrogram);
    thread t2(realTimeAudio, time_samples_fm,time_samples_am,Fs,BW,nn,max_delay);
    return a.exec();
}
