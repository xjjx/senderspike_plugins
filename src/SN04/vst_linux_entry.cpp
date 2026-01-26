#include "sn_04e.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseEqualizer* fx = new SignalNoiseEqualizer(audioMaster);
    return fx->getAeffect();
}
