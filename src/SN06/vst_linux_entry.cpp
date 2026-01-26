#include "sn_06e.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseOpamp* fx = new SignalNoiseOpamp(audioMaster);
    return fx->getAeffect();
}
