/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KnobGroup.h"
#include "ModeSelector.h"

//==============================================================================
/**
*/
class EnvelopeFilterAudioProcessorEditor : public juce::AudioProcessorEditor {
    public:
        EnvelopeFilterAudioProcessorEditor(EnvelopeFilterAudioProcessor&);
        ~EnvelopeFilterAudioProcessorEditor() override;

        //==============================================================================
        void paint (juce::Graphics&) override;
        void resized() override;
    
        void updateGUI(const juce::String& mode);

    private:
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        EnvelopeFilterAudioProcessor& audioProcessor;
    
        std::unique_ptr<KnobGroup> knobGroup;
        std::unique_ptr<ModeSelector> modeSelector;
    
        juce::TextButton envFilterMode;
        juce::TextButton sampHoldMode;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeFilterAudioProcessorEditor)
};
