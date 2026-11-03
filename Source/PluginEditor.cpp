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

    void styleReadout (juce::Label& label)
    {
        label.setJustificationType (juce::Justification::centred);
        label.setFont (makeMonoFont (12.0f, true));
        label.setColour (juce::Label::textColourId, GritLookAndFeel::cream());
    }
}

GritLookAndFeel::GritLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, cream());
    setColour (juce::ComboBox::textColourId, cream());
    setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::TextButton::textColourOffId, cream());
    setColour (juce::TextButton::textColourOnId, juce::Colour (0xff172023));
}

juce::Colour GritLookAndFeel::background() { return juce::Colour (0xff15191b); }
juce::Colour GritLookAndFeel::panel()      { return juce::Colour (0xff414749); }
juce::Colour GritLookAndFeel::raised()     { return juce::Colour (0xff262c2f); }
juce::Colour GritLookAndFeel::accent()     { return juce::Colour (0xff66a8b3); }
juce::Colour GritLookAndFeel::cream()      { return juce::Colour (0xffe0ded3); }
juce::Colour GritLookAndFeel::muted()      { return juce::Colour (0xffa9ada6); }

void GritLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearVertical)
    {
        juce::LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos,
                                                minSliderPos, maxSliderPos, style, slider);
        return;
    }

    juce::ignoreUnused (slider);

    const auto centreX = static_cast<float> (x) + static_cast<float> (width) * 0.5f;
    const auto top = static_cast<float> (y) + 10.0f;
    const auto bottom = static_cast<float> (y + height) - 10.0f;
    const auto thumbY = juce::jlimit (top, bottom, sliderPos);

    // Recessed fader slot, inspired by the SP-1200 performance section.
    g.setColour (juce::Colour (0xff1d2224));
    g.fillRoundedRectangle (centreX - 4.5f, top, 9.0f, bottom - top, 2.0f);
    g.setColour (juce::Colour (0xff697174));
    g.drawRoundedRectangle (centreX - 4.5f, top, 9.0f, bottom - top, 2.0f, 1.0f);

    // Printed scale marks on both sides of the slot.
    for (int tick = 0; tick <= 10; ++tick)
    {
        const auto proportion = static_cast<float> (tick) / 10.0f;
        const auto tickY = juce::jmap (proportion, top, bottom);
        const auto tickWidth = (tick % 5 == 0) ? 10.0f : 5.0f;
        g.setColour (tick % 5 == 0 ? juce::Colour (0xffc3c4b8)
                                   : juce::Colour (0xff898f8d));
        g.drawLine (centreX - 12.0f - tickWidth, tickY,
                    centreX - 12.0f, tickY, 1.0f);
        g.drawLine (centreX + 12.0f, tickY,
                    centreX + 12.0f + tickWidth, tickY, 1.0f);
    }

    // Metal-edged, dark plastic fader cap.
    const juce::Rectangle<float> cap (centreX - 18.0f, thumbY - 8.0f, 36.0f, 16.0f);
    g.setColour (juce::Colour (0xff171b1d));
    g.fillRect (cap.translated (0.0f, 2.0f));
    g.setColour (juce::Colour (0xff30383a));
    g.fillRect (cap);
    g.setColour (juce::Colour (0xffaeb2aa));
    g.drawRect (cap, 1.0f);
    g.setColour (GritLookAndFeel::accent());
    g.drawLine (centreX - 12.0f, thumbY, centreX + 12.0f, thumbY, 1.5f);
    g.setColour (juce::Colour (0xffe0ded3));
    g.drawLine (centreX - 1.0f, thumbY - 4.0f, centreX + 1.0f, thumbY - 4.0f, 1.0f);
}

void GritLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                             const juce::Colour&, bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const auto active = button.getToggleState();
    auto fill = active ? accent() : juce::Colour (0xff303638);

    if (down)
        fill = fill.darker (0.20f);
    else if (highlighted)
        fill = fill.brighter (0.08f);

    g.setColour (juce::Colour (0xff1c2123));
    g.fillRect (bounds.translated (0.0f, 2.0f));
    g.setColour (fill);
    g.fillRect (bounds);
    g.setColour (active ? juce::Colour (0xffb7d4d3) : juce::Colour (0xff858c89));
    g.drawRect (bounds, 1.0f);

    if (active)
    {
        g.setColour (juce::Colour (0xffe6f0d8));
        g.fillEllipse (bounds.getRight() - 9.0f, bounds.getY() + 4.0f, 4.0f, 4.0f);
    }
}

void GritLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                     int, int, int, int, juce::ComboBox&)
{
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width),
                                                 static_cast<float> (height)).reduced (1.0f);
    g.setColour (juce::Colour (0xff252b2d));
    g.fillRect (bounds);
    g.setColour (isButtonDown ? accent() : juce::Colour (0xff858c89));
    g.drawRect (bounds, 1.0f);

    juce::Path arrow;
    const auto cx = static_cast<float> (width) - 15.0f;
    const auto cy = static_cast<float> (height) * 0.5f;
    arrow.addTriangle (cx - 4.0f, cy - 2.0f, cx + 4.0f, cy - 2.0f, cx, cy + 3.0f);
    g.setColour (accent());
    g.fillPath (arrow);
}

Grit12AudioProcessorEditor::Grit12AudioProcessorEditor (Grit12AudioProcessor& processor)
    : AudioProcessorEditor (&processor), audioProcessor (processor)
{
    setLookAndFeel (&gritLookAndFeel);
    setSize (980, 590);

    detuneLabel.setText ("RPM / DETUNE", juce::dontSendNotification);
    configureFader (detuneSlider, detuneLabel, "RPM / DETUNE");
    configureFader (filterSlider, filterLabel, "S950 FILTER");
    configureFader (bitsSlider, bitsLabel, "BIT DEPTH");
    configureFader (inputSlider, inputLabel, "INPUT GAIN");
    configureFader (outputSlider, outputLabel, "OUTPUT GAIN");
    configureFader (mixSlider, mixLabel, "WET / DRY");

    detuneReadout.setFont (makeMonoFont (13.0f, true));
    detuneReadout.setJustificationType (juce::Justification::centred);
    detuneReadout.setColour (juce::Label::textColourId, juce::Colour (0xff17302c));
    addAndMakeVisible (detuneReadout);

    for (auto* readout : { &filterReadout, &bitsReadout, &inputReadout,
                           &outputReadout, &mixReadout })
    {
        styleReadout (*readout);
        addAndMakeVisible (*readout);
    }

    rangeButton.setClickingTogglesState (true);
    fineButton.setClickingTogglesState (true);
    linkButton.setClickingTogglesState (true);
    addAndMakeVisible (rangeButton);
    addAndMakeVisible (fineButton);
    addAndMakeVisible (linkButton);

    routingLabel.setText ("AUDIO ROUTING", juce::dontSendNotification);
    routingLabel.setFont (makeMonoFont (10.0f, true));
    routingLabel.setColour (juce::Label::textColourId, GritLookAndFeel::cream());
    addAndMakeVisible (routingLabel);

    routingBox.addItemList ({ "STEREO", "MONO SUM", "MONO L", "MONO R", "MID", "SIDE" }, 1);
    routingBox.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (routingBox);

    footerLabel.setText ("GRIT-12  /  12-BIT SAMPLER CHARACTER  /  SIX-POLE LPF  /  V0.2",
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
    filterSlider.onValueChange = [this] { updateValueReadouts(); };
    bitsSlider.onValueChange = [this] { updateValueReadouts(); };
    inputSlider.onValueChange = [this] { updateValueReadouts(); };
    outputSlider.onValueChange = [this] { updateValueReadouts(); };
    mixSlider.onValueChange = [this] { updateValueReadouts(); };

    updateDetuneRange();
    updateDetuneReadout();
    updateValueReadouts();
}

Grit12AudioProcessorEditor::~Grit12AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void Grit12AudioProcessorEditor::configureFader (juce::Slider& slider, juce::Label& label,
                                                  const juce::String& labelText)
{
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setMouseDragSensitivity (150);
    slider.setColour (juce::Slider::trackColourId, GritLookAndFeel::accent());
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
                           + juce::String (semitones, 1) + " ST",
                           juce::dontSendNotification);
}

