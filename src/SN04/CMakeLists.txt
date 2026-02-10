juce_add_plugin(SN04
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN4J

    FORMATS LV2 VST3 VST
    PRODUCT_NAME "SN04-J-Channel-EQ"
    PLUGIN_NAME "SN04-J Channel EQ"
    VERSION 1.2.10

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

	LV2URI "urn:signalsnoise:sn04"
)

juce_add_binary_data(SN04_BinaryData
    SOURCES
        ../graphics/sn04g/sn04g_bk.png # background
        ../graphics/sn04g/sn04g_b0.png # inner knob
        ../graphics/sn04g/sn04g_b1.png # rim knob
        ../graphics/sn04g/sn04g_b2.png # HPF knob
        ../graphics/sn04g/sn04g_b3.png # LPF knob
        ../graphics/sn04g/sn04g_b4.png # gain knob
        ../graphics/sn03g/sn03g_b2.png # 2-way sliding switch
        ../graphics/sn04g/sn04g_b5.png # 4-way sliding switch
        ../graphics/sn04g/sn04g_s1.png # bell switch button
        ../graphics/sn04g/sn04g_s2.png # phase switch button
        ../graphics/sn04g/sn04g_ld.png # on/off led
        ../graphics/sn04g/sn04g_pk.png # peak led
        ../graphics/sn04g/sn04g_s3.png # "A" button
)

target_link_libraries(SN04 PRIVATE SN04_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN04
    PRIVATE
        SignalNoiseEqualizer.cpp
        SignalNoiseEqualizerGUI.cpp
)

# ----------------------
# Include directories
# ----------------------
target_include_directories(SN04
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN04
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ----------------------
# Link JUCE modules
# ----------------------
target_link_libraries(SN04
    PRIVATE
        SN
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
)

set_target_properties(SN04 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
