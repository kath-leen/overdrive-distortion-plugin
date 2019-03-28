/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class DistortionOverdrivePluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DistortionOverdrivePluginAudioProcessor();
    ~DistortionOverdrivePluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void ChangeMixingLevel(float inputLevel) { mixingLevel = inputLevel; };
    void ChangeEffect(bool isOverdrive) { effect = (isOverdrive == true) ? overdrive : distortion; };
    void ChangeParameter(bool isOverdrive, float value)
    {
        if (isOverdrive)
            overdriveParameter = value;
        else
            distortionParameter = value;
    };
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionOverdrivePluginAudioProcessor)
    enum Effects {overdrive = 0, distortion};
    
    void processOverdriveBlock (AudioBuffer<float>& buffer);
    void processDistortionBlock (AudioBuffer<float>& buffer);
    
    float mixingLevel;
    Effects effect;
    float overdriveParameter;
    float distortionParameter;
    int blockLength;
};
