#pragma once

#include <string>
#include "sn_core.h"

struct ParameterInfo
{
	float minDb;
	float maxDb;
	float defaultDb;
	std::string paramID;

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
