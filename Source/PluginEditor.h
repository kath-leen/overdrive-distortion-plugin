/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "vector"

//==============================================================================
/**
*/
struct SliderInitValues
{
    String sliderName;
    double startValue;
    double stopValue;
    double intervalValue;
    double defaultValue;
    String suffix;
    Slider::Listener* listener;
    SliderInitValues(const String&& name, Slider::Listener* inListener, double start, double stop, double interval, double def = 0.0, const String suf = "") :
        sliderName(name),
        startValue(start),
        stopValue(stop),
        intervalValue(interval),
        defaultValue(def),
        suffix(suf),
        listener(inListener)
    {}
};

class EffectComponent : public Component
{
public:
    EffectComponent(const String& componentName, std::vector<SliderInitValues> sliderValues);
    ~EffectComponent() {};
    
    //===========================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    double GetSliderValue(String sliderName)
    {
        std::shared_ptr<Slider> sl = sliderMap.at(sliderName);
        return sl->getValue();
    }
    
    // Extracts the index of the corresponding slider from the map. If it is not the member of the map, then returns -1
    int ExtractSliderIdx(Slider* sliderPtr);
    
private:
    String effectName;
    std::map<String, std::shared_ptr<Slider>> sliderMap;
    std::map<String, std::shared_ptr<Label>> sliderLabelMap;
    Label nameLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectComponent)
};


class DistortionOverdrivePluginAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    DistortionOverdrivePluginAudioProcessorEditor (DistortionOverdrivePluginAudioProcessor&);
    ~DistortionOverdrivePluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider* slider) override;

private:
    
    struct ImageButtonProps {
        std::vector<Image> states;
        int statesQuantity;
        int currentStateIdx;
        
        ImageButtonProps(int quantity): statesQuantity(quantity), currentStateIdx(0) {};
        void ChangeStateIdx()
        {
            jassert(states.size() == statesQuantity);
            currentStateIdx = (currentStateIdx + 1) % statesQuantity;
        };
        
//        Image state1; //TODO: multiple states (in case of many effects)
//        Image state2;
//        Image* currentState;
//        bool isState1;
//        ImageButtonProps(Image&& st1, Image&& st2) : state1(st1), state2(st2), currentState(&st1), isState1(true) {};
//        void ChangeState()
//        {
//            currentState = (isState1) ? &state2 : &state1;
//            isState1 = !isState1;
//        }
    };
    
    
    void ChangeTypeOfEffect();
    
    //this function should be implemented only in the beginning!
    void SetDefaultParameters();
    
    DistortionOverdrivePluginAudioProcessor& processor;
    
    std::vector<SliderInitValues> sliderOverdrive;
    std::vector<SliderInitValues> sliderDistortion;
    
    ImageButtonProps buttonProps;
    
    EffectComponent* OverdriveComponent;
    EffectComponent* DistortionComponent;
    ImageButton isOverdriveButton;
    Slider drywetSlider;
    
    bool isOverdrive;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionOverdrivePluginAudioProcessorEditor)
};
