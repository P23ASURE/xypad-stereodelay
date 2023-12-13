/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XyPadAudioProcessorEditor::XyPadAudioProcessorEditor(XyPadAudioProcessor& p) :
    AudioProcessorEditor(&p),
    audioProcessor(p),
    delayTimeAttachment(audioProcessor.getApvts(), "delayTime", delayTimeSlider),
    dryWetMixAttachment(audioProcessor.getApvts(), "dryWetMix", dryWetMixSlider)
{
    addAndMakeVisible(xyPad);

    // Aggiungi e rendi visibili gli slider per delayTime e dryWetMix
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(dryWetMixSlider);

    // Configurazione degli slider
    delayTimeSlider.setRange(-17.5f, 17.5f, 0.01f); // Aggiornato per il nuovo intervallo

    dryWetMixSlider.setRange(0.f, 1.f, 0.01f);  // Imposta l'intervallo e lo step

    // Aggiungi e rendi visibili le label
    addAndMakeVisible(delayLabel);
    addAndMakeVisible(mixLabel);

    // Configura le label
    delayLabel.attachToComponent(&delayTimeSlider, false);
    mixLabel.attachToComponent(&dryWetMixSlider, false);

    xyPad.registerSlider(&dryWetMixSlider, Gui::XyPad::Axis::Y);
    xyPad.registerSlider(&delayTimeSlider, Gui::XyPad::Axis::X);

    // Imposta la dimensione dell'editor
    setSize(500, 300);
}

XyPadAudioProcessorEditor::~XyPadAudioProcessorEditor()
{
    xyPad.deregisterSlider(&delayTimeSlider);
    xyPad.deregisterSlider(&dryWetMixSlider);
}

//==============================================================================
void XyPadAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));    
}

void XyPadAudioProcessorEditor::resized()
{
    const auto container = getLocalBounds().reduced(20);
    auto bounds = container;
    delayTimeSlider.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.25f)));
    xyPad.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.5)).reduced(20));
    dryWetMixSlider.setBounds(bounds);

    // Aggiungi il logging per i cambiamenti di valore degli slider
    delayTimeSlider.onValueChange = [this]() {
        DBG("Delay Time Slider Value Changed: " << delayTimeSlider.getValue());
        };

    dryWetMixSlider.onValueChange = [this]() {
        DBG("Dry/Wet Mix Slider Value Changed: " << dryWetMixSlider.getValue());
        };
}

