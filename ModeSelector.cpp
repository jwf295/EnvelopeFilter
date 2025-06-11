/*
  ==============================================================================

    ModeSelector.cpp
    Created: 2 Jun 2025 3:50:57pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#include "ModeSelector.h"

ModeSelector::ModeSelector(const juce::String& paramID, int radioGroupID)
    : paramID(paramID), radioGroupID(radioGroupID), numModes(0) {
        
}

ModeSelector::~ModeSelector() {
    modes.clear();
    modes.shrink_to_fit();
    for (int mode = 0; mode < modes.size(); ++mode) {
        modeAttachments[mode].reset();
    }
    modeAttachments.clear();
    modeAttachments.shrink_to_fit();
}

void ModeSelector::addMode(juce::AudioProcessorValueTreeState& vts, std::unique_ptr<ModeButton> mode) {
    if (mode != nullptr) {
        mode->getButton().setRadioGroupId(radioGroupID);
        addAndMakeVisible(*mode);
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(vts, paramID, mode->getButton());
        numModes++;
        modes.push_back(std::move(mode));
        modeAttachments.push_back(std::move(modeAttachment));
    }
}

int ModeSelector::getNumModes() {
    return numModes;
}

void ModeSelector::resized() {
    int buttonHeight = 30;
    int y = 0;
    for (int mode = 0; mode < modes.size(); ++mode) {
        modes[mode]->setBounds(0, y, getWidth(), buttonHeight);
        y += buttonHeight;
    }
}
