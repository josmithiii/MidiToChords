# MidiToChords - MIDI Chord Recognition Plugin
# A JUCE-based AU/Standalone plugin that displays chord names from MIDI input

cmake_minimum_required(VERSION 3.16)

project(MidiToChords
    VERSION 1.0.0
    DESCRIPTION "MIDI chord recognition plugin"
    LANGUAGES C CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# macOS deployment target
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
endif()

# Add JUCE
add_subdirectory(JUCE)

# Plugin target
set(TARGET_NAME MidiToChords)

juce_add_plugin(${TARGET_NAME}
    COMPANY_NAME "JOS"
    COMPANY_WEBSITE "https://ccrma.stanford.edu/~jos/"
    BUNDLE_ID "edu.stanford.ccrma.jos.MidiToChords"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS Standalone AU
    AU_MAIN_TYPE "kAudioUnitType_MIDIProcessor"
    PLUGIN_MANUFACTURER_CODE Joss
    PLUGIN_CODE Mtc1
    PLUGIN_NAME "${TARGET_NAME}"
    PRODUCT_NAME "${TARGET_NAME}"
)

juce_generate_juce_header(${TARGET_NAME})

# Source files
target_sources(${TARGET_NAME} PRIVATE
    Source/ChordRecognizer.cpp
    Source/ControlButtons.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Binary data (XML resource)
juce_add_binary_data(${TARGET_NAME}-BinaryData SOURCES
    MidiChords.xml
)

# Compile definitions
target_compile_definitions(${TARGET_NAME}
    PUBLIC
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
)

# Include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Link libraries
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ${TARGET_NAME}-BinaryData
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_cryptography
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

message(STATUS "MidiToChords: Build with 'cmake -B build && cmake --build build'")
message(STATUS "Output: build/${TARGET_NAME}_artefacts/")
