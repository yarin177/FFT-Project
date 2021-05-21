#define _USE_MATH_DEFINES
#include <GL/glut.h>


#include <stdio.h>
#include <iostream>
#include <cmath>
#include "fft.h"


using namespace std;

const int N = 1000;
Complex in[N];
double test[N];
double t[N];//time vector 
float frequencies[N];
double mag[N];


void drawText(float x, float y, char *string) {
    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}

void display()
{
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH); glLineWidth(2);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-6, 6, -6, 6, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glColor4f(0.4, 1.0, 0.6, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < N; i++)
    {
        glVertex2f(t[i] - 5, test[i]);
    }
    glEnd();
    glutSwapBuffers();
}

void displayFreqSpec()
{
    char str[] = "hello";
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH); glLineWidth(2);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 6, 0, 6, -1, 1);
    glScalef(0.1, 0.03, 0.1);
    glRotatef(0, 0, 0, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glColor4f(0.4, 1.0, 0.6, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < N / 2; i++)
    {
        glVertex2f(frequencies[i] + 5, mag[i]);
    }
    glEnd();
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    int i;
    double y;
    const double Fs = 100;//How many time points are needed i,e., Sampling Frequency
    const double  T = 1 / Fs;//# At what intervals time points are sampled
    const double f = 4;//frequency

    for (int i = 0; i < N; i++)
    {
        t[i] = i * T;
        in[i] = { (0.7 * cos(2 * M_PI * 15 * t[i])), (0.7 * sin(2 * M_PI * f * t[i])) };// generate (complex) sine waveform
        //double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (N)));//Hanning Window
        //in[i] = multiplier * in[i];
        test[i] = in[i].real() + in[i].imag();
    }
    CArray data(in, N);


    printf("\n");
    printf("  Input Data(only the imag value):\n");
    printf("\n");

    for (i = 0; i < N; i++)
    {
        printf("%4d  %12f\n", i, in[i].real()); 
    }


    fft(data);

    /*
    printf("output Data from FFT implementation:\n");

    for (int i = 0; i < N; ++i)
    {
        std::cout << data[i] << std::endl;
    }

    */

   printf("\n");
   printf("log magnitude of frequency domain components :\n");
    for (i = 0; i < N; i++)
    {
        mag[i] = std::abs(data[i]);
        std::cout << mag[i] << endl;
    }

    //might need this part to calculate spectrogram
    int tpCount = N;
    int values[1000];

    for (int i = 0; i < 1000; i++)
        values[i] = i;
    float timePeriod = tpCount / Fs;

    //printf("\n");

    for (int i = 0; i < 1000; i++)
    {
        frequencies[i] = values[i] / timePeriod;
    }

    // GUI Stuff
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);

    glutCreateWindow("Graph showing the waveform after hanning window");
    //glutDisplayFunc(display);
    glutDisplayFunc(displayFreqSpec);
    glutMainLoop();
    return 0;
}
