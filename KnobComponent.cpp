/*
  ==============================================================================

    KnobComponent.cpp
    Created: 2 Jun 2025 3:51:39pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#include "KnobComponent.h"

//==============================================================================

KnobComponent::KnobComponent(juce::AudioProcessorValueTreeState& vts, float min, float max, float interval, float defaultVal, const juce::String& paramID, const juce::String& paramName) {
    // Attach slider
    if (!knobAttachment) {
        knobAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, paramID, knob));
    }
    // Set up knob
    if (paramID.contains("MS")) {
        knob.setRange(static_cast<int>(min), static_cast<int>(max), static_cast<int>(interval));
        knob.setValue(static_cast<int>(defaultVal));
    }
    else {
        knob.setRange(min, max, interval);
        knob.setValue(defaultVal);
    }
    knob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    
    // Set up label
    label.setText(paramName, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    
    // Add and make subcomponents visible
    addAndMakeVisible(knob);
    addAndMakeVisible(label);
}

KnobComponent::~KnobComponent() {
    // Memory management for sliderAttachment
    knobAttachment.reset();
}

void KnobComponent::resized() {
    // Position subcomponents
    int labelHeight = 30;
    int knobHeight = 100;
    label.setBounds(0, 0, 100, labelHeight);
    knob.setBounds(0, labelHeight, 100, knobHeight);
}
