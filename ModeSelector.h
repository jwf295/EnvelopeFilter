/*
  ==============================================================================

    ModeSelector.h
    Created: 2 Jun 2025 3:50:42pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModeButton.h"

//==============================================================================
/**
*/
class ModeSelector : public juce::Component {
    public:
        ModeSelector(const juce::String& paramID, int radioGroupID);
        ~ModeSelector() override;
        void addMode(juce::AudioProcessorValueTreeState& vts, std::unique_ptr<ModeButton> mode);
        int getNumModes();
        void resized() override;
    private:
        std::vector<std::unique_ptr<ModeButton>> modes;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> modeAttachments;
        juce::String paramID;
        int radioGroupID;
        int numModes;
};
