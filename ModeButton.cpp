/*
  ==============================================================================

    ModeButton.cpp
    Created: 2 Jun 2025 3:51:11pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#include "ModeButton.h"

//==============================================================================

ModeButton::ModeButton(const juce::String& paramID) {
    button.setButtonText(paramID);
    addAndMakeVisible(button);
}

ModeButton::~ModeButton() {
    
}

juce::ToggleButton& ModeButton::getButton() {
    return button;
}

void ModeButton::resized() {
    int padding = 5;
    button.setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - 2 * padding);
}
