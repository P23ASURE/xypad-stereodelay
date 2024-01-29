/*
  ==============================================================================

    Filters.h
    Created: 29 Dec 2023 9:56:21pm
    Author:  Marco

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Filters {
public:
    Filters();
    Filters(double sampleRate);

    void setHPFParameters(float frequency, float resonance);
    void setLPFParameters(float frequency, float resonance);
    void process(AudioBuffer<float>& buffer);

    void setSampleRate(double sampleRate);
private:
    float currentHPFFrequency;
    float currentLPFFrequency;

    double currentSampleRate;
    IIRFilter hpf, lpf;

    void updateHPFCoefficients(float frequency, float resonance);
    void updateLPFCoefficients(float frequency, float resonance);
};
