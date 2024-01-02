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

    // Initialize the two-value slider
    hpfLpfSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    hpfLpfSlider.setRange(20.0, 20000.0);
    hpfLpfSlider.setMinValue(400.0); // initial HPF value
    hpfLpfSlider.setMaxValue(5000.0); // initial LPF value
    hpfLpfSlider.addListener(this);

    // Inizializza le label
    hpfLabel.setText("HPF: 0", juce::dontSendNotification);
    lpfLabel.setText("LPF: 0", juce::dontSendNotification);
    // Posiziona le label
    hpfLabel.setBounds(10, 30, 100, 20); // Ad esempio, a 10 pixel dal lato sinistro e 30 pixel dall'alto
    lpfLabel.setBounds(10, 60, 100, 20); // Posizionato sotto l'HPF label
    hpfLpfSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(hpfLabel);
    addAndMakeVisible(lpfLabel);

    addAndMakeVisible(hpfLpfSlider);

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
    // Dimensioni fisse per gli slider e le label
    const int sliderHeight = 20;
    const int labelHeight = 20;
    const int labelWidth = 100;
    const int verticalSpaceBelowXyPad = 10; // Spazio aggiuntivo minimo sotto xyPad
    const int horizontalSpace = 20; // Spazio orizzontale dai bordi
    const int extraSpaceForLPFLabel = 240; // Spazio aggiuntivo per la label LPF

    // Rendi l'interfaccia utente resizable con limiti di dimensione definiti
    setResizable(true, true);
    setResizeLimits(400, 300, 800, 600); // Imposta i tuoi limiti di dimensione

    auto bounds = getLocalBounds().reduced(20);

    // Calcola le proporzioni per gli slider
    auto widthForDelaySlider = bounds.proportionOfWidth(0.25f);
    auto widthForXyPad = bounds.proportionOfWidth(0.5f);
    auto widthForDryWetSlider = bounds.getWidth() - widthForDelaySlider - widthForXyPad;

    // Posiziona gli slider e il xyPad
    delayTimeSlider.setBounds(bounds.removeFromLeft(widthForDelaySlider));
    xyPad.setBounds(bounds.removeFromLeft(widthForXyPad).reduced(20));
    dryWetMixSlider.setBounds(bounds.removeFromLeft(widthForDryWetSlider));

    // Logging per i cambiamenti di valore degli slider
    delayTimeSlider.onValueChange = [this]() {
        DBG("Delay Time Slider Value Changed: " << delayTimeSlider.getValue());
        };

    dryWetMixSlider.onValueChange = [this]() {
        DBG("Dry/Wet Mix Slider Value Changed: " << dryWetMixSlider.getValue());
        };

    // Regola lo spazio per lo slider a due valori
    int yPositionForTwoValueSlider = xyPad.getBounds().getBottom() + verticalSpaceBelowXyPad;
    hpfLpfSlider.setBounds(horizontalSpace, yPositionForTwoValueSlider, getWidth() - 2 * horizontalSpace, sliderHeight);

    // Aggiorna anche la posizione delle label correlate
    hpfLabel.setBounds(horizontalSpace, yPositionForTwoValueSlider - (labelHeight + 5), labelWidth, labelHeight);
    // Sposta la label LPF più a destra
    lpfLabel.setBounds(horizontalSpace + labelWidth + extraSpaceForLPFLabel, yPositionForTwoValueSlider - (labelHeight + 5), labelWidth, labelHeight);

    // Posiziona le caselle di testo degli slider di Delay e Mix più in alto
    const int textBoxHeight = 20; // Altezza della casella di testo
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, labelWidth, textBoxHeight);
    dryWetMixSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, labelWidth, textBoxHeight);
}

void XyPadAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &hpfLpfSlider) // Assicurati che sia lo slider corretto
    {
        auto hpfFreq = hpfLpfSlider.getMinValue();
        auto lpfFreq = hpfLpfSlider.getMaxValue();

        // Aggiorna il testo delle label
        hpfLabel.setText("HPF: " + juce::String(hpfFreq, 4), juce::dontSendNotification);
        lpfLabel.setText("LPF: " + juce::String(lpfFreq, 4), juce::dontSendNotification);

        // Aggiorna i parametri HPF e LPF nel processore
        audioProcessor.getApvts().getParameter("hpfFrequency")->setValueNotifyingHost(hpfFreq / 20000.0);
        audioProcessor.getApvts().getParameter("lpfFrequency")->setValueNotifyingHost(lpfFreq / 20000.0);
    }
}

