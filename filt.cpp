#include "filt.h"

#define ECODE(x) {m_error_flag = x; return;}

// Handles LPF and HPF case
Filter::Filter(filterType filt_t, int num_taps, float Fs, float Fx)
{
	m_error_flag = 0;
	m_filt_t = filt_t;
	m_num_taps = num_taps;
	m_Fs = Fs;
	m_Fx = Fx;
	m_lambda = M_PI * Fx / (Fs/2);

	if( Fs <= 0 ) ECODE(-1);
	if( Fx <= 0 || Fx >= Fs/2 ) ECODE(-2);
	if( m_num_taps <= 0 || m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(-3);

	m_taps = m_sr = NULL;
	m_taps = (float*)malloc( m_num_taps * sizeof(float) );
	m_sr = (float*)malloc( m_num_taps * sizeof(float) );
	if( m_taps == NULL || m_sr == NULL ) ECODE(-4);
	
	init();

	if( m_filt_t == LPF ) designLPF();
	else if( m_filt_t == HPF ) designHPF();
	else ECODE(-5);

	return;
}

// Handles BPF case
Filter::Filter(filterType filt_t, int num_taps, int Fs, int Fl,
               int Fu)
{
	m_error_flag = 0;
	m_filt_t = filt_t;
	m_num_taps = num_taps;
	m_Fs = Fs / 1000;
	m_Fx = Fl / 1000;
	m_Fu = Fu / 1000;
	m_lambda = M_PI * Fl / (Fs/2);
	m_phi = M_PI * Fu / (Fs/2);

	if( Fs <= 0 ) ECODE(-10);
	if( Fl >= Fu ) ECODE(-11);
	if( Fl <= 0 || Fl >= Fs/2 ) ECODE(-12);
	if( Fu <= 0 || Fu >= Fs/2 ) ECODE(-13);
	if( m_num_taps <= 0 || m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(-14);

	m_taps = m_sr = NULL;
	m_taps = (float*)malloc( m_num_taps * sizeof(float) );
	m_sr = (float*)malloc( m_num_taps * sizeof(float) );
	if( m_taps == NULL || m_sr == NULL ) ECODE(-15);
	
	init();

	if( m_filt_t == BPF ) designBPF();
	else ECODE(-16);

	return;
}

Filter::~Filter()
{
	if( m_taps != NULL ) free( m_taps );
	if( m_sr != NULL ) free( m_sr );
}

void 
Filter::designLPF()
{
	int n;
	float mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = m_lambda / M_PI;
		else m_taps[n] = sin( mm * m_lambda ) / (mm * M_PI);
	}

	return;
}

void 
Filter::designHPF()
{
	int n;
	float mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = 1.0 - m_lambda / M_PI;
		else m_taps[n] = -sin( mm * m_lambda ) / (mm * M_PI);
	}

	return;
}

void 
Filter::designBPF()
{
	int n;
	float mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = (m_phi - m_lambda) / M_PI;
		else m_taps[n] = (   sin( mm * m_phi ) -
		                     sin( mm * m_lambda )   ) / (mm * M_PI);
	}

	return;
}

void 
Filter::get_taps(float* taps)
{
	int i;

	if (m_error_flag != 0) return;

	for (i = 0; i < m_num_taps; i++) taps[i] = m_taps[i];

	return;
}

void 
Filter::init()
{
	int i;

	if( m_error_flag != 0 ) return;

	for(i = 0; i < m_num_taps; i++) m_sr[i] = 0;

	return;
}

float 
Filter::do_sample(float data_sample)
{
	int i;
	float result;

	if( m_error_flag != 0 ) return(0);

	for(i = m_num_taps - 1; i >= 1; i--){
		m_sr[i] = m_sr[i-1];
	}	
	m_sr[0] = data_sample;

	result = 0;
	for(i = 0; i < m_num_taps; i++) result += m_sr[i] * m_taps[i];

	return result;
}
