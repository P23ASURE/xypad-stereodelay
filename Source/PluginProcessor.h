/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Filters.h"
//==============================================================================
/**
*/
class XyPadAudioProcessor : public juce::AudioProcessor,
    AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    XyPadAudioProcessor();
    ~XyPadAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState& getApvts();
    
    void updateFilters(float hpfFreq, float lpfFreq) {
        // Assuming a fixed resonance value; adjust as needed.
        float resonance = 0.707;
        filters.setHPFParameters(hpfFreq, resonance);
        filters.setLPFParameters(lpfFreq, resonance);
    }

    float XyPadAudioProcessor::smoothParameterChange(float oldValue, float newValue)
    {
        // Adjust the factor based on how quickly you want the parameter to change
        const float smoothingFactor = 0.01f;
        return oldValue + smoothingFactor * (newValue - oldValue);
    }

private:
    float prevDelayTime = 0.0f;
    float prevDryWetValue = 0.0f;
    float prevHPFFrequency = 0.0f;
    float prevLPFFrequency = 0.0f;

    void parameterChanged(const String& parameterID, float newValue) override;
	
    AudioProcessorValueTreeState parameters;

    Filters filters;

    // Variabili per il buffer di ritardo
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength;
    int delayWritePosition;
    int delayReadPosition;

    int getChannelSpecificDelayTime(int channel, float delayTimeValue, float maxDelayTimeMs, double sampleRate);

	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XyPadAudioProcessor)
};
