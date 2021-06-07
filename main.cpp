#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.h"
#include <algorithm>
#include <vector>
using namespace std;

const int N = 204800; // Samples
const int TIME_RESOLUTION = 400;
const int FREQUENCY_RESOLUTION = 512;
vector< vector<double> > dBm(TIME_RESOLUTION, std::vector<double>(FREQUENCY_RESOLUTION, 0));

struct Color { // Represents a pixel color
    double R;
    double G;
    double B;
};

Color GetColorEntry(double x) { // Converts dBm entry to RGB color entry
    Color pixel;
    x = 3 * x;
    double R = min(x, 1.0);
    double G = min(1.0, max(x - 1, 0.0));
    double B = min(1.0, max(x - 2, 0.0));
    pixel = { R,G,B };
    return pixel;
}

void displaySpectrogram() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, FREQUENCY_RESOLUTION, 0, TIME_RESOLUTION, -1, 1); // Project a 64x64 matrix on a 640x640 window for better overview
    //glPointSize(10); // Because the window size is x10 bigger, we'll enlarge each point by x10
    double R = 0, G = 0, B = 0;

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glBegin(GL_POINTS);
    double x = 0;
    Color colors;

    for (int i = 0; i < TIME_RESOLUTION; i++) // go over the array of colors
    {
        for (int j = 0; j < FREQUENCY_RESOLUTION; j++)
        {
            colors = GetColorEntry(dBm[i][j]);
            glColor3f(colors.R, colors.G, colors.B);
            glVertex2f(j, i);
        }
    }
    glEnd();

    glFlush();  // Render now
}

Complex ApplyHanningWindow(vector<Complex> &in, int i, int chuck_size)
{
    double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (chuck_size)));//Hanning Window
    return in[i] * multiplier;
}

void NormalizedBm(double max_dBm, vector<double> magnitude)
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
}

int main(int argc, char** argv)
{
    vector<Complex> in(N); // Hold the samples (Complex sine wave)
    vector<double> t(N); // Time vector 
    vector<double> magnitude(N);


    const double Fs = 100; // How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs; // At what intervals time points are sampled
    double f = 4; // Frequency
    int chuck_size = FREQUENCY_RESOLUTION; // Chunk size (N / 64=64 chunks)
    Complex chuck[FREQUENCY_RESOLUTION];
    int j = 0;
    int counter = 0;
    double max_dBm = 0.0;

    for (int i = 0; i < N; i++)
    {
        t[i] = i * T;
        in[i] = { (0.7 * cos(2 * M_PI * f * t[i])), (0.7 * sin(2 * M_PI * f * t[i])) };// generate (complex) sine waveform

        in[i] = ApplyHanningWindow(in, i, FREQUENCY_RESOLUTION);

        chuck[j] = in[i]; // take every value and put it in chunk until 64

        //compute FFT for each chunk
        if (i + 1 == chuck_size) // for each set of 64 chunks (i.e. 64,TIME_RESOLUTION,192), apply FFT and save it all in a 1d array (magnitude)
        {
            //f = f + 0.235;
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
    NormalizedBm(max_dBm, magnitude);

    // GUI Stuff
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitWindowSize(FREQUENCY_RESOLUTION, TIME_RESOLUTION);   // Set the window's initial TIME_RESOLUTION & FREQUENCY_RESOLUTION
    glutCreateWindow("spectrogram"); // Create a window with the given title
    glutDisplayFunc(displaySpectrogram); // Register display callback handler for window re-paint
    glutMainLoop();           // Enter the event-processing loop
    return 0;
}
