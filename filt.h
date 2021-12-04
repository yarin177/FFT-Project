#ifndef _FILTER_H
#define _FILTER_H

#define MAX_NUM_FILTER_TAPS 1000
#pragma warning(disable:4996)

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>

enum filterType {LPF, HPF, BPF};

class Filter{
	private:
		filterType m_filt_t;
		int m_num_taps;
		int m_error_flag;
		float m_Fs;
		float m_Fx;
		float m_lambda;
		float *m_taps;
		float *m_sr;
		void designLPF();
		void designHPF();

		// Only needed for the bandpass filter case
		float m_Fu, m_phi;
		void designBPF();

	public:
		Filter(filterType filt_t, int num_taps, float Fs, float Fx);
		Filter(filterType filt_t, int num_taps, int Fs, int Fl, int Fu);
		~Filter();
		void init();
		float do_sample(float data_sample);
		int get_error_flag(){return m_error_flag;};
		void get_taps( float *taps );
};

#endif
