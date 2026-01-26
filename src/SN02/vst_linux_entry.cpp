#include "sn_02g.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseVUMeter* fx = new SignalNoiseVUMeter(audioMaster);
    return fx->getAeffect();
}
