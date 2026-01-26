#include "sn_03e.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    SignalNoiseTapedeck* fx = new SignalNoiseTapedeck(audioMaster);
    return fx->getAeffect();
}
