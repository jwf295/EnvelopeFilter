/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Initialize parameters
EnvelopeFilterAudioProcessor::EnvelopeFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters(*this, nullptr, "Parameters", createParameterLayout()),
    lastAttackTime(100),
    lastSpeedMS(100),
    activeMode("EF")
#endif
{
    // Start timer based on parameter
    startTimer(parameters.getRawParameterValue("speedMS")->load());
    // Reset smoothed random value, ramp length is 0.01 seconds
    smoothedRandomValue.reset(sampleRate, 0.01);
}

EnvelopeFilterAudioProcessor::~EnvelopeFilterAudioProcessor()
{
}

//==============================================================================
const juce::String EnvelopeFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EnvelopeFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EnvelopeFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EnvelopeFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EnvelopeFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EnvelopeFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EnvelopeFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EnvelopeFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EnvelopeFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void EnvelopeFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EnvelopeFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Save sample rate
    this->sampleRate = sampleRate;
    
    // Set up parameter for filter.prepare, requires juce::uint32
    juce::dsp::ProcessSpec processSpec {sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumInputChannels())};
    
    // Prepare filters
    lowPassFilter.prepare(processSpec);
    highPassFilter.prepare(processSpec);
    bandPassFilter.prepare(processSpec);
    
    // Set filter types
    lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    highPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    bandPassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    
    // Get attack time from parameter and save value as lastAttackTime
    std::atomic<float>* attackTimeMS = parameters.getRawParameterValue("attackTimeMS");
    lastAttackTime = *attackTimeMS;
    
    // Set attack of envelope follower
    envelopeFollower.setAttack(*attackTimeMS, sampleRate);
}

void EnvelopeFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EnvelopeFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EnvelopeFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    
    // Get raw parameter values based on paramID
    std::atomic<float>* filterBlend = parameters.getRawParameterValue("filterBlend");
    std::atomic<float>* envelopeSensitivity = parameters.getRawParameterValue("envelopeSensitivity");
    std::atomic<float>* resonance = parameters.getRawParameterValue("resonance");
    std::atomic<float>* attackTimeMS = parameters.getRawParameterValue("attackTimeMS");
    
    // Get raw parameter value for speedMS (for S&H)
    std::atomic<float>* speedMS = parameters.getRawParameterValue("speedMS");

    // Only update attack time if slider has been changed
    if (*attackTimeMS != lastAttackTime) {
        envelopeFollower.setAttack(*attackTimeMS, sampleRate);
        lastAttackTime = *attackTimeMS;
    }
    
    // If S&H, update timer
    if (activeMode == "S&H" && *speedMS != lastSpeedMS) {
        startTimer(*speedMS);
        lastSpeedMS = *speedMS;
    }
    
    // Loop through channels
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        
        // Get write pointer
        auto* channelData = buffer.getWritePointer(channel);
        
        // Loop through samples
        for (int sample = 0; sample < buffer.getNumSamples(); ++ sample) {
            
            // Get input sample
            float inputSample = channelData[sample];
            
            // Track amplitude of incoming audio
            float envelope = envelopeFollower.processSample(inputSample);
            
            // Scale cutoff and apply envelope sensitivity
            float cutoffFreq = juce::jmap(envelope, 20.0f, 20000.0f) * (*envelopeSensitivity);
            
            if (activeMode == "S&H") {
                // If S&H, generate randomCutoffFreq by scaling heldRandomValue
                // Envelope + random modulation from smoothed value
                cutoffFreq += smoothedRandomValue.getNextValue();
            }
            
            // Set cutoff frequencies
            lowPassFilter.setCutoffFrequency(cutoffFreq);
            highPassFilter.setCutoffFrequency(cutoffFreq);
            bandPassFilter.setCutoffFrequency(cutoffFreq);
            
            // Clamp resonance value
            float clampedResonance = juce::jlimit(0.1f, 10.0f, resonance->load());
            
            // Set clamped resonance value for each filters
            lowPassFilter.setResonance(clampedResonance);
            highPassFilter.setResonance(clampedResonance);
            bandPassFilter.setResonance(clampedResonance);
            
            // Process sample for each filter
            float lowPass = lowPassFilter.processSample(channel, inputSample);
            float highPass = highPassFilter.processSample(channel, inputSample);
            float bandPass = bandPassFilter.processSample(channel, inputSample);
            
            // Blend filters
            // This math allows the blend control to combine the different filter types
            // 0.0 = totally high pass
            // 0.5 = totally band pass
            // 1.0 = totally low pass
            float blend = *filterBlend;
            float lowPassAmount = juce::jmax(0.0f, juce::jmin(1.0f, 1.0f - 2.0f * std::abs(blend - 1.0f)));
            float highPassAmount = juce::jmax(0.0f, juce::jmin(1.0f, 1.0f - 2.0f * std::abs(blend - 1.0f)));
            float bandPassAmount = 1.0f - lowPassAmount - highPassAmount;
            
            // Calculate based on amount of each filter
            float blendedOutput = lowPassAmount * lowPass + highPassAmount * highPass + bandPassAmount * bandPass;
            
            // Apply to sample
            channelData[sample] = blendedOutput;
        }
    }
    // Normalization
    normalizeBuffer(buffer);
}

//==============================================================================
bool EnvelopeFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EnvelopeFilterAudioProcessor::createEditor()
{
    return new EnvelopeFilterAudioProcessorEditor (*this);
}

//==============================================================================
void EnvelopeFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void EnvelopeFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml (*xmlState));
        }
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout EnvelopeFilterAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    // Add parameters
    //params.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("mode", 1), "Mode", juce::StringArray{"EF", "S&H"}, 0));
    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filterBlend", 1), "Filter Blend", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("envelopeSensitivity", 1), "Envelope Sensitivity", juce::NormalisableRange<float>(0.0f, 5.0f), 1.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("resonance", 1), "Resonance", juce::NormalisableRange<float>(0.0f, 10.0f), 1.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("attackTimeMS", 1), "Attack Time (MS)", juce::NormalisableRange<float>(1.0f, 500.0f), 100.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("speedMS", 1), "Speed (MS)", juce::NormalisableRange<float>(1.0f, 1000.0f), 100.0f));
    
    return params;
}

juce::AudioProcessorValueTreeState& EnvelopeFilterAudioProcessor::getParameters() {
    return parameters;
}

void EnvelopeFilterAudioProcessor::setActiveMode(const juce::String& mode) {
    activeMode = mode;
}

void EnvelopeFilterAudioProcessor::timerCallback() {
    // For S&H, generate random value from 0.0 to 1.0
    float newRandomValue = juce::Random::getSystemRandom().nextFloat();
    // Scale random value, map to freq range, use smoothing
    smoothedRandomValue.setTargetValue(juce::jmap(newRandomValue, 20.0f, 20000.0f));
}

void EnvelopeFilterAudioProcessor::updateModeImmediately() {
    if (activeMode == "EF") {
        // For S&H, start timer
        startTimer(*parameters.getRawParameterValue("speedMS"));
    }
    else if (activeMode == "S&H") {
        // For EF, stop timer
        stopTimer();
    }
}

void EnvelopeFilterAudioProcessor::normalizeBuffer(juce::AudioBuffer<float>& buffer) {
    float maxSample = 0.0f;
    
    // Track max sample found
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        maxSample = std::max(maxSample, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));
    }
    
    // If max sample exceeds 1.0f
    if (maxSample > 1.0f) {
        
        // Get scale factor
        float normalizationFactor = 1.0f / maxSample;
        
        // Scale down, call applyGain
        buffer.applyGain(normalizationFactor);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EnvelopeFilterAudioProcessor();
}
