/*
  ==============================================================================

    KnobGroup.h
    Created: 2 Jun 2025 3:50:05pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "KnobComponent.h"

//==============================================================================
/**
*/
class KnobGroup : public juce::Component {
    public:
        KnobGroup() = default;
        ~KnobGroup() override = default;
        void addKnob(const juce::String& paramID, std::unique_ptr<KnobComponent> knob);
        void setKnobVisibility(const juce::String& paramID, bool isVisible);
        void setNumVisibleKnobs(int numKnobs);
        int getNumVisibleKnobs();
        void resized() override;
    private:
        std::unordered_map<juce::String, std::unique_ptr<KnobComponent>> knobs;
        int numVisibleKnobs = 0;
};
