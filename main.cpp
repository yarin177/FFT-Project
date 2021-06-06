#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.h"
#include <algorithm>

using namespace std;

const int N = 4096; // Samples
double dBm[128][32]; // Used to hold dBm values

struct Color { // Represents a pixel color
    double R;
    double G;
    double B;
};

void GetColorEntry(Color& color, double x) { // Converts dBm entry to RGB color entry
    x = 3 * x;
    double R = min(x, 1.0);
    double G = min(1.0, max(x - 1, 0.0));
    double B = min(1.0, max(x - 2, 0.0));
    color = { R,G,B };
}

void displaySpectrogram() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 32, 0, 128, -1, 1); // Project a 64x64 matrix on a 640x640 window for better overview
    glPointSize(10); // Because the window size is x10 bigger, we'll enlarge each point by x10
    double R = 0, G = 0, B = 0;

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glBegin(GL_POINTS);
    double x = 0;
    Color colors;

    for (int i = 0; i < 128; i++) // go over the array of colors
    {
        for (int j = 0; j < 32; j++)
        {
            GetColorEntry(colors,dBm[i][j]);
            glColor3f(colors.R, colors.G, colors.B);
            glVertex2f(j, i);
        }
    }
    glEnd();

    glFlush();  // Render now
}

Complex ApplyHanningWindow(Complex in[], int i, int chuck_size)
{
    double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (chuck_size)));//Hanning Window
    return multiplier * in[i];
}

int main(int argc, char** argv)
{
    Complex in[N]; // Hold the samples (Complex sine wave)
    double t[N]; // Time vector 
    double magnitude[N];
    double spectrums[128][32];

    int frequency_resolution = 32, time_resolution = 128;

    const double Fs = 100; // How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs; // At what intervals time points are sampled
    double f = 4; // Frequency
    int chuck_size = 32; // Chunk size (N / 64=64 chunks)
    Complex chuck[32];
    int j = 0;
    int counter = 0;
    double max_dBm = 0.0;

    for (int i = 0; i < N; i++)
    {
        t[i] = i * T;
        in[i] = { (0.7 * cos(2 * M_PI * f * t[i])), (0.7 * sin(2 * M_PI * f * t[i])) };// generate (complex) sine waveform

        in[i] = ApplyHanningWindow(in, i, 32);

        chuck[j] = in[i]; // take every value and put it in chunk until 64

        //compute FFT for each chunk
        if (i + 1 == chuck_size) // for each set of 64 chunks (i.e. 64,128,192), apply FFT and save it all in a 1d array (magnitude)
        {
            f = f + 0.7;
            chuck_size += 32;
            CArray data(chuck, 32); // Apply fft for 64 chunk
            fft(data);
            j = 0;
            for (int h = 0; h < 32; h++)
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
    counter = 0; 
    // We want to save the magnitude into a 2d array of 64x64 inorder to plot the spectrogram, for that we'll go over the array and save it to a new 2d array.
    std::cout << "max dBm is: " << max_dBm;
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            dBm[i][j] = (30 + (20 * log10(magnitude[counter]))) / max_dBm; // Normalize output
            counter++;
        }
    }

    // GUI Stuff
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitWindowSize(320, 1280);   // Set the window's initial width & height
    glutCreateWindow("1280x320 spectrogram (based on 64x64 matrix)"); // Create a window with the given title
    glutDisplayFunc(displaySpectrogram); // Register display callback handler for window re-paint
    glutMainLoop();           // Enter the event-processing loop
    return 0;
}