void Grit12AudioProcessorEditor::updateValueReadouts()
{
    const auto filter = filterSlider.getValue();
    filterReadout.setText (filter >= 98.99 ? "OFF" : juce::String (juce::roundToInt (filter)),
                           juce::dontSendNotification);
    bitsReadout.setText (juce::String (juce::roundToInt (bitsSlider.getValue())) + " BIT",
                         juce::dontSendNotification);
    inputReadout.setText (juce::String (inputSlider.getValue(), 1) + " dB",
                          juce::dontSendNotification);
    outputReadout.setText (juce::String (outputSlider.getValue(), 1) + " dB",
                           juce::dontSendNotification);
    mixReadout.setText (juce::String (juce::roundToInt (mixSlider.getValue())) + " %",
                        juce::dontSendNotification);
}

void Grit12AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (GritLookAndFeel::background());

    const auto outer = getLocalBounds().toFloat().reduced (10.0f);
    g.setColour (juce::Colour (0xff0b0e0f));
    g.fillRect (outer.expanded (1.0f));
    g.setColour (juce::Colour (0xff747a78));
    g.drawRect (outer, 1.0f);

    auto face = outer.reduced (7.0f);
    g.setColour (GritLookAndFeel::panel());
    g.fillRect (face);
    g.setColour (juce::Colour (0xff242a2c));
    g.drawRect (face, 2.0f);

    // Very subtle printed-metal grain, kept low contrast so controls stay legible.
    g.setColour (juce::Colour (0x120d1112));
    for (int y = 20; y < getHeight() - 20; y += 4)
        g.drawHorizontalLine (y, 18.0f, static_cast<float> (getWidth() - 18));

    // Main panel divisions and the blue-green silkscreen rules of the hardware reference.
    g.setColour (GritLookAndFeel::accent());
    g.drawHorizontalLine (77, 32.0f, 948.0f);
    g.drawHorizontalLine (184, 32.0f, 948.0f);
    g.drawHorizontalLine (477, 32.0f, 948.0f);
    g.drawHorizontalLine (106, 48.0f, 226.0f);
    g.drawHorizontalLine (106, 260.0f, 415.0f);
    g.drawHorizontalLine (106, 448.0f, 646.0f);
    g.drawHorizontalLine (106, 690.0f, 932.0f);

    // Chassis fasteners.
    for (const auto point : { juce::Point<float> (27.0f, 27.0f), juce::Point<float> (953.0f, 27.0f),
                              juce::Point<float> (27.0f, 563.0f), juce::Point<float> (953.0f, 563.0f) })
    {
        g.setColour (juce::Colour (0xff1c2224));
        g.fillEllipse (point.x - 5.0f, point.y - 5.0f, 10.0f, 10.0f);
        g.setColour (juce::Colour (0xff90958d));
        g.drawEllipse (point.x - 5.0f, point.y - 5.0f, 10.0f, 10.0f, 1.0f);
        g.drawLine (point.x - 2.6f, point.y + 2.6f, point.x + 2.6f, point.y - 2.6f, 1.0f);
    }

    // Branding: compact instrument legend rather than a modern software header.
    g.setColour (GritLookAndFeel::cream());
    g.setFont (makeLabelFont (25.0f, true));
    g.drawText ("GRIT-12", 48, 24, 190, 31, juce::Justification::centredLeft);
    g.setColour (GritLookAndFeel::muted());
    g.setFont (makeMonoFont (9.0f));
    g.drawText ("TONGA AUDIO  /  SAMPLING PROCESSOR", 50, 55, 300, 13,
                juce::Justification::centredLeft);

    // Functional section titles printed on the panel.
    g.setColour (GritLookAndFeel::cream());
    g.setFont (makeMonoFont (10.0f, true));
    g.drawText ("SET-UP", 50, 88, 174, 14, juce::Justification::centredLeft);
    g.drawText ("GAIN", 262, 88, 150, 14, juce::Justification::centredLeft);
    g.drawText ("ROUTING", 450, 88, 185, 14, juce::Justification::centredLeft);
    g.drawText ("MASTER CONTROL", 694, 88, 238, 14, juce::Justification::centredLeft);

    // LCD bezel and active indicator.
    const juce::Rectangle<float> lcd (694.0f, 112.0f, 238.0f, 49.0f);
    g.setColour (juce::Colour (0xff191e1f));
    g.fillRect (lcd.expanded (4.0f));
    g.setColour (juce::Colour (0xffa3aaa1));
    g.drawRect (lcd.expanded (4.0f), 1.0f);
    g.setColour (juce::Colour (0xff9ab5a0));
    g.fillRect (lcd);
    g.setColour (juce::Colour (0xff28433b));
    g.drawRect (lcd, 1.0f);
    g.setColour (juce::Colour (0xff244137));
    g.setFont (makeMonoFont (8.0f, true));
    g.drawText ("GRIT-12  /  DIGITAL SAMPLER", 701, 99, 220, 12,
                juce::Justification::centredRight);

    g.setColour (GritLookAndFeel::muted());
    g.setFont (makeMonoFont (9.0f));
    g.drawText ("PERFORMANCE", 49, 196, 200, 14, juce::Justification::centredLeft);
    g.drawText ("26.04 kHz REFERENCE  /  12-BIT MODE", 690, 489, 240, 14,
                juce::Justification::centredRight);

    // Small red power/status lamp, a nod to the original hardware's indicator lamps.
    g.setColour (juce::Colour (0xffc55e4d));
    g.fillEllipse (918.0f, 32.0f, 7.0f, 7.0f);
    g.setColour (juce::Colour (0xffe59a84));
    g.drawEllipse (918.0f, 32.0f, 7.0f, 7.0f, 0.8f);
}

