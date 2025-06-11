/*
  ==============================================================================

    KnobGroup.cpp
    Created: 2 Jun 2025 3:51:28pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#include "KnobGroup.h"

//==============================================================================

void KnobGroup::addKnob(const juce::String& paramID, std::unique_ptr<KnobComponent> knob) {
    if (knob) {
        knobs[paramID] = std::move(knob);
        addAndMakeVisible(*knobs[paramID]);
        resized();
    }
}

void KnobGroup::setKnobVisibility(const juce::String& paramID, bool isVisible) {
    if (knobs.find(paramID) != knobs.end()) {
        knobs[paramID]->setVisible(isVisible);
        resized();
    }
}

void KnobGroup::setNumVisibleKnobs(int numVisibleKnobs) {
    this->numVisibleKnobs = numVisibleKnobs;
}

int KnobGroup::getNumVisibleKnobs() {
    return numVisibleKnobs;
}

void KnobGroup::resized() {
    int knobWidth = 100;
    int knobHeight = 150;
    int spacing = 0;
    if (numVisibleKnobs == 4) {
        spacing = 50;
    }
    else if (numVisibleKnobs == 5) {
        spacing = 30;
    }
    int x = 0;
    for (auto& [id, knob] : knobs) {
        if (knob->isVisible()) {
            knob->setBounds(x, 0, knobWidth, knobHeight);
            x += knobWidth + spacing;
        }
    }
}
