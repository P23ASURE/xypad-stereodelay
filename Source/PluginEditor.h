/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/XyPad.h"

//==============================================================================
/**
*/
class XyPadAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    XyPadAudioProcessorEditor (XyPadAudioProcessor&);
    ~XyPadAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
	
    XyPadAudioProcessor& audioProcessor;

    Slider delayTimeSlider{ Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow };
    Slider dryWetMixSlider{ Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow };
    SliderAttachment delayTimeAttachment, dryWetMixAttachment;
    Label mixLabel{ "mixLabel", "Mix" };
    Label delayLabel{ "delayLabel", "Delay" };
    Label delayDisplayLabel{ "delayDisplayLabel", "DelayLRms" };
    Gui::XyPad xyPad;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XyPadAudioProcessorEditor)
};
