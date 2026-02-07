#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <dlfcn.h>
#include <vector>

#define VST_2_3_EXTENSIONS 1
#define VST_2_4_EXTENSIONS 1

#include "aeffect.h"
#include "aeffectx.h"

// -------------------- Host callback --------------------
static VstIntPtr VSTCALLBACK hostCallback(
	AEffect* effect,
	VstInt32 opcode,
	VstInt32 index,
	VstIntPtr value,
	void* ptr,
	float opt)
{
	switch (opcode)
	{
		case audioMasterVersion:
			return 2400;  // VST 2.4
		default:
			return 0;
	}
}

int run(const char* libpath)
{
	std::fprintf(stderr, "dlopen try %s: %s\n", libpath, dlerror());

	void* handle = dlopen(libpath, RTLD_NOW | RTLD_LOCAL);
	if (!handle) {
		std::fprintf(stderr, "dlopen failed for %s: %s\n", libpath, dlerror());
		return 1;
	}

	typedef AEffect* (*VSTMainFn)(audioMasterCallback);

	VSTMainFn mainFn = (VSTMainFn)dlsym(handle, "VSTPluginMain");
	if (!mainFn)
		mainFn = (VSTMainFn)dlsym(handle, "VSTMain");

	if (!mainFn)
	{
		std::fprintf(stderr, "No VST entry point found\n");
		dlclose(handle);
		return 1;
	}

	AEffect* effect = mainFn(hostCallback);

	if (!effect)
	{
		std::fprintf(stderr, "VSTPluginMain returned NULL\n");
		dlclose(handle);
		return 1;
	}

	if (effect->magic != kEffectMagic)
	{
		std::fprintf(stderr, "Invalid effect magic: 0x%x\n", effect->magic);
		dlclose(handle);
		return 1;
	}

	std::printf("Plugin loaded OK\n");
	std::printf("Inputs: %d Outputs: %d\n", effect->numInputs, effect->numOutputs);

	bool supportsDouble = (effect->flags & effFlagsCanDoubleReplacing) != 0;
	std::printf("Support double precission: %s\n", supportsDouble ? "true" : "false");

	if (effect->dispatcher)
	{
		effect->dispatcher(effect, effOpen, 0, 0, nullptr, 0);

	// ------------------- Enumerate parameters -------------------
		const int numParams = effect->numParams;
		std::printf("Plugin has %d parameters:\n", numParams);

		char name[256] = {};
		for (int i = 0; i < numParams; ++i)
		{
			effect->dispatcher(effect, effGetParamName, i, 0, name, 0.0f);

			float value = effect->getParameter(effect, i);

			std::printf("  Param %d: '%s' = %f\n", i, name, value);
		}
	}

	// ===================== Simple audio test =====================
	const int blockSize = 64;
	const int numBlocks = 10;

	float inL[blockSize] = {0};
	float inR[blockSize] = {0};
	float outL[blockSize] = {0};
	float outR[blockSize] = {0};

	float* inputs[2] = { inL, inR };
	float* outputs[2] = { outL, outR };

	// Fill input with a test sine wave
	for (int i = 0; i < blockSize; ++i)
	{
		inL[i] = std::sin(2.0 * M_PI * 440.0 * i / 44100.0f);
		inR[i] = std::sin(2.0 * M_PI * 440.0 * i / 44100.0f);
	}

	// ------------------- Initialize plugin -------------------
	if (effect->dispatcher)
	{
		effect->dispatcher(effect, effSetSampleRate, 0, 0, nullptr, 44100.0f);
		effect->dispatcher(effect, effSetBlockSize, 0, blockSize, nullptr, 0);
		effect->dispatcher(effect, effMainsChanged, 0, 1, nullptr, 0); 
		effect->dispatcher(effect, effStartProcess, 0, 0, nullptr, 0);
	}

	// ------------------- Process audio blocks -------------------
	for (int b = 0; b < numBlocks; ++b)
	{
		if (effect->processReplacing)
			effect->processReplacing(effect, inputs, outputs, blockSize);

		// Print first 8 samples of output
		std::printf("Block %d output L:", b);
		for (int i = 0; i < 8 && i < blockSize; ++i)
			std::printf(" %f", outL[i]);
		std::printf("\n");

		std::printf("Block %d output R:", b);
		for (int i = 0; i < 8 && i < blockSize; ++i)
			std::printf(" %f", outR[i]);
		std::printf("\n");
	}

	// ------------------- Finish -------------------
	if (effect->dispatcher)
	{
		effect->dispatcher(effect, effStopProcess, 0, 0, nullptr, 0);
		effect->dispatcher(effect, effMainsChanged, 0, 0, nullptr, 0);
		effect->dispatcher(effect, effClose, 0, 0, nullptr, 0);
	}

	dlclose(handle);

	return 1;
}

// -------------------- Main host --------------------
int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::fprintf(stderr, "Usage: %s plugin.so\n", argv[0]);
		return 1;
	}

	// Load all plugins from command line arguments
	for (int i = 1; i < argc; ++i) {
		run(argv[i]);
	}

	return 0;
}

