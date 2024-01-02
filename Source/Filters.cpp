#include "Filters.h"

Filters::Filters() : currentSampleRate(44100) { // Initialize with a default sample rate
    hpf.setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, 400, 0.707));
    lpf.setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, 5000, 0.707));
}

void Filters::setHPFParameters(float frequency, float resonance) {
    currentHPFFrequency = frequency;
    updateHPFCoefficients(currentHPFFrequency, resonance);
}

void Filters::setLPFParameters(float frequency, float resonance) {
    currentLPFFrequency = frequency;
    updateLPFCoefficients(currentLPFFrequency, resonance);
}

void Filters::process(AudioBuffer<float>& buffer) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        
        // Applica il filtro HPF al canale con un parametro di transizione
        hpf.setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, currentHPFFrequency, 0.707));
        hpf.processSamples(buffer.getWritePointer(channel), buffer.getNumSamples());

        // Applica il filtro LPF al canale con un parametro di transizione
        lpf.setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, currentLPFFrequency, 0.707));
        lpf.processSamples(buffer.getWritePointer(channel), buffer.getNumSamples());
    }
}

void Filters::updateHPFCoefficients(float frequency, float resonance) {
    hpf.setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, frequency, resonance));
}

void Filters::updateLPFCoefficients(float frequency, float resonance) {
    lpf.setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, frequency, resonance));
}

void Filters::setSampleRate(double sampleRate) {
    currentSampleRate = sampleRate;
    // Optionally update filter coefficients here if necessary
}