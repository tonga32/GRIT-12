#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

namespace
{
    constexpr float referenceRate = 26040.0f;
    constexpr float minimumCutoff = 250.0f;
    constexpr float maximumCutoff = 20000.0f;

    float readParameter (const juce::AudioProcessorValueTreeState& state,
                         const char* parameterID) noexcept
    {
        if (auto* value = state.getRawParameterValue (parameterID))
            return value->load();
        return 0.0f;
    }
}

Grit12AudioProcessor::Grit12AudioProcessor()
    : juce::AudioProcessor (BusesProperties()
                                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "GRIT12_PARAMS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout Grit12AudioProcessor::createParameterLayout()
{
    using FloatParameter = juce::AudioParameterFloat;
    using BoolParameter = juce::AudioParameterBool;
    using ChoiceParameter = juce::AudioParameterChoice;
    using juce::ParameterID;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "detune", 1 }, "RPM / Detune",
        juce::NormalisableRange<float> { -30.0f, 0.0f, 0.1f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("st")));

    layout.add (std::make_unique<BoolParameter> (ParameterID { "range30", 1 }, "Range -30", false));
    layout.add (std::make_unique<BoolParameter> (ParameterID { "fine", 1 }, "Fine", false));

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "filter", 1 }, "S950 Filter",
        juce::NormalisableRange<float> { 0.0f, 99.0f, 1.0f }, 99.0f));

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "bits", 1 }, "Bit Depth",
        juce::NormalisableRange<float> { 6.0f, 16.0f, 1.0f }, 12.0f,
        juce::AudioParameterFloatAttributes().withLabel ("bit")));

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "input", 1 }, "Input Gain",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "output", 1 }, "Output Gain",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<BoolParameter> (ParameterID { "link", 1 }, "Gain Link", false));

    layout.add (std::make_unique<FloatParameter> (
        ParameterID { "mix", 1 }, "Wet / Dry",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 100.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    layout.add (std::make_unique<ChoiceParameter> (
        ParameterID { "routing", 1 }, "Routing",
        juce::StringArray { "STEREO", "MONO SUM", "MONO L", "MONO R", "MID", "SIDE" }, 0));

    return layout;
}

const juce::String Grit12AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

void Grit12AudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = juce::jmax (1.0, sampleRate);
    decimatorPhase[0] = decimatorPhase[1] = 1.0f;
    heldSample[0] = heldSample[1] = 0.0f;

    for (auto& channel : lowPass)
        for (auto& section : channel)
            section.reset();

    lastFilterAmount = -100.0f;
    updateFilter (readParameter (parameters, "filter"));
}

void Grit12AudioProcessor::releaseResources()
{
}

bool Grit12AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    if (input != output)
        return false;

    return input == juce::AudioChannelSet::mono()
        || input == juce::AudioChannelSet::stereo();
}

void Grit12AudioProcessor::Biquad::setLowPass (double sampleRate, double cutoffHz, double q)
{
    const auto safeSampleRate = juce::jmax (1.0, sampleRate);
    const auto safeCutoff = juce::jlimit (10.0, safeSampleRate * 0.45, cutoffHz);
    const auto omega = juce::MathConstants<double>::twoPi * safeCutoff / safeSampleRate;
    const auto cosine = std::cos (omega);
    const auto sine = std::sin (omega);
    const auto alpha = sine / (2.0 * juce::jmax (0.1, q));
    const auto a0 = 1.0 + alpha;

    b0 = ((1.0 - cosine) * 0.5) / a0;
    b1 = (1.0 - cosine) / a0;
    b2 = ((1.0 - cosine) * 0.5) / a0;
    a1 = (-2.0 * cosine) / a0;
    a2 = (1.0 - alpha) / a0;
}

void Grit12AudioProcessor::Biquad::reset() noexcept
{
    x1 = x2 = y1 = y2 = 0.0;
}

float Grit12AudioProcessor::Biquad::process (float input) noexcept
{
    const auto output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return static_cast<float> (output);
}

void Grit12AudioProcessor::updateFilter (float amount)
{
    lastFilterAmount = amount;

    if (amount >= 98.99f)
        return;

    const auto normalised = juce::jlimit (0.0f, 1.0f, amount / 99.0f);
    const auto cutoff = minimumCutoff * std::pow (maximumCutoff / minimumCutoff, normalised);

    // Three Butterworth second-order sections form a 6th-order low-pass.
    constexpr double qValues[3] { 0.5176380902, 0.7071067812, 1.9318516526 };

    for (auto& channel : lowPass)
        for (int section = 0; section < 3; ++section)
            channel[section].setLowPass (currentSampleRate, cutoff, qValues[section]);
}

