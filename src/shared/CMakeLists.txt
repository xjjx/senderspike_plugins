add_library(SN STATIC
    sn_core.cpp
	SignalNoiseFX.cpp
    SignalNoiseKnobLookAndFeel.cpp
    SignalNoiseKnobPrecise.cpp
    SignalNoiseKnob.cpp
    SignalNoisePeakMeter.cpp
    SignalNoiseKnobLabel.cpp
    SignalNoiseFilmstripLNF.cpp
    SignalNoiseGR.cpp
)

juce_add_binary_data(SN_BinaryData
    NAMESPACE SN
    SOURCES
        ../graphics/sn06g/sn06g_vu.png
)

target_link_libraries(SN PRIVATE SN_BinaryData)

set_target_properties(SN PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Add include directories (so #include <juce_xxx/juce_xxx.h> works)
target_include_directories(SN
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}           # for shared headers
        ${JUCE_INCLUDE_DIRS}                  # usually set by JUCE
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules that are used in shared code
target_link_libraries(SN
    PUBLIC
        juce::juce_audio_processors
        juce::juce_gui_basics
)

target_include_directories(SN PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
