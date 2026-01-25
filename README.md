# Sender Spike Plugins

1. Get and unpack sources from
https://senderspike.wordpress.com/2026/01/20/sn-mixing-tools-source-code/

2. Get vst 2.4 sdk
git clone https://github.com/R-Tur/VST_SDK_2.4.git

3. Fix some simple issues like adding this when compiler complains
#include <cstdlib>

4. Create glue files
./gen_glue.sh

4. Compile
make -f Makefile.nogui
