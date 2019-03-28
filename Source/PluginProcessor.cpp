/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionOverdrivePluginAudioProcessor::DistortionOverdrivePluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
    mixingLevel(0.5f),
    effect(overdrive),
    overdriveParameter(0.0f),
    distortionParameter(0.0f),
    blockLength(0)
#endif
{
}

DistortionOverdrivePluginAudioProcessor::~DistortionOverdrivePluginAudioProcessor()
{
}

//==============================================================================
const String DistortionOverdrivePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionOverdrivePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionOverdrivePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionOverdrivePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionOverdrivePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionOverdrivePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionOverdrivePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionOverdrivePluginAudioProcessor::setCurrentProgram (int index)
{
}

const String DistortionOverdrivePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionOverdrivePluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DistortionOverdrivePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    blockLength = samplesPerBlock;
}

void DistortionOverdrivePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionOverdrivePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DistortionOverdrivePluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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

    switch (effect)
    {
        case overdrive:
            processOverdriveBlock(buffer);
            break;
        case distortion:
            processDistortionBlock(buffer);
            break;
    }
}


void DistortionOverdrivePluginAudioProcessor::processOverdriveBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto processedSample = channelData[0];
        //auto* channelData1 = buffer.getWritePointer (0);
        //auto* channelData2 = buffer.getWritePointer (1);
        for (auto j = 0; j < blockLength; ++j)
        {
            if (std::abs(channelData[j]) < overdriveParameter)
                processedSample = channelData[j];
            else
                if (std::abs(channelData[j]) < 2*overdriveParameter)
                    processedSample = ((channelData[j] > 0) ? 1 : -1) * ((3 - std::pow((2 - std::abs(channelData[j])*3), 2)) / 3.0);
                else
                    processedSample = (channelData[j] > 0) ? 1 : -1;
            processedSample *= std::sqrt(overdriveParameter * 2); // level compensation (heuristics)
            channelData[j] = processedSample * mixingLevel + channelData[j] * (1.0 - mixingLevel);
        }
    }
}

void DistortionOverdrivePluginAudioProcessor::processDistortionBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto processedSample = channelData[0];
        //auto* channelData1 = buffer.getWritePointer (0);
        //auto* channelData2 = buffer.getWritePointer (1);
        for (auto j = 0; j < blockLength; ++j)
        {
            processedSample = ((channelData[j] > 0) ? 1 : -1) * (1 - std::exp(-std::abs(channelData[j] * distortionParameter)));
            processedSample /= std::sqrt(distortionParameter); // level compensation (heuristics)
            channelData[j] = processedSample * mixingLevel + channelData[j] * (1.0 - mixingLevel);
        }
    }
}



//==============================================================================
bool DistortionOverdrivePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DistortionOverdrivePluginAudioProcessor::createEditor()
{
    return new DistortionOverdrivePluginAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionOverdrivePluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DistortionOverdrivePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionOverdrivePluginAudioProcessor();
}
