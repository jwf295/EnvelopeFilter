/*
  ==============================================================================

    EnvelopeFollower.cpp
    Created: 9 Dec 2024 4:17:54pm
    Author:  Jeremy Freeman

  ==============================================================================
*/

#include "EnvelopeFollower.h"

EnvelopeFollower::EnvelopeFollower() {
    // default values
    this->attackCoefficient = 0.99f;
    this->envelope = 0.01f;
}

void EnvelopeFollower::setAttack(int attackTimeMS, float sampleRate) {
    // Convert attackTimeMS to attackSamples
    float attackSamples = attackTimeMS * sampleRate * 0.001f;
    
    // Use attackSamples as exponent of e to create attackCoefficient
    attackCoefficient = std::exp(-1.0f / attackSamples);
}

float EnvelopeFollower::processSample(float input) {
    // Handle negative amplitudes of waveform
    float absValInput = std::abs(input);
    
    // Smoothing function, an exponential moving average
    envelope = attackCoefficient * envelope + (1.0f - attackCoefficient) * absValInput;
    
    return envelope;
}
