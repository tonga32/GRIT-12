#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class GritLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    GritLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle, juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    static juce::Colour background();
    static juce::Colour panel();
    static juce::Colour raised();
    static juce::Colour amber();
    static juce::Colour cream();
    static juce::Colour muted();
};

class Grit12AudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit Grit12AudioProcessorEditor (Grit12AudioProcessor&);
    ~Grit12AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void configureKnob (juce::Slider&, juce::Label&, const juce::String& labelText);
    void updateDetuneRange();
    void updateDetuneReadout();

    Grit12AudioProcessor& audioProcessor;
    GritLookAndFeel gritLookAndFeel;

    juce::Slider detuneSlider;
    juce::Slider filterSlider;
    juce::Slider bitsSlider;
    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider mixSlider;

    juce::Label detuneLabel;
    juce::Label detuneReadout;
    juce::Label filterLabel;
    juce::Label bitsLabel;
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label mixLabel;
    juce::Label routingLabel;
    juce::Label footerLabel;

    juce::ComboBox routingBox;
    juce::TextButton rangeButton { "RANGE -15" };
    juce::TextButton fineButton { "FINE" };
    juce::TextButton linkButton { "GAIN LINK" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> routingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> rangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fineAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linkAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Grit12AudioProcessorEditor)
};
