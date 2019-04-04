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
    EffectComponent(const String& componentName, const std::vector<SliderInitValues>& sliderValues);
    
    //===========================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    double GetSliderValue(const String& sliderName);
    
    // Extracts the index of the corresponding slider from the map. If it is not the member of the map, then returns -1
    String ExtractSliderName(Slider* sliderPtr);
    
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

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider* slider) override;

private:
    static const Image EMPTY_IMAGE;
    
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
    };
    
    void ChangeTypeOfEffect();
    
    //this function should be implemented only in the beginning!
    void SetDefaultParameters();
    
    DistortionOverdrivePluginAudioProcessor& processor;
    
    std::vector<SliderInitValues> sliderOverdrive;
    std::vector<SliderInitValues> sliderDistortion;
    
    std::unique_ptr<EffectComponent> overdriveComponent;
    std::unique_ptr<EffectComponent> distortionComponent;
    
    ImageButtonProps buttonProps;
    ImageButton isOverdriveButton;
    Slider drywetSlider;
    Label drywetLabel;
    
    bool isOverdrive;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionOverdrivePluginAudioProcessorEditor)
};
