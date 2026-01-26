#include "sn_02g.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "vstgui/lib/vstguiinit.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    // Initialize VSTGUI for Linux **without crashing**
    // LinuxFactory will automatically detect the module if you pass nullptr
    VSTGUI::init(nullptr);

    // Create your plugin instance
    SignalNoiseVUMeter* fx = new SignalNoiseVUMeter(audioMaster);
    return fx->getAeffect();
}
