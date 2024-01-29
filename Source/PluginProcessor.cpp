#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XyPadAudioProcessor::XyPadAudioProcessor() :
    AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "xypad", {
    std::make_unique<AudioParameterFloat>("delayTime", "Delay Time", NormalisableRange<float>(-17.5f, 17.5f, 0.01f), 0.f),
    std::make_unique<AudioParameterFloat>("dryWetMix", "Dry/Wet Mix", NormalisableRange<float>(0.f, 1.f, 0.01f), 0.5f),
    std::make_unique<AudioParameterFloat>("hpfFrequency", "HPF Frequency", NormalisableRange<float>(20.0f, 20000.0f, 1.0f), 440.0f),
    std::make_unique<AudioParameterFloat>("lpfFrequency", "LPF Frequency", NormalisableRange<float>(20.0f, 20000.0f, 1.0f), 5000.0f)
    })
{

}

XyPadAudioProcessor::~XyPadAudioProcessor()
{

}

//==============================================================================
const juce::String XyPadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool XyPadAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool XyPadAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool XyPadAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double XyPadAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int XyPadAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int XyPadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void XyPadAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String XyPadAudioProcessor::getProgramName (int index)
{
    return {};
}

void XyPadAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void XyPadAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    filters.setSampleRate(sampleRate);

    const double maxDelayTimeMs = 35.0;
    delayBufferLength = static_cast<int>(sampleRate * maxDelayTimeMs / 1000.0);

    if (delayBuffer.getNumChannels() != getTotalNumInputChannels() || delayBuffer.getNumSamples() != delayBufferLength)
    {
        delayBuffer.setSize(getTotalNumInputChannels(), delayBufferLength);
        delayBuffer.clear(); 
        delayWritePosition = 0;
    }
}

void XyPadAudioProcessor::releaseResources()
{
     delayBuffer.setSize(0, 0);    
}


bool XyPadAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}


void XyPadAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
  

    auto delayTimeValue = parameters.getRawParameterValue("delayTime")->load();
    auto dryWetValue = parameters.getRawParameterValue("dryWetMix")->load();
    auto hpfFreq = parameters.getRawParameterValue("hpfFrequency")->load();
    auto lpfFreq = parameters.getRawParameterValue("lpfFrequency")->load();

    DBG("Delay Time Value: " << delayTimeValue << ", Dry/Wet Value: " << dryWetValue
        << ", HPF Frequency: " << hpfFreq << ", LPF Frequency: " << lpfFreq);
    
    filters.setHPFParameters(hpfFreq, 0.707f); 
    filters.setLPFParameters(lpfFreq, 0.707f);
    filters.process(buffer);

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(channel);
        delayData[delayWritePosition % delayBufferLength] = 0.0f;

        int delayTimeInSamples = getChannelSpecificDelayTime(channel, delayTimeValue, 35.0f, getSampleRate());
        DBG("Channel: " << channel << ", Delay Time in Samples: " << delayTimeInSamples);
      
        if (delayTimeInSamples > 0)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                const int localDelayReadPos = (delayWritePosition - delayTimeInSamples + delayBufferLength) % delayBufferLength;
                delayData[delayWritePosition] = channelData[i];

                float wetSignal = delayData[localDelayReadPos];
                float drySignal = channelData[i];
                channelData[i] = drySignal * (1.0f - dryWetValue) + wetSignal * dryWetValue;
            }
        }
        else
        {
            DBG("No delay applied to Channel: " << channel);
        }

        delayWritePosition = (delayWritePosition + buffer.getNumSamples()) % delayBufferLength;
    }
}


int XyPadAudioProcessor::getChannelSpecificDelayTime(int channel, float delayTimeValue, float maxDelayTimeMs, double sampleRate)
{
    // Calculate a scaling factor based on half of the delayTimeValue range. (-17.5 a 17.5)
    float scaleFactor = std::abs(delayTimeValue) / 17.5f;
    float actualDelayTimeMs = scaleFactor * maxDelayTimeMs;

    
    int delayTimeInSamples = static_cast<int>(actualDelayTimeMs * sampleRate / 1000.0);

    // If delayTimesValues is negative, apply delay on left channel
    // add delay on right channel if delayTimeValues is positive
    if ((channel == 0 && delayTimeValue < 0) || (channel == 1 && delayTimeValue > 0))
    {
        DBG("Channell: " << (channel == 0 ? "Left" : "Right")
            << ", Delay Value: " << delayTimeValue
            << ", Delay Tempo (ms): " << actualDelayTimeMs
            << ", Delay Time (Samples): " << delayTimeInSamples);

        return delayTimeInSamples;
    }

    DBG("No Delay: " << (channel == 0 ? "Sinistro" : "Destro"));
    return 0; // Nessun delay per il canale se la condizione non è soddisfatta
}



//==============================================================================
bool XyPadAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* XyPadAudioProcessor::createEditor()
{
    return new XyPadAudioProcessorEditor (*this);
}

//==============================================================================
void XyPadAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void XyPadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

AudioProcessorValueTreeState& XyPadAudioProcessor::getApvts()
{
    
    return parameters;
    
}


void XyPadAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID.equalsIgnoreCase("delayTime"))
    {
        // Smoothly update delayTime
        float smoothValue = smoothParameterChange(prevDelayTime, newValue);
        DBG("Parameter Changed: " << parameterID << " to " << smoothValue);
        prevDelayTime = smoothValue;

        // Handle delayTime parameter change
        // Update the delayTime parameter in your code as needed
    }
    else if (parameterID.equalsIgnoreCase("dryWetMix"))
    {
        // Smoothly update dryWetMix
        float smoothValue = smoothParameterChange(prevDryWetValue, newValue);
        // ...
    }
    else if (parameterID.equalsIgnoreCase("hpfFrequency"))
    {
        // Smoothly update hpfFrequency
        float smoothValue = smoothParameterChange(prevHPFFrequency, newValue);
        DBG("HPF Frequency Parameter Changed: " << smoothValue);
        prevHPFFrequency = smoothValue;

        // Handle HPF frequency parameter change
        // Update the HPF parameter in your Filters object
        filters.setHPFParameters(smoothValue, 0.707f); // Example resonance value
    }
    else if (parameterID.equalsIgnoreCase("lpfFrequency"))
    {
        // Smoothly update lpfFrequency
        float smoothValue = smoothParameterChange(prevLPFFrequency, newValue);
        DBG("LPF Frequency Parameter Changed: " << smoothValue);
        prevLPFFrequency = smoothValue;

        // Handle LPF frequency parameter change
        // Update the LPF parameter in your Filters object
        filters.setLPFParameters(smoothValue, 0.707f); // Example resonance value
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XyPadAudioProcessor();
}
