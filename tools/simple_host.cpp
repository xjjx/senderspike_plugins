#include <cstdio>
#include <dlfcn.h>

#include "aeffect.h"

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

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "Usage: %s plugin.so\n", argv[0]);
        return 1;
    }

    void* handle = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!handle)
    {
        std::fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }

    typedef AEffect* (*VSTMainFn)(audioMasterCallback);

    VSTMainFn mainFn = nullptr;
    mainFn = (VSTMainFn)dlsym(handle, "VSTPluginMain");
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
    std::printf("Inputs: %d Outputs: %d\n",
        effect->numInputs,
        effect->numOutputs);

    if (effect->dispatcher)
        effect->dispatcher(effect, effOpen, 0, 0, nullptr, 0);

    if (effect->dispatcher)
        effect->dispatcher(effect, effClose, 0, 0, nullptr, 0);

    dlclose(handle);
    return 0;
}

