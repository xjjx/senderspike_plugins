juce_add_plugin(SN06
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN6J

    FORMATS VST3 VST
    PRODUCT_NAME "SN06-J"
    PLUGIN_NAME "SN06-J OpAmp"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

juce_add_binary_data(SN06_BinaryData
    SOURCES
        ../graphics/sn06g/sn06g_bk.png
        ../graphics/sn06g/sn06g_b1.png
        ../graphics/sn01g/sn01g_b1.png
)

target_link_libraries(SN06 PRIVATE SN06_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN06
    PRIVATE
        SN06Processor.cpp
        SN06Editor.cpp
)

# ----------------------
# Include directories
# ----------------------
target_include_directories(SN06
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN06
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ----------------------
# Link JUCE modules
# ----------------------
target_link_libraries(SN06
    PRIVATE
        SN
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
)

# Force the plugin to be named SN06.so instead of libSN06.so
set_target_properties(SN06 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
