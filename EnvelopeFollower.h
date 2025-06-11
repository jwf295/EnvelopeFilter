/*
  ==============================================================================

    EnvelopeFollower.h
    Created: 9 Dec 2024 4:17:32pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class EnvelopeFollower {
    public:
        EnvelopeFollower();
        void setAttack(int attackTimeMS, float sampleRate);
        float processSample(float input);
    
    private:
        float attackCoefficient;
        float envelope;
};
