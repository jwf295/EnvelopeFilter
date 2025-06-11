/*
  ==============================================================================

    KnobComponent.h
    Created: 2 Jun 2025 3:49:05pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class KnobComponent : public juce::Component {
    public:
        KnobComponent(juce::AudioProcessorValueTreeState& vts, float min, float max, float interval, float defaultVal, const juce::String& paramID, const juce::String& paramName);
        ~KnobComponent() override;
        void resized() override;
    private:
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knobAttachment;
        juce::Slider knob;
        juce::Label label;
};
