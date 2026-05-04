//------------------------------------------------------------------------------------
//
//	file:		sn_core.cpp
//
//	purpose:	DSP core include
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <sn_core.h>

//------------------------------------------------------------------------------------
// first-order LPF
//------------------------------------------------------------------------------------

foLPF::foLPF()
{
	reset();
}

//------------------------------------------------------------------------------------

void foLPF::clear()
{
	x1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foLPF::reset()
{
	b0 = 1;
	b1 = a1 = x1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foLPF::setup(double Fc, double Fs)
{
	double A = 1 / tan(juce::MathConstants<double>::pi * Fc / Fs);
	b0 = 1 / (1 + A);
	a1 = (1 - A) / (1 + A);
	b1 = b0;
}

//------------------------------------------------------------------------------------

void foLPF::invrs(double Fc, double Fs)
{
	double A = 1 / tan(juce::MathConstants<double>::pi * Fc / Fs);
	b0 = 1;
	a1 = 1 / (1 + A);
	b1 = (1 - A) / (1 + A);
}

//------------------------------------------------------------------------------------
// first-order HPF
//------------------------------------------------------------------------------------

foHPF::foHPF()
{
	reset();
}

//------------------------------------------------------------------------------------

void foHPF::clear()
{
	x1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foHPF::reset()
{
	b0 = 1;
	b1 = a1 = x1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foHPF::setup(double Fc, double Fs)
{
	double A = 1 / tan(juce::MathConstants<double>::pi * Fc / Fs);
	b0 = A / (1 + A);
	a1 = (1 - A) / (1 + A);
	b1 = -b0;
}

//------------------------------------------------------------------------------------

void foHPF::invrs(double Fc, double Fs)
{
	double w = juce::MathConstants<double>::pi * Fc / Fs;
	double A = 1 / tan(w);
	b0 = 1 + w;
	a1 = -(A / (1 + A));
	b1 = (1 - A) / (1 + A);
}

//------------------------------------------------------------------------------------
// first-order low shelf
//------------------------------------------------------------------------------------

foLSF::foLSF()
{
	reset();
}

//------------------------------------------------------------------------------------

void foLSF::clear()
{
	y1 = 0;
}

//------------------------------------------------------------------------------------

void foLSF::reset()
{
	b0 = a1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foLSF::setup(double g, double Fc, double Fs)
{
	double wc = tan(juce::MathConstants<double>::pi * Fc / Fs);
	double v0 = dB2lin(g);
	double h0 = v0 - 1;

	a1 = g >= 0 ? (wc - 1) / (wc + 1) : (wc - v0) / (wc + v0);
	b0 = h0 * 0.5;
}

//------------------------------------------------------------------------------------
// first-order high shelf
//------------------------------------------------------------------------------------

foHSF::foHSF()
{
	reset();
}

//------------------------------------------------------------------------------------

void foHSF::clear()
{
	y1 = 0;
}

//------------------------------------------------------------------------------------

void foHSF::reset()
{
	b0 = a1 = y1 = 0;
}

//------------------------------------------------------------------------------------

void foHSF::setup(double g, double Fc, double Fs)
{
	double wc = tan(juce::MathConstants<double>::pi * Fc / Fs);
	double v0 = dB2lin(g);
	double h0 = v0 - 1;

	a1 = g >= 0 ? (wc - 1) / (wc + 1) : (v0 * wc - 1) / (v0 * wc + 1);
	b0 = h0 * 0.5;
}

//------------------------------------------------------------------------------------
// class biquad
//------------------------------------------------------------------------------------

biquad::biquad()
{
	reset();
}

//------------------------------------------------------------------------------------

void biquad::clear()
{
	x1 = x2 = y1 = y2 = 0;
}

//------------------------------------------------------------------------------------

void biquad::reset()
{
	a0 = a1 = a2 = b1 = b2 = x1 = x2 = y1 = y2 = 0;
	b0 = 1;
	cc = 0.0;
}

//------------------------------------------------------------------------------------

double biquad::getmag(double Fc, double Fs)
{
	double w = juce::MathConstants<double>::twoPi * Fc / Fs;

	double c1 = cos(-1 * w);
	double c2 = cos(-2 * w);
	double s1 = sin(-1 * w);
	double s2 = sin(-2 * w);

	double rZ = b0 + b1 * c1 + b2 * c2;		// zeros - real
	double iZ = b1 * s1 + b2 * s2;			// zeros - imaginary
	double rP = 1. + a1 * c1 + a2 * c2;		// poles - real
	double iP = a1 * s1 + a2 * s2;			// poles - imaginary

	double d = rP * rP + iP * iP;

	double rH = (rZ * rP + iZ * iP) / d;
	double iH = (iZ * rP - rZ * iP) / d;

	return sqrt(rH * rH + iH * iH);			//<- linear magnitude
	//return atan2(iH, rH);					//<- phase
}

//------------------------------------------------------------------------------------

void biquad::setup_lpf(double cw0, double alf)
{
	//low pass -> H(s) = 1 / (s^2 + s/Q + 1)

	b0 = (1 - cw0) / 2;
	b1 = 1 - cw0;
	b2 = (1 - cw0) / 2;
	a0 = 1 + alf;
	a1 = -2 * cw0;
	a2 = 1 - alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_hpf(double cw0, double alf)
{
	//high pass -> H(s) = s^2 / (s^2 + s/Q + 1)

	b0 = (1 + cw0) / 2;
	b1 = -(1 + cw0);
	b2 = (1 + cw0) / 2;
	a0 = 1 + alf;
	a1 = -2 * cw0;
	a2 = 1 - alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_bpf(double cw0, double alf)
{
	//band pass -> H(s) = (s/Q) / (s^2 + s/Q + 1)      (constant 0 dB peak gain)

	b0 = alf;
	b1 = 0;
	b2 = -alf;
	a0 = 1 + alf;
	a1 = -2 * cw0;
	a2 = 1 - alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_bsf(double cw0, double alf)
{
	//band stop -> H(s) = (s^2 + 1) / (s^2 + s/Q + 1)

	b0 = 1;
	b1 = -2 * cw0;
	b2 = 1;
	a0 = 1 + alf;
	a1 = -2 * cw0;
	a2 = 1 - alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_apf(double cw0, double alf)
{
	//all pass -> H(s) = (s^2 - s/Q + 1) / (s^2 + s/Q + 1)

	b0 = 1 - alf;
	b1 = -2 * cw0;
	b2 = 1 + alf;
	a0 = 1 + alf;
	a1 = -2 * cw0;
	a2 = 1 - alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_pkf(double A, double cw0, double alf)
{
	//peak/bell -> H(s) = (s^2 + s*(A/Q) + 1) / (s^2 + s/(A*Q) + 1)

	b0 = 1 + alf * A;
	b1 = -2 * cw0;
	b2 = 1 - alf * A;
	a0 = 1 + alf / A;
	a1 = -2 * cw0;
	a2 = 1 - alf / A;
}

//------------------------------------------------------------------------------------

void biquad::setup_lsf(double A, double cw0, double alf)
{
	//low shelf -> H(s) = A * (s^2 + (sqrt(A)/Q)*s + A)/(A*s^2 + (sqrt(A)/Q)*s + 1)

	double Ap1 = A + 1;
	double An1 = A - 1;
	double sqA = sqrt(A);

	b0 = A * (Ap1 - An1 * cw0 + 2 * sqA * alf);
	b1 = 2 * A * (An1 - Ap1 * cw0);
	b2 = A * (Ap1 - An1 * cw0 - 2 * sqA * alf);
	a0 = Ap1 + An1 * cw0 + 2 * sqA * alf;
	a1 = -2 * (An1 + Ap1 * cw0);
	a2 = Ap1 + An1 * cw0 - 2 * sqA * alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_hsf(double A, double cw0, double alf)
{
	//high shelf ->  H(s) = A * (A*s^2 + (sqrt(A)/Q)*s + 1)/(s^2 + (sqrt(A)/Q)*s + A)

	double Ap1 = A + 1;
	double An1 = A - 1;
	double sqA = sqrt(A);

	b0 = A * (Ap1 + An1 * cw0 + 2 * sqA * alf);
	b1 = -2 * A * (An1 + Ap1 * cw0);
	b2 = A * (Ap1 + An1 * cw0 - 2 * sqA * alf);
	a0 = Ap1 - An1 * cw0 + 2 * sqA * alf;
	a1 = 2 * (An1 - Ap1 * cw0);
	a2 = Ap1 - An1 * cw0 - 2 * sqA * alf;
}

//------------------------------------------------------------------------------------

void biquad::setup_q(biquad_e ft, double dB, double q, double Fc, double Fs)
{
	double A = dB2lsq(dB);
	double w0 = juce::MathConstants<double>::twoPi * Fc / Fs;
	double cw0 = cos(w0);
	double alf = sin(w0) / (2 * q);

	switch (ft)
	{
	case LPF: setup_lpf(cw0, alf);		break;
	case HPF: setup_hpf(cw0, alf);		break;
	case BPF: setup_bpf(cw0, alf);		break;
	case BSF: setup_bsf(cw0, alf);		break;
	case APF: setup_apf(cw0, alf); 		break;
	case PKF: setup_pkf(A, cw0, alf);	break;
	case LSF: setup_lsf(A, cw0, alf);	break;
	case HSF: setup_hsf(A, cw0, alf);	break;
	}

	double ra0 = 1. / a0;
	b0 *= ra0;
	b1 *= ra0;
	b2 *= ra0;
	a1 *= ra0;
	a2 *= ra0;
	cc = 1.0;
}

//------------------------------------------------------------------------------------

void biquad::setup_s(biquad_e ft, double dB, double s, double Fc, double Fs)
{
	double A = dB2lsq(dB);
	double w0 = juce::MathConstants<double>::twoPi * Fc / Fs;
	double cw0 = cos(w0);
	double alf = sin(w0) / 2 * sqrt((A + 1 / A) * (1 / s - 1) + 2);

	switch (ft)
	{
	case LPF: setup_lpf(cw0, alf);		break;
	case HPF: setup_hpf(cw0, alf);		break;
	case BPF: setup_bpf(cw0, alf);		break;
	case BSF: setup_bsf(cw0, alf);		break;
	case APF: setup_apf(cw0, alf); 		break;
	case PKF: setup_pkf(A, cw0, alf);	break;
	case LSF: setup_lsf(A, cw0, alf);	break;
	case HSF: setup_hsf(A, cw0, alf);	break;
	}

	double ra0 = 1. / a0;
	b0 *= ra0;
	b1 *= ra0;
	b2 *= ra0;
	a1 *= ra0;
	a2 *= ra0;
	cc = 1.0;
}

//------------------------------------------------------------------------------------

void biquad::setup_bw(biquad_e ft, double dB, double bw, double Fc, double Fs)
{
	double A = dB2lsq(dB);
	double w0 = juce::MathConstants<double>::twoPi * Fc / Fs;
	double sw0 = sin(w0);
	double cw0 = cos(w0);
	double alf = sw0 * sinh(M_LN_2 / 2 * bw * w0 / sw0);

	switch (ft)
	{
	case LPF: setup_lpf(cw0, alf);		break;
	case HPF: setup_hpf(cw0, alf);		break;
	case BPF: setup_bpf(cw0, alf);		break;
	case BSF: setup_bsf(cw0, alf);		break;
	case APF: setup_apf(cw0, alf); 		break;
	case PKF: setup_pkf(A, cw0, alf);	break;
	case LSF: setup_lsf(A, cw0, alf);	break;
	case HSF: setup_hsf(A, cw0, alf);	break;
	}

	double ra0 = 1. / a0;
	b0 *= ra0;
	b1 *= ra0;
	b2 *= ra0;
	a1 *= ra0;
	a2 *= ra0;
	cc = 1.0;
}

//------------------------------------------------------------------------------------
// class noise
//------------------------------------------------------------------------------------

noise::noise()
{
	iw = 0;
	_seed = 0;
	_pink = 0;
	_count = 1;
	_brown = 0.0f;
	for (int i = 0; i < PINK_MAX; i++)
		_pinks[i] = 0.0f;
}

//------------------------------------------------------------------------------------

inline int CTZ(uint32_t x)
{
#if defined(_MSC_VER)
	unsigned long index;
	return _BitScanForward(&index, x) ? static_cast<int>(index) : 32;
#else
	return x ? __builtin_ctz(x) : 32;
#endif
}

//------------------------------------------------------------------------------------

void noise::seed(uint32_t s)
{
	_seed = s ? s : juce::Time::getMillisecondCounter();
}

//------------------------------------------------------------------------------------

float noise::white(float scl)
{
	_seed = (_seed * 196314165) + 907633515;
	iw = _seed >> 9;
	iw |= 0x40000000;
	return (fw - 3.0f) * scl;
}

//------------------------------------------------------------------------------------

float noise::pink()
{
	float pr;
	float cr;
	uint32_t k = CTZ(_count) & (PINK_MAX - 1);

	//get previous value of this octave
	pr = _pinks[k];
	while (true)
	{
		cr = white();
		//store new value 
		_pinks[k] = cr;
		cr -= pr;
		//update total
		_pink += cr;
		if (_pink < -4.0f || _pink > 4.0f)
			_pink -= cr;
		else
			break;
	}
	//update counter
	_count++;
	return (white() + _pink) * 0.125f;
}

//------------------------------------------------------------------------------------

float noise::brown()
{
	while (true)
	{
		float r = white();
		_brown += r;
		if (_brown < -8.0f || _brown > 8.0f)
			_brown -= r;
		else
			break;
	}
	return _brown * 0.0625f;
}

//------------------------------------------------------------------------------------
