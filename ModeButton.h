/*
  ==============================================================================

    ModeButton.h
    Created: 2 Jun 2025 3:50:30pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ModeButton : public juce::Component {
    public:
        ModeButton(const juce::String& paramID);
        ~ModeButton() override;
        juce::ToggleButton& getButton();
        void resized() override;
    private:
        juce::ToggleButton button;
};
