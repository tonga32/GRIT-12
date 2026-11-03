cmake_minimum_required(VERSION 3.22)

project(GRIT12 VERSION 0.2.0 LANGUAGES C CXX)

include(FetchContent)

# JUCE is fetched at a fixed release so the project builds reproducibly.
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(GRIT12
    COMPANY_NAME "Tonga Audio"
    BUNDLE_ID "com.tongaaudio.grit12"
    PLUGIN_NAME "GRIT-12"
    PLUGIN_MANUFACTURER_CODE Tnga
    PLUGIN_CODE Grt1
    PRODUCT_NAME "GRIT-12"
    DESCRIPTION "Vintage-style sample-rate and bit-depth character processor"
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Distortion
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(GRIT12)

target_sources(GRIT12
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

target_compile_features(GRIT12 PRIVATE cxx_std_17)

target_compile_definitions(GRIT12
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(GRIT12
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
