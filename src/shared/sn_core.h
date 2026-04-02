//------------------------------------------------------------------------------------
//
//	file:		sn_core.h
//
//	purpose:	DSP core include
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_CORE_H
#define _SN_CORE_H

#include <juce_audio_processors/juce_audio_processors.h>

//------------------------------------------------------------------------------------
// types & consts
//------------------------------------------------------------------------------------

typedef unsigned int uint;
typedef unsigned long dword;

//------------------------------------------------------------------------------------

#define DC_OFFSET			1.0e-25
#define M_LN_2				0.69314718055994530941723212145818		// ln(2)
#define M_LN10_20			0.11512925464970228420089957273422		// ln(10) / 20
#define M_LN10_40			0.057564627324851142100449786367109		// ln(10) / 40
#define M_LN_M120			13.815510557964274104107948728106		// -ln(0.000001) => -120dB

//------------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------------

#define dB2lin( v )			(exp((v) * M_LN10_20))
#define dB2lsq( v )			(exp((v) * M_LN10_40))	// = sqrt(dB2lin(v))

//------------------------------------------------------------------------------------
// first-order LPF
//------------------------------------------------------------------------------------

class foLPF
{
private:
	double b0, b1, a1, x1, y1;
public:
	foLPF();
	void clear();
	void reset();
	void setup(double Fc, double Fs);
	void invrs(double Fc, double Fs);
	//runtime
	inline double run(double x) {
		double y0 = b0 * x + b1 * x1 - a1 * y1;
		x1 = x;
		return y1 = y0;
	}
};

//------------------------------------------------------------------------------------
// first-order HPF
//------------------------------------------------------------------------------------

class foHPF
{
private:
	double b0, b1, a1, x1, y1;
public:
	foHPF();
	void clear();
	void reset();
	void setup(double Fc, double Fs);
	void invrs(double Fc, double Fs);
	//runtime
	inline double run(double x) {
		double y0 = b0 * x + b1 * x1 - a1 * y1;
		x1 = x;
		return y1 = y0;
	}
};

//------------------------------------------------------------------------------------
// first-order low shelf
//------------------------------------------------------------------------------------

class foLSF
{
private:
	double b0, a1, y1;
public:
	foLSF();
	void clear();
	void reset();
	void setup(double g, double Fc, double Fs);
	//runtime
	inline double run(double x) {
		double x1 = x - a1 * y1;
		double y0 = a1 * x1 + y1;
		y1 = x1;
		return b0 * (x + y0) + x;
	}
};

//------------------------------------------------------------------------------------
// first-order high shelf
//------------------------------------------------------------------------------------

class foHSF
{
private:
	double b0, a1, y1;
public:
	foHSF();
	void clear();
	void reset();
	void setup(double g, double Fc, double Fs);
	//runtime
	inline double run(double x) {
		double x1 = x - a1 * y1;
		double y0 = a1 * x1 + y1;
		y1 = x1;
		return b0 * (x - y0) + x;
	}
};

//------------------------------------------------------------------------------------
// biquad filters - based on algorithms by (c) 2005 Robert Bristow-Johnson
// as found in 'Cookbook formulae for audio EQ biquad filter coefficients'
// https://www.musicdsp.org/en/latest/Filters/197-rbj-audio-eq-cookbook.html
//------------------------------------------------------------------------------------

enum biquad_e { LPF = 0, HPF, BPF, BSF, APF, PKF, LSF, HSF };

//------------------------------------------------------------------------------------
// special Qs - use with setup_q()
static const double qf12 = 0.7071;			// sqrt(0.5) => single LPF/HPF (12dB/oct)
static const double qf18 = 1;				// foHPF/foLPF into HPF/LPF  (18dB/oct)
static const double qf24a = 1 / 1.8478;		// 1st of two LPF/HPF in series (24dB/oct)
static const double qf24b = 1 / 0.7654;		// 2nd of two LPF/HPF in series (24dB/oct)

//------------------------------------------------------------------------------------

class biquad
{
private:
	double cc;				// validity check
	double a0, a1, a2;		// coefficients (zeros)
	double b0, b1, b2;		// coefficients (poles)
	double x1, x2, y1, y2;	// previous values
private:
	void setup_lpf(double cw0, double alf);
	void setup_hpf(double cw0, double alf);
	void setup_bpf(double cw0, double alf);
	void setup_bsf(double cw0, double alf);
	void setup_apf(double cw0, double alf);
	void setup_pkf(double A, double cw0, double alf);
	void setup_lsf(double A, double cw0, double alf);
	void setup_hsf(double A, double cw0, double alf);
public:
	//create & destroy
	biquad();
	//runtime
	inline double run(double v) {
		double y0 = b0 * v + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
		x2 = x1;
		y2 = y1;
		x1 = v;
		return y1 = y0;
	}
	inline bool valid() {	//<- filter is set up
		return cc != 0.0;
	}
	void clear();
	void reset();
	double getmag(double Fc, double Fs);
	//setup
	void setup_q(biquad_e ft, double dB, double q, double Fc, double Fs);
	void setup_s(biquad_e ft, double dB, double s, double Fc, double Fs);
	void setup_bw(biquad_e ft, double dB, double bw, double Fc, double Fs);
};

//------------------------------------------------------------------------------------
// noise generator - original code by (c) 2006 Andrew Simper, based on algorithms
// by Allan Herriman, James McCartney, Phil Burk, Paul Kellet and Robin Whittle
// for details see info @ http://www.firstpr.com.au/dsp/pink-noise/
//------------------------------------------------------------------------------------

#define PINK_MAX	16

//------------------------------------------------------------------------------------

class noise
{
private:
	union {
		uint  iw;
		float fw;
	};
	uint	_seed;					// PRNG seed
	uint	_count;					// hash counter				
	float	_pink;					// last pink
	float	_brown;					// last brown
	float	_pinks[PINK_MAX];		// pink storage
private:
	int CTZ(int num);
public:
	//create & destroy
	noise();
	//methods
	void seed(dword s = 0);			// set PRNG seed
	float white(float scl = 0.5f);	// get white noise number [-scl..scl]
	float pink();					// get pink noise number [-0.5..0.5]
	float brown();					// get brown noise number [-0.5..0.5]
};

//------------------------------------------------------------------------------------


#endif // _SN_CORE_H
