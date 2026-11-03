#include "PluginEditor.h"

#include <cmath>

namespace
{
    juce::Font makeLabelFont (float height, bool bold = false)
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), height,
                           bold ? juce::Font::bold : juce::Font::plain);
    }

    juce::Font makeMonoFont (float height, bool bold = false)
    {
        return juce::Font (juce::Font::getDefaultMonospacedFontName(), height,
                           bold ? juce::Font::bold : juce::Font::plain);
    }
}

GritLookAndFeel::GritLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, cream());
    setColour (juce::ComboBox::textColourId, cream());
    setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::TextButton::textColourOffId, cream());
    setColour (juce::TextButton::textColourOnId, juce::Colour (0xff242526));
}

juce::Colour GritLookAndFeel::background() { return juce::Colour (0xff252728); }
juce::Colour GritLookAndFeel::panel()      { return juce::Colour (0xff303335); }
juce::Colour GritLookAndFeel::raised()     { return juce::Colour (0xff3a3d3f); }
juce::Colour GritLookAndFeel::amber()      { return juce::Colour (0xffc18a4a); }
juce::Colour GritLookAndFeel::cream()      { return juce::Colour (0xffd9d0be); }
juce::Colour GritLookAndFeel::muted()      { return juce::Colour (0xff858178); }

void GritLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider&)
{
    const auto size = static_cast<float> (juce::jmin (width, height) - 16);
    const auto radius = size * 0.5f;
    const auto centreX = static_cast<float> (x) + static_cast<float> (width) * 0.5f;
    const auto centreY = static_cast<float> (y) + static_cast<float> (height) * 0.5f;
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    const juce::Rectangle<float> body (centreX - radius, centreY - radius, size, size);
    g.setColour (juce::Colour (0xff1d1f20));
    g.fillEllipse (body.expanded (2.0f));
    g.setColour (juce::Colour (0xff66645e));
    g.drawEllipse (body.expanded (1.0f), 1.2f);
    g.setColour (raised());
    g.fillEllipse (body.reduced (3.0f));
    g.setColour (juce::Colour (0xff4b4e4f));
    g.drawEllipse (body.reduced (3.0f), 1.0f);

    juce::Path arc;
    arc.addCentredArc (centreX, centreY, radius - 5.0f, radius - 5.0f,
                       0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (juce::Colour (0xff555655));
    g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc (centreX, centreY, radius - 5.0f, radius - 5.0f,
                            0.0f, rotaryStartAngle, angle, true);
    g.setColour (amber());
    g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    const auto pointerLength = radius * 0.61f;
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.6f, -pointerLength, 3.2f, pointerLength * 0.43f, 1.2f);
    g.setColour (cream());
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centreX, centreY));

    g.setColour (juce::Colour (0xff1b1d1e));
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
}

void GritLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearHorizontal)
    {
        juce::LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos,
                                                minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const auto centreY = static_cast<float> (y) + static_cast<float> (height) * 0.5f;
    const auto left = static_cast<float> (x) + 7.0f;
    const auto right = static_cast<float> (x + width) - 7.0f;
    const auto thumbX = juce::jlimit (left, right, sliderPos);

    g.setColour (juce::Colour (0xff1c1e1f));
    g.fillRoundedRectangle (left, centreY - 5.0f, right - left, 10.0f, 5.0f);
    g.setColour (juce::Colour (0xff66645e));
    g.drawRoundedRectangle (left, centreY - 5.0f, right - left, 10.0f, 5.0f, 1.0f);
    g.setColour (amber());
    g.fillRoundedRectangle (left, centreY - 2.0f, juce::jmax (0.0f, thumbX - left), 4.0f, 2.0f);

    g.setColour (juce::Colour (0xff17191a));
    g.fillEllipse (thumbX - 11.0f, centreY - 11.0f, 22.0f, 22.0f);
    g.setColour (cream());
    g.drawEllipse (thumbX - 11.0f, centreY - 11.0f, 22.0f, 22.0f, 1.2f);
    g.setColour (amber());
    g.fillEllipse (thumbX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
}

void GritLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                             const juce::Colour&, bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const auto active = button.getToggleState();
    auto fill = active ? amber() : raised();
    if (down)
        fill = fill.darker (0.18f);
    else if (highlighted)
        fill = fill.brighter (0.10f);

    g.setColour (juce::Colour (0xff1d1f20));
    g.fillRoundedRectangle (bounds.translated (0.0f, 2.0f), 4.0f);
    g.setColour (fill);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (active ? juce::Colour (0xffe0ad6b) : juce::Colour (0xff55585a));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}

void GritLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                     int, int, int, int, juce::ComboBox&)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width),
                                          static_cast<float> (height)).reduced (1.0f);
    g.setColour (juce::Colour (0xff1e2021));
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (isButtonDown ? amber() : juce::Colour (0xff5b5d5d));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    juce::Path arrow;
    const auto cx = static_cast<float> (width) - 17.0f;
    const auto cy = static_cast<float> (height) * 0.5f;
    arrow.addTriangle (cx - 4.0f, cy - 2.0f, cx + 4.0f, cy - 2.0f, cx, cy + 3.0f);
    g.setColour (amber());
    g.fillPath (arrow);
}

Grit12AudioProcessorEditor::Grit12AudioProcessorEditor (Grit12AudioProcessor& processor)
    : AudioProcessorEditor (&processor), audioProcessor (processor)
{
    setLookAndFeel (&gritLookAndFeel);
    setSize (960, 560);

    detuneLabel.setText ("RPM / DETUNE", juce::dontSendNotification);
    detuneLabel.setFont (makeLabelFont (13.0f, true));
    detuneLabel.setColour (juce::Label::textColourId, GritLookAndFeel::cream());
    addAndMakeVisible (detuneLabel);

    detuneReadout.setJustificationType (juce::Justification::centredRight);
    detuneReadout.setFont (makeMonoFont (13.0f, true));
    detuneReadout.setColour (juce::Label::textColourId, GritLookAndFeel::amber());
    addAndMakeVisible (detuneReadout);

    detuneSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    detuneSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    detuneSlider.setMouseDragSensitivity (180);
    addAndMakeVisible (detuneSlider);

    configureKnob (filterSlider, filterLabel, "S950 FILTER");
    configureKnob (bitsSlider, bitsLabel, "BIT DEPTH");
    configureKnob (inputSlider, inputLabel, "INPUT dB");
    configureKnob (outputSlider, outputLabel, "OUTPUT dB");
    configureKnob (mixSlider, mixLabel, "WET / DRY");

    rangeButton.setClickingTogglesState (true);
    fineButton.setClickingTogglesState (true);
    linkButton.setClickingTogglesState (true);
    addAndMakeVisible (rangeButton);
    addAndMakeVisible (fineButton);
    addAndMakeVisible (linkButton);

    routingLabel.setText ("ROUTING", juce::dontSendNotification);
    routingLabel.setFont (makeLabelFont (11.0f, true));
    routingLabel.setColour (juce::Label::textColourId, GritLookAndFeel::muted());
    addAndMakeVisible (routingLabel);

    routingBox.addItemList ({ "STEREO", "MONO SUM", "MONO L", "MONO R", "MID", "SIDE" }, 1);
    routingBox.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (routingBox);

    footerLabel.setText ("GRIT-12  //  DIGITAL CHARACTER PROCESSOR  //  V0.1.0",
                         juce::dontSendNotification);
    footerLabel.setFont (makeMonoFont (10.0f));
    footerLabel.setColour (juce::Label::textColourId, GritLookAndFeel::muted());
    addAndMakeVisible (footerLabel);

    auto& state = audioProcessor.parameters;
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "detune", detuneSlider);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "filter", filterSlider);
    bitsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "bits", bitsSlider);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "input", inputSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "output", outputSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "mix", mixSlider);
    routingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (state, "routing", routingBox);
    rangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, "range30", rangeButton);
    fineAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, "fine", fineButton);
    linkAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, "link", linkButton);

    rangeButton.onClick = [this] { updateDetuneRange(); };
    fineButton.onClick = [this] { updateDetuneRange(); };
    detuneSlider.onValueChange = [this] { updateDetuneReadout(); };

    updateDetuneRange();
    updateDetuneReadout();
}

Grit12AudioProcessorEditor::~Grit12AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void Grit12AudioProcessorEditor::configureKnob (juce::Slider& slider, juce::Label& label,
                                                 const juce::String& labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setFont (makeLabelFont (11.0f, true));
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, GritLookAndFeel::cream());
    addAndMakeVisible (label);
}

void Grit12AudioProcessorEditor::updateDetuneRange()
{
    const auto wide = rangeButton.getToggleState();
    const auto fine = fineButton.getToggleState();
    detuneSlider.setRange (wide ? -30.0 : -15.0, 0.0, fine ? 0.1 : 1.0);

    if (! wide && detuneSlider.getValue() < -15.0)
        detuneSlider.setValue (-15.0, juce::sendNotificationSync);
    if (! fine)
        detuneSlider.setValue (std::round (detuneSlider.getValue()), juce::sendNotificationSync);

    rangeButton.setButtonText (wide ? "RANGE -30" : "RANGE -15");
    updateDetuneReadout();
}

void Grit12AudioProcessorEditor::updateDetuneReadout()
{
    const auto semitones = static_cast<float> (detuneSlider.getValue());
    const auto virtualRpm = 33.0f * std::pow (2.0f, -semitones / 12.0f);
    detuneReadout.setText (juce::String (virtualRpm, 1) + " RPM   /   "
                           + juce::String (semitones, 1) + " st",
                           juce::dontSendNotification);
}

