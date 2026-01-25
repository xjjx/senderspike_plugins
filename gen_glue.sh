#!/bin/bash

# Define plugins and their class names
declare -A classes=(
    [SN01]="SignalNoiseCompressor"
    [SN03]="SignalNoiseTapedeck"
    [SN04]="SignalNoiseEqualizer"
    [SN05]="SignalNoiseLimiter"
    [SN06]="SignalNoiseOpamp"
)

# Generate vst_linux_entry.cpp for each plugin
for plugin in "${!classes[@]}"; do
    class=${classes[$plugin]}
    header=$(basename sources/$plugin/*e.h)
    glue_file="sources/$plugin/vst_linux_entry.cpp"

    cat > "$glue_file" <<EOL
#include "$header"
#include "public.sdk/source/vst2.x/audioeffectx.h"

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    $class* fx = new $class(audioMaster);
    return fx->getAeffect();
}
EOL

    echo "Generated $glue_file"
done

