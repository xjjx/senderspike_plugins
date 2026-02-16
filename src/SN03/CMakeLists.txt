juce_add_plugin(SN03
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN3J

    FORMATS LV2 VST3 VST
    PRODUCT_NAME "SN03-J-Tape-Recorder"
    PLUGIN_NAME "SN03-J Tape Recorder"
    VERSION 1.3.10

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

	LV2URI "urn:signalsnoise:sn03"
)

juce_add_binary_data(SN03_BinaryData
    SOURCES
        ../graphics/sn03g/sn03g_bk.png # background
        ../graphics/sn01g/sn01g_b1.png # large knob
        ../graphics/sn01g/sn01g_b2.png # small knob
        ../graphics/sn01g/sn01g_s1.png # switch
        ../graphics/sn02g/sn02g_v1.png # vu needle
        ../graphics/sn02g/sn02g_p1.png # peak needle
#        ../graphics/sn02g/sn02g_l1.png # peak led
        ../graphics/sn03g/sn03g_b1.png # headroom switch
        ../graphics/sn03g/sn03g_b2.png # on/off switch
        ../graphics/sn03g/sn03g_s1.png # 3-way switch
        ../graphics/sn04g/sn04g_s3.png # "A" button
)

target_link_libraries(SN03 PRIVATE SN03_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN03
    PRIVATE
        SignalNoiseTapedeck.cpp
        SignalNoiseTapedeckGUI.cpp
)

# ----------------------
# Include directories
# ----------------------
target_include_directories(SN03
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN03
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ----------------------
# Link JUCE modules
# ----------------------
target_link_libraries(SN03
    PRIVATE
        SN
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
)

set_target_properties(SN03 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