void Grit12AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (GritLookAndFeel::background());

    auto outer = getLocalBounds().toFloat().reduced (12.0f);
    g.setColour (juce::Colour (0xff17191a));
    g.fillRoundedRectangle (outer, 9.0f);
    g.setColour (juce::Colour (0xff5a5b57));
    g.drawRoundedRectangle (outer, 9.0f, 1.0f);

    auto inner = outer.reduced (8.0f);
    g.setColour (GritLookAndFeel::panel());
    g.fillRoundedRectangle (inner, 6.0f);

    // Subtle panel lines and warm, screen-printed hardware labels.
    g.setColour (juce::Colour (0xff47494a));
    g.drawHorizontalLine (91, 28.0f, 932.0f);
    g.drawHorizontalLine (224, 28.0f, 932.0f);
    g.drawHorizontalLine (431, 28.0f, 932.0f);
    g.drawVerticalLine (206, 246.0f, 414.0f);
    g.drawVerticalLine (378, 246.0f, 414.0f);
    g.drawVerticalLine (550, 246.0f, 414.0f);
    g.drawVerticalLine (722, 246.0f, 414.0f);

    // Four recessed enclosure screws.
    for (const auto point : { juce::Point<float> (35.0f, 35.0f), juce::Point<float> (925.0f, 35.0f),
                              juce::Point<float> (35.0f, 525.0f), juce::Point<float> (925.0f, 525.0f) })
    {
        g.setColour (juce::Colour (0xff202223));
        g.fillEllipse (point.x - 5.0f, point.y - 5.0f, 10.0f, 10.0f);
        g.setColour (juce::Colour (0xff77766f));
        g.drawLine (point.x - 2.5f, point.y + 2.5f, point.x + 2.5f, point.y - 2.5f, 1.0f);
    }

    g.setColour (GritLookAndFeel::cream());
    g.setFont (makeLabelFont (29.0f, true));
    g.drawText ("GRIT-12", 48, 27, 220, 37, juce::Justification::centredLeft);

    g.setColour (GritLookAndFeel::muted());
    g.setFont (makeMonoFont (10.0f));
    g.drawText ("VINTAGE DIGITAL PROCESSOR", 50, 64, 260, 15, juce::Justification::centredLeft);

    g.setColour (GritLookAndFeel::amber());
    g.fillEllipse (820.0f, 42.0f, 8.0f, 8.0f);
    g.setColour (GritLookAndFeel::cream());
    g.setFont (makeMonoFont (12.0f, true));
    g.drawText ("12 BIT  /  26.04 kHz", 838, 34, 90, 20, juce::Justification::centredRight);
    g.setColour (GritLookAndFeel::muted());
    g.setFont (makeMonoFont (9.0f));
    g.drawText ("SAMPLER CHARACTER + 6-POLE LPF", 650, 57, 278, 15,
                juce::Justification::centredRight);

    g.setColour (GritLookAndFeel::muted());
    g.setFont (makeMonoFont (9.0f));
    g.drawText ("DECIMATION / PITCH COMPENSATION", 48, 205, 300, 12,
                juce::Justification::centredLeft);
    g.drawText ("SIGNAL SHAPING", 48, 237, 160, 12, juce::Justification::centredLeft);
    g.drawText ("OUTPUT SECTION", 744, 237, 180, 12, juce::Justification::centredLeft);
}

void Grit12AudioProcessorEditor::resized()
{
    detuneLabel.setBounds (48, 108, 220, 22);
    detuneReadout.setBounds (620, 108, 306, 22);
    detuneSlider.setBounds (48, 143, 878, 42);

    rangeButton.setBounds (48, 185, 115, 25);
    fineButton.setBounds (171, 185, 75, 25);

    const int knobY = 265;
    const int knobSize = 112;
    const int labelY = 388;
    const int labelWidth = 150;
    const int centres[] { 120, 292, 464, 636, 808 };
    juce::Slider* sliders[] { &filterSlider, &bitsSlider, &inputSlider, &outputSlider, &mixSlider };
    juce::Label* labels[] { &filterLabel, &bitsLabel, &inputLabel, &outputLabel, &mixLabel };

    for (int i = 0; i < 5; ++i)
    {
        sliders[i]->setBounds (centres[i] - knobSize / 2, knobY, knobSize, knobSize);
        labels[i]->setBounds (centres[i] - labelWidth / 2, labelY, labelWidth, 20);
    }

    routingLabel.setBounds (48, 458, 100, 18);
    routingBox.setBounds (48, 480, 220, 30);
    linkButton.setBounds (290, 480, 120, 30);
    footerLabel.setBounds (48, 521, 600, 15);
}
