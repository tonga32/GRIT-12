#pragma once

#include <JuceHeader.h>

class Grit12AudioProcessor final : public juce::AudioProcessor
{
public:
    Grit12AudioProcessor();
    ~Grit12AudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct Biquad
    {
        void setLowPass (double sampleRate, double cutoffHz, double q);
        void reset() noexcept;
        float process (float input) noexcept;

        double b0 = 1.0, b1 = 0.0, b2 = 0.0;
        double a1 = 0.0, a2 = 0.0;
        double x1 = 0.0, x2 = 0.0;
        double y1 = 0.0, y2 = 0.0;
    };

    float decimateAndQuantise (float input, int stream, float rateRatio,
                               int bitDepth, float inputGain) noexcept;
    void updateFilter (float amount);

    double currentSampleRate = 44100.0;
    float decimatorPhase[2] { 1.0f, 1.0f };
    float heldSample[2] { 0.0f, 0.0f };
    Biquad lowPass[2][3];
    float lastFilterAmount = -100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Grit12AudioProcessor)
};
