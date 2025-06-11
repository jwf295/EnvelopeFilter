/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Construct editor, initialize editor, processor, and sliders
EnvelopeFilterAudioProcessorEditor::EnvelopeFilterAudioProcessorEditor (EnvelopeFilterAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p)
{
    // Issue with modeSelector and ModeButton working properly due to value tree state issues
    /*
    modeSelector = std::make_unique<ModeSelector>("mode", 1);
    
    std::unique_ptr<ModeButton> envFilterMode = std::make_unique<ModeButton>("EF");
    std::unique_ptr<ModeButton> sampHoldMode = std::make_unique<ModeButton>("SH");
    
    modeSelector->addMode(audioProcessor.getParameters(), std::move(envFilterMode));
    modeSelector->addMode(audioProcessor.getParameters(), std::move(sampHoldMode));
    
    addAndMakeVisible(*modeSelector);
     */
    
    envFilterMode.setButtonText("EF");
    sampHoldMode.setButtonText("S&H");
    
    envFilterMode.setRadioGroupId(1);
    sampHoldMode.setRadioGroupId(1);
    
    envFilterMode.setClickingTogglesState(true);
    sampHoldMode.setClickingTogglesState(true);
    
    envFilterMode.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    sampHoldMode.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    
    envFilterMode.setToggleState(true, juce::dontSendNotification);
    
    addAndMakeVisible(envFilterMode);
    addAndMakeVisible(sampHoldMode);
    
    knobGroup = std::make_unique<KnobGroup>();
    
    std::unique_ptr<KnobComponent> filterKnob = std::make_unique<KnobComponent>(audioProcessor.getParameters(), 0.0f, 1.0f, 0.01f, 0.5f, "filterBlend", "Filter Blend");
    std::unique_ptr<KnobComponent> sensitivityKnob = std::make_unique<KnobComponent>(audioProcessor.getParameters(), 0.0f, 5.0f, 0.1f, 1.0f, "envelopeSensitivity", "Envelope Sensitivity");
    std::unique_ptr<KnobComponent> resonanceKnob = std::make_unique<KnobComponent>(audioProcessor.getParameters(), 0.0f, 10.0f, 0.1f, 1.0f, "resonance", "Resonance");
    std::unique_ptr<KnobComponent> attackKnob = std::make_unique<KnobComponent>(audioProcessor.getParameters(), 1.0f, 500.0f, 1.0f, 100.0f, "attackTimeMS", "Attack Time (MS)");
    std::unique_ptr<KnobComponent> speedKnob = std::make_unique<KnobComponent>(audioProcessor.getParameters(), 1.0f, 1000.0f, 1.0f, 100.0f, "speedMS", "Speed (MS)");
    
    knobGroup->addKnob("filterBlend", std::move(filterKnob));
    knobGroup->addKnob("envelopeSensitivity", std::move(sensitivityKnob));
    knobGroup->addKnob("resonance", std::move(resonanceKnob));
    knobGroup->addKnob("attackTimeMS", std::move(attackKnob));
    knobGroup->addKnob("speedMS", std::move(speedKnob));
    
    knobGroup->setKnobVisibility("filterBlend", true);
    knobGroup->setKnobVisibility("envelopeSensitivity", true);
    knobGroup->setKnobVisibility("resonance", true);
    knobGroup->setKnobVisibility("attackTimeMS", true);
    knobGroup->setKnobVisibility("speedMS", false);
    
    knobGroup->setNumVisibleKnobs(4);
    
    addAndMakeVisible(*knobGroup);
    
    audioProcessor.setActiveMode("EF");
    
    envFilterMode.onClick = [this]() {
        audioProcessor.setActiveMode("EF");
        updateGUI("EF");
        audioProcessor.updateModeImmediately();
    };
    
    sampHoldMode.onClick = [this]() {
        audioProcessor.setActiveMode("S&H");
        updateGUI("S&H");
        audioProcessor.updateModeImmediately();
    };
    
    // Set window size
    setSize(800, 300);
}

EnvelopeFilterAudioProcessorEditor::~EnvelopeFilterAudioProcessorEditor() {
    
}

void EnvelopeFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions (15.0f));
}

void EnvelopeFilterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    int editorHeight = getHeight();
    
    /*
    int modeSelectorX = 30;
    int modeSelectorWidth = 100;
    int modeSelectorHeight = modeSelector->getNumModes() * 30;
    int modeSelectorY = (editorHeight - modeSelectorHeight) / 2;
    
    // Position mode selector
    modeSelector->setBounds(modeSelectorX, modeSelectorY, modeSelectorWidth, modeSelectorHeight);
     */
    
    envFilterMode.setBounds(30, 110, 50, 30);
    sampHoldMode.setBounds(30, 160, 50, 30);
    
    /*
    int knobGroupX = modeSelectorX + modeSelectorWidth + 20;
    int knobGroupHeight = 200;
    int knobGroupWidth = knobGroup->getNumKnobs() * 100 + (knobGroup->getNumKnobs() - 1) * 50;
    int knobGroupY = (editorHeight - knobGroupHeight) / 2;
     */
    
    int knobGroupX = 0;
    if (knobGroup->getNumVisibleKnobs() == 4) {
        knobGroupX = 150;
    }
    else if (knobGroup->getNumVisibleKnobs() == 5) {
        knobGroupX = 120;
    }
    int knobGroupHeight = 200;
    int knobGroupWidth = 650;
    int knobGroupY = (editorHeight - knobGroupHeight) / 2;
    
    // Position knob group
    knobGroup->setBounds(knobGroupX, knobGroupY, knobGroupWidth, knobGroupHeight);
}

void EnvelopeFilterAudioProcessorEditor::updateGUI(const juce::String& mode) {
    if (mode == "EF") {
        knobGroup->setNumVisibleKnobs(4);
        knobGroup->setKnobVisibility("speedMS", false);
    }
    else if (mode == "S&H") {
        knobGroup->setNumVisibleKnobs(5);
        knobGroup->setKnobVisibility("speedMS", true);
    }
    resized();
}
