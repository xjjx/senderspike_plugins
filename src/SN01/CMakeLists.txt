juce_add_plugin(SN01
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN1J

    FORMATS LV2 VST3 VST
    PRODUCT_NAME "SN01-J-VCA-Comp"
    PLUGIN_NAME "SN01-J VCA Comp"
    VERSION 1.2.10

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

	LV2URI "urn:signalsnoise:sn01"
)

juce_add_binary_data(SN01_BinaryData
    SOURCES
        ../graphics/sn01g/sn01g_bk.png # background
        ../graphics/sn01g/sn01g_b1.png
        ../graphics/sn01g/sn01g_b2.png
        ../graphics/sn01g/sn01g_b3.png
#        ../graphics/sn01g/sn01g_gr.png
        ../graphics/sn01g/sn01g_s1.png
        ../graphics/sn01g/sn01g_s3.png
)

target_link_libraries(SN01 PRIVATE SN01_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN01
    PRIVATE
        SignalNoiseCompressor.cpp
        SignalNoiseCompressorGUI.cpp
)

# ----------------------
# Include directories
# ----------------------
target_include_directories(SN01
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN01
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ----------------------
# Link JUCE modules
# ----------------------
target_link_libraries(SN01
    PRIVATE
        SN
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
)

set_target_properties(SN01 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
