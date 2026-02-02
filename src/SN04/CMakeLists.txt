juce_add_plugin(SN04
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN6J

    FORMATS LV2 VST3 VST
    PRODUCT_NAME "SN04-J-Channel-EQ"
    PLUGIN_NAME "SN04-J Channel EQ"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

	LV2URI "urn:signalsnoise:sn04"
)

#juce_add_binary_data(SN04_BinaryData
#    SOURCES
#        ../graphics/sn06g/sn06g_bk.png
#        ../graphics/sn06g/sn06g_b1.png
#        ../graphics/sn01g/sn01g_b1.png
#)

#target_link_libraries(SN04 PRIVATE SN04_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN04
    PRIVATE
        SignalNoiseEqualizer.cpp
#        SignalNoiseOpampEditor.cpp
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