float Grit12AudioProcessor::decimateAndQuantise (float input, int stream, float rateRatio,
                                                 int bitDepth, float inputGain) noexcept
{
    auto& phase = decimatorPhase[stream];
    auto& held = heldSample[stream];

    phase += rateRatio;
    if (phase >= 1.0f)
    {
        phase -= std::floor (phase);

        const auto driven = juce::jlimit (-1.0f, 1.0f, input * inputGain);
        const auto levels = static_cast<float> (std::pow (2.0, bitDepth - 1.0) - 1.0);
        held = std::round (driven * levels) / levels;
    }

    return held;
}

void Grit12AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto channelCount = juce::jmin (2, buffer.getNumChannels());
    if (channelCount == 0)
        return;

    const auto detune = readParameter (parameters, "detune");
    const auto range30 = readParameter (parameters, "range30") >= 0.5f;
    const auto maximumDetune = range30 ? -30.0f : -15.0f;
    const auto fine = readParameter (parameters, "fine") >= 0.5f;
    auto safeDetune = juce::jlimit (maximumDetune, 0.0f, detune);
    if (! fine)
        safeDetune = std::round (safeDetune);
    safeDetune = juce::jlimit (maximumDetune, 0.0f, safeDetune);
    const auto targetRate = referenceRate * std::pow (2.0f, safeDetune / 12.0f);
    const auto rateRatio = juce::jlimit (0.001f, 1.0f,
                                         targetRate / static_cast<float> (currentSampleRate));

    const auto bits = juce::roundToInt (readParameter (parameters, "bits"));
    const auto inputDb = readParameter (parameters, "input");
    const auto outputDb = readParameter (parameters, "output");
    const auto linked = readParameter (parameters, "link") >= 0.5f;
    const auto inputGain = juce::Decibels::decibelsToGain (inputDb);
    const auto compensatedOutputDb = linked ? outputDb - inputDb : outputDb;
    const auto outputGain = juce::Decibels::decibelsToGain (compensatedOutputDb);
    const auto mix = juce::jlimit (0.0f, 1.0f, readParameter (parameters, "mix") / 100.0f);

    const auto filterAmount = readParameter (parameters, "filter");
    if (std::abs (filterAmount - lastFilterAmount) > 0.001f)
        updateFilter (filterAmount);
    const auto filterEnabled = filterAmount < 98.99f;

    const auto routing = juce::roundToInt (readParameter (parameters, "routing"));

    auto* left = buffer.getWritePointer (0);
    auto* right = channelCount > 1 ? buffer.getWritePointer (1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto dryL = left[sample];
        const auto dryR = right != nullptr ? right[sample] : dryL;

        float wetL = dryL;
        float wetR = dryR;

        switch (routing)
        {
            case 1: // Mono sum
            {
                const auto mono = 0.5f * (dryL + dryR);
                const auto processed = decimateAndQuantise (mono, 0, rateRatio, bits, inputGain);
                wetL = wetR = processed;
                break;
            }
            case 2: // Mono left
            {
                const auto processed = decimateAndQuantise (dryL, 0, rateRatio, bits, inputGain);
                wetL = wetR = processed;
                break;
            }
            case 3: // Mono right
            {
                const auto processed = decimateAndQuantise (dryR, 0, rateRatio, bits, inputGain);
                wetL = wetR = processed;
                break;
            }
            case 4: // Process mid, preserve side
            {
                const auto mid = 0.5f * (dryL + dryR);
                const auto side = 0.5f * (dryL - dryR);
                const auto processedMid = decimateAndQuantise (mid, 0, rateRatio, bits, inputGain);
                wetL = processedMid + side;
                wetR = processedMid - side;
                break;
            }
            case 5: // Preserve mid, process side
            {
                const auto mid = 0.5f * (dryL + dryR);
                const auto side = 0.5f * (dryL - dryR);
                const auto processedSide = decimateAndQuantise (side, 0, rateRatio, bits, inputGain);
                wetL = mid + processedSide;
                wetR = mid - processedSide;
                break;
            }
            case 0:
            default: // Stereo
                wetL = decimateAndQuantise (dryL, 0, rateRatio, bits, inputGain);
                wetR = decimateAndQuantise (dryR, 1, rateRatio, bits, inputGain);
                break;
        }

        wetL *= outputGain;
        wetR *= outputGain;

        if (filterEnabled)
        {
            for (int section = 0; section < 3; ++section)
            {
                wetL = lowPass[0][section].process (wetL);
                wetR = lowPass[1][section].process (wetR);
            }
        }

        left[sample] = dryL + (wetL - dryL) * mix;
        if (right != nullptr)
            right[sample] = dryR + (wetR - dryR) * mix;
    }
}

juce::AudioProcessorEditor* Grit12AudioProcessor::createEditor()
{
    return new Grit12AudioProcessorEditor (*this);
}

void Grit12AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = parameters.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void Grit12AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Grit12AudioProcessor();
}
