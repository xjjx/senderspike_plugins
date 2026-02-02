juce_add_plugin(SN05
    COMPANY_NAME "SignalNoise"
    PLUGIN_MANUFACTURER_CODE SNFX
    PLUGIN_CODE SN5J

    FORMATS LV2 VST3 VST
    PRODUCT_NAME "SN05-J-Limiter"
    PLUGIN_NAME "SN05-J Limiter"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

	LV2URI "urn:signalsnoise:sn05"
)

#juce_add_binary_data(SN05_BinaryData
#    SOURCES
#        ../graphics/sn06g/sn06g_bk.png
#        ../graphics/sn06g/sn06g_b1.png
#        ../graphics/sn01g/sn01g_b1.png
#)

#target_link_libraries(SN05 PRIVATE SN05_BinaryData)

# ----------------------
# Add plugin source files
# ----------------------
target_sources(SN05
    PRIVATE
        SignalNoiseLimiter.cpp
#        SignalNoiseLimiterEditor.cpp
)

# ----------------------
# Include directories
# ----------------------
target_include_directories(SN05
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------
# Disable features we don't need
# ----------------------
target_compile_definitions(SN05
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ----------------------
# Link JUCE modules
# ----------------------
target_link_libraries(SN05
    PRIVATE
        SN
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
)

set_target_properties(SN05 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
