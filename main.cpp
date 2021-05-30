#define _USE_MATH_DEFINES
#include <GL/glut.h>


#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.h"
#include <algorithm>
#include <vector>
using namespace std;

const int N = 4096; // samples
Complex in[N];
double t[N];//time vector 
double magnitude[N];
double dBm[N / 64][N / 64]; // used to convert FFT output to dBm
double spectrums[N / 64][N / 64];

vector<double> f(double x) {
    x = 3 * x;
    double R = min(x, 1.0);
    double G = min(1.0, max(x - 1, 0.0));
    double B = min(1.0, max(x - 2, 0.0));
    vector<double> color = { R,G,B };
    return color;
}
void displaySpectrogram() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 72, 0, 64, -1, 1); // project a 64x64 matrix on a 640x640 window for better overview
    glPointSize(10); // because the window size is x10 bigger, we'll enlarge each point by x10
    double main_color = 0, R = 0, G = 0, B = 0;

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glBegin(GL_POINTS);
    double mat[62][11];
    double x = 0;

    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 11; ++j) {
            mat[i][j] = x;
        }
        x += 1 / 50.0;
    }
    int counter = 0;
    for (int i = 0; i < 64; i++) // go over the array of colors
    {
        for (int j = 0; j < 72; j++)
        {
            if (j >= 64 && i < 50)
            {
                vector<double> colors = f(mat[i][counter]);
                glColor3f(colors[0], colors[1], colors[2]);
                glVertex2f(j, i);
                counter++;
            }
            else if (j >= 64 && i > 50)
            {
                glColor3f(0, 0, 0);
                glVertex2f(j, i);
                counter++;
            }
            else
            {
                vector<double> colors = f(dBm[i][j]);
                glColor3f(colors[0], colors[1], colors[2]);
                glVertex2f(j, i);
            }
        }
        counter = 0;
    }
    glEnd();

    glFlush();  // Render now
}

int main(int argc, char** argv)
{
    const double Fs = 100;//How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs;//# At what intervals time points are sampled
    double f = 4;//frequency
    int chuck_size = 64; // chunk size (N / 64=64 chunks)
    Complex chuck[64];
    int j = 0;
    int counter = 0;
    double max_dBm = 0.0;
    for (int i = 0; i < N; i++)
    {
        t[i] = i * T;
        in[i] = { (0.7 * cos(2 * M_PI * f * t[i])), (0.7 * sin(2 * M_PI * f * t[i])) };// generate (complex) sine waveform
        double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (64)));//Hanning Window
        in[i] = multiplier * in[i];
        chuck[j] = in[i]; // take every value and put it in chunk until 64
        //compute FFT for each chunk
        if (i + 1 == chuck_size) // for each set of 64 chunks (i.e. 64,128,192), apply FFT and save it all in a 1d array (magnitude)
        {
            f = f + 1.5;
            chuck_size += 64;
            CArray data(chuck, 64); // Apply fft for 64 chunk
            fft(data);
            j = 0;
            for (int h = 0; h < 64; h++)
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
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            dBm[i][j] = (30 + (20 * log10(magnitude[counter]))) / max_dBm; // Normalize output
            counter++;
        }
    }

    // GUI Stuff
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitWindowSize(720, 640);   // Set the window's initial width & height
    glutCreateWindow("640x640 spectrogram (based on 64x64 matrix)"); // Create a window with the given title
    glutDisplayFunc(displaySpectrogram); // Register display callback handler for window re-paint
    glutMainLoop();           // Enter the event-processing loop
    return 0;
}