void Grit12AudioProcessorEditor::resized()
{
    // Upper function area: compact switches, routing selector and a small LCD.
    rangeButton.setBounds (50, 119, 96, 29);
    fineButton.setBounds (153, 119, 66, 29);
    linkButton.setBounds (262, 119, 116, 29);

    routingLabel.setBounds (450, 112, 185, 14);
    routingBox.setBounds (450, 130, 190, 29);
    detuneReadout.setBounds (701, 124, 224, 24);

    // Six vertical performance faders: the actual six parameters exposed by GRIT-12.
    const int centres[] { 125, 271, 417, 563, 709, 855 };
    juce::Slider* sliders[] { &detuneSlider, &filterSlider, &bitsSlider,
                              &inputSlider, &outputSlider, &mixSlider };
    juce::Label* labels[] { &detuneLabel, &filterLabel, &bitsLabel,
                            &inputLabel, &outputLabel, &mixLabel };
    juce::Label* readouts[] { nullptr, &filterReadout, &bitsReadout,
                              &inputReadout, &outputReadout, &mixReadout };

    for (int i = 0; i < 6; ++i)
    {
        labels[i]->setBounds (centres[i] - 65, 222, 130, 20);
        sliders[i]->setBounds (centres[i] - 32, 248, 64, 200);
        if (readouts[i] != nullptr)
            readouts[i]->setBounds (centres[i] - 58, 451, 116, 20);
    }

    footerLabel.setBounds (49, 495, 590, 17);
}
