#include "sn_05e.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseLimiter* fx = new SignalNoiseLimiter(audioMaster);
    return fx->getAeffect();
}
