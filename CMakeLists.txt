cmake_minimum_required(VERSION 3.15)

project(SignalNoise
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------
# VST3 SDK path
# ----------------------
set(JUCE_ONLY_VST3 TRUE CACHE BOOL "Build only VST3")
set(JUCE_VST3SDK_PATH "${CMAKE_SOURCE_DIR}/vst3sdk")  # path to your VST3 SDK

# ----------------------
# JUCE
# ----------------------
add_compile_definitions(JUCE_DISPLAY_SPLASH_SCREEN=0)
add_subdirectory(juce)

# Point JUCE to the VST2 SDK
#set(JUCE_VST2_PATH "${CMAKE_SOURCE_DIR}/VST_SDK_2.4")
juce_set_vst2_sdk_path("${CMAKE_SOURCE_DIR}/VST_SDK_2.4")
#set(JUCE_VST2SDK_PATH "" CACHE PATH "Disable VST2")

# ----------------------
# Global warning settings (safe)
# ----------------------
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# ----------------------
# Add plugins
# ----------------------
add_subdirectory(src/shared)
add_subdirectory(src/SN06)
add_subdirectory(src/SN04)
