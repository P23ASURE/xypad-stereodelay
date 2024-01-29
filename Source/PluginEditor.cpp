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


    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(dryWetMixSlider);
      
    delayTimeSlider.setRange(-17.5f, 17.5f, 0.01f); 

    dryWetMixSlider.setRange(0.f, 1.f, 0.01f); 

    addAndMakeVisible(delayLabel);
    addAndMakeVisible(mixLabel);

    delayLabel.attachToComponent(&delayTimeSlider, false);
    mixLabel.attachToComponent(&dryWetMixSlider, false);

    xyPad.registerSlider(&dryWetMixSlider, Gui::XyPad::Axis::Y);
    xyPad.registerSlider(&delayTimeSlider, Gui::XyPad::Axis::X);


    hpfLpfSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    hpfLpfSlider.setRange(20.0, 20000.0);
    hpfLpfSlider.setMinValue(400.0); 
    hpfLpfSlider.setMaxValue(5000.0);
    hpfLpfSlider.addListener(this);


    hpfLabel.setText("HPF: 0", juce::dontSendNotification);
    lpfLabel.setText("LPF: 0", juce::dontSendNotification);

    hpfLabel.setBounds(10, 30, 100, 20); 
    lpfLabel.setBounds(10, 60, 100, 20); 
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
    
    const int sliderHeight = 20;
    const int labelHeight = 20;
    const int labelWidth = 100;
    const int verticalSpaceBelowXyPad = 10; 
    const int horizontalSpace = 20; 
    const int extraSpaceForLPFLabel = 240; 
        
    // setResizable(true, true);
    // setResizeLimits(400, 300, 800, 600); 

    auto bounds = getLocalBounds().reduced(20);

    
    auto widthForDelaySlider = bounds.proportionOfWidth(0.25f);
    auto widthForXyPad = bounds.proportionOfWidth(0.5f);
    auto widthForDryWetSlider = bounds.getWidth() - widthForDelaySlider - widthForXyPad;

    delayTimeSlider.setBounds(bounds.removeFromLeft(widthForDelaySlider));
    xyPad.setBounds(bounds.removeFromLeft(widthForXyPad).reduced(20));
    dryWetMixSlider.setBounds(bounds.removeFromLeft(widthForDryWetSlider));

    // Logging/Debug
    delayTimeSlider.onValueChange = [this]() {
        DBG("Delay Time Slider Value Changed: " << delayTimeSlider.getValue());
        };

    dryWetMixSlider.onValueChange = [this]() {
        DBG("Dry/Wet Mix Slider Value Changed: " << dryWetMixSlider.getValue());
        };

    int yPositionForTwoValueSlider = xyPad.getBounds().getBottom() + verticalSpaceBelowXyPad;
    hpfLpfSlider.setBounds(horizontalSpace, yPositionForTwoValueSlider, getWidth() - 2 * horizontalSpace, sliderHeight);

    hpfLabel.setBounds(horizontalSpace, yPositionForTwoValueSlider - (labelHeight + 5), labelWidth, labelHeight);

    lpfLabel.setBounds(horizontalSpace + labelWidth + extraSpaceForLPFLabel, yPositionForTwoValueSlider - (labelHeight + 5), labelWidth, labelHeight);


    const int textBoxHeight = 20; 
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, labelWidth, textBoxHeight);
    dryWetMixSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, labelWidth, textBoxHeight);
}

void XyPadAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &hpfLpfSlider) 
    {
        auto hpfFreq = hpfLpfSlider.getMinValue();
        auto lpfFreq = hpfLpfSlider.getMaxValue();

        hpfLabel.setText("HPF: " + juce::String(hpfFreq, 4), juce::dontSendNotification);
        lpfLabel.setText("LPF: " + juce::String(lpfFreq, 4), juce::dontSendNotification);

        audioProcessor.getApvts().getParameter("hpfFrequency")->setValueNotifyingHost(hpfFreq / 20000.0);
        audioProcessor.getApvts().getParameter("lpfFrequency")->setValueNotifyingHost(lpfFreq / 20000.0);
    }
}

