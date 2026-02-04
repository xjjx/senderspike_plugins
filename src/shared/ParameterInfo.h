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

enum class ParamType
{
	Normalized,	// legacy 0..1
	Decibel,	// linear gain in dB
	Choice		// generic stepped parameter (EQ frequencies, etc)
};

struct ParamDesc
{
	ParamType type;		// type of parameter
	const char* id;		// unique host/preset ID
	const char* name;	// display name
	const char* unit;	// "dB", "ms", "", etc

	// only for continuous params
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float defaultValue = 0.0f;

	std::vector<const char*> choiceLabels{};	// non-empty -> choice param
};
