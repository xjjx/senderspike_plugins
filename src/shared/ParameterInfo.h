#pragma once

#include <string>
#include "sn_core.h"

struct ParameterInfo
{
	float minDb;
	float maxDb;
	float defaultDb;
	std::string paramID;
	std::string unit;

	float rangeDb() const noexcept {
		return maxDb - minDb;
	}

	float normalizedToDb (float norm) const noexcept {
		return norm * rangeDb() + minDb;
	}

	float dbToNormalized (float db) const noexcept {
		return (db - minDb) / rangeDb();
	}

	float normalizedToLinear (float norm) const noexcept {
		return dB2lin(normalizedToDb(norm));
	}
};

struct ParamDesc
{
	const char* id;			  // JUCE parameter ID
	const char* name;		  // display name
	const char* unit;		  // "dB", "Hz", "n/y", etc.
	float defaultNorm;		  // normalized [0..1]
};
