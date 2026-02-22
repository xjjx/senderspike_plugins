#pragma once
#include <cmath>

class VUFilter
{
public:
	static constexpr double kWn = 13.5119;
	static constexpr double kEt = 0.8127;

	VUFilter() { reset(); }

	void prepare(double sampleRate)
	{
		reset();
		const double Td  = 1.0 / sampleRate;
		const double Td2 = Td * Td;
		const double wn2 = kWn * kWn;

		double a0 = 4.0 + 4.0 * kEt * kWn * Td + wn2 * Td2;
		_a1 = (-8.0 + 2.0 * wn2 * Td2)                    / a0;
		_a2 = ( 4.0 - 4.0 * kEt * kWn * Td + wn2 * Td2)   / a0;
		_b0 = (wn2 * Td2)                                  / a0;
		_b1 = 2.0 * _b0;
		_b2 = _b0;
	}

	double process(double x0)
	{
		double y0 = _b0 * x0 + _b1 * _x1 + _b2 * _x2 - _a1 * _y1 - _a2 * _y2;
		_x2 = _x1;
		_x1 = x0;
		_y2 = _y1;
		_y1 = y0;

		return y0;
	}

	void reset()
	{
		_x1 = _x2 = _y1 = _y2 = 0.0;
		_a1 = _a2 = _b0 = _b1 = _b2 = 0.0;
	}

private:
	double _a1, _a2;
	double _b0, _b1, _b2;
	double _x1, _x2;   // input history  (x0 is the live sample)
	double _y1, _y2;   // output history (y0 is returned directly)
};
