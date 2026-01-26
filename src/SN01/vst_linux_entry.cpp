#include "sn_01e.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseCompressor* fx = new SignalNoiseCompressor(audioMaster);
    return fx->getAeffect();
}
