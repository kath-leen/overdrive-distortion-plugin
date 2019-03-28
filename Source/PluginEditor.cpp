/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

EffectComponent::EffectComponent(const String& componentName, std::vector<SliderInitValues> sliderValues) :
    effectName(componentName)
{
    Slider* sliderPtr = 0;
    Label* labelPtr = 0;
    for (auto i = 0; i < sliderValues.size(); ++i)
    {
        sliderMap[sliderValues[i].sliderName] = std::make_shared<Slider>();
        sliderPtr = sliderMap[sliderValues[i].sliderName].get();
        
        addAndMakeVisible(*sliderPtr);
        sliderPtr->setRange(sliderValues[i].startValue, sliderValues[i].stopValue, sliderValues[i].intervalValue);
        sliderPtr->setValue(sliderValues[i].defaultValue);
        sliderPtr->setTextValueSuffix (sliderValues[i].suffix);
        //sliderPtr->setColour(Slider::thumbColourId, Colours::white);
        //sliderPtr->setColour(Slider::trackColourId, Colours::lightgrey);
        sliderPtr->addListener(sliderValues[i].listener);
        
        sliderLabelMap[sliderValues[i].sliderName] = std::make_shared<Label>();
        labelPtr = sliderLabelMap[sliderValues[i].sliderName].get();
        
        addAndMakeVisible(*labelPtr);
        labelPtr->setFont(Font(14.0f));
        labelPtr->setText (sliderValues[i].sliderName, dontSendNotification);
        labelPtr->setColour(Label::textColourId, Colours::whitesmoke);
        labelPtr->attachToComponent(sliderPtr, true);
    }
    
    addAndMakeVisible(nameLabel);
    nameLabel.setFont(Font(18.0f));
    nameLabel.setText(effectName, dontSendNotification);
    //nameLabel.setColour(Label::textColourId, Colours::whitesmoke);
    nameLabel.setJustificationType(Justification::centred);
}

//==============================================================================

void EffectComponent::paint(Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}

void EffectComponent::resized()
{
    auto r = getLocalBounds();
    int sideGap = 10;
    nameLabel.setBounds(sideGap, sideGap, r.getWidth() - 2*sideGap, 2*sideGap);
    
    int sliderHeightGap = 20;
    double startingPoint = r.getHeight()/2 - sliderHeightGap/2 - (sliderHeightGap/2) * (sliderMap.size() - 1);
    for (std::map<String, std::shared_ptr<Slider>>::iterator it = sliderMap.begin(); it != sliderMap.end(); ++it)
    {
        it->second->setBounds(sideGap, startingPoint, r.getWidth() - 2*sideGap, sliderHeightGap);
        startingPoint += sliderHeightGap;
    }
}

int EffectComponent::ExtractSliderIdx(Slider* sliderPtr)
{
    int idx = 0;
    for (std::map<String, std::shared_ptr<Slider>>::iterator it = sliderMap.begin(); it != sliderMap.end(); ++it)
    {
        if (it->second.get() == sliderPtr)
            break;
        ++idx;
    }
    if (idx >= sliderMap.size())
        idx = -1;
    return idx;
}

//==============================================================================
//==============================================================================

DistortionOverdrivePluginAudioProcessorEditor::DistortionOverdrivePluginAudioProcessorEditor (DistortionOverdrivePluginAudioProcessor& p) :
        AudioProcessorEditor (&p),
        processor (p),
        sliderOverdrive({SliderInitValues("Overdrive Level", this, 0, 0.5, 0.01, 0.5)}),
        sliderDistortion({SliderInitValues("Distortion Level", this, 1, 30, 0.1)}),
//        buttonProps(ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/DistortionOverdrivePlugin/Images/toggle-dist.png")), ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/DistortionOverdrivePlugin/Images/toggle-overdr.png"))),
        buttonProps(2),
        isOverdriveButton("effectTypeButton"),
        isOverdrive(true)
{
    //add components
    OverdriveComponent = new EffectComponent("Overdrive", sliderOverdrive);
    DistortionComponent = new EffectComponent("Distortion", sliderDistortion);
    addAndMakeVisible(*OverdriveComponent);
    addAndMakeVisible(*DistortionComponent);
    OverdriveComponent->toBack();
    DistortionComponent->toBack();
    
    //update images for the effect button
    buttonProps.states.push_back(ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/ActualProjects/DistortionOverdrivePlugin/Images/toggle-overdr.png")));
    buttonProps.states.push_back(ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/ActualProjects/DistortionOverdrivePlugin/Images/toggle-dist.png")));
    
    //set the effect button
    addAndMakeVisible(isOverdriveButton);
    isOverdriveButton.setButtonText("Override / Distortion");
    isOverdriveButton.setImages(false, true, true, buttonProps.states[buttonProps.currentStateIdx], 1.0f, Colours::transparentWhite, Image(), 1.0f, Colours::transparentWhite, Image(), 1.0f, Colours::transparentWhite);
    isOverdriveButton.onClick = [this] {ChangeTypeOfEffect();};
    
    //set the dry/wet slider
    addAndMakeVisible(drywetSlider);
    drywetSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    drywetSlider.setRange(0, 100, 1);
    drywetSlider.setValue(50);
    drywetSlider.setTextValueSuffix("%");
    drywetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, true, 40, 20);
    //drywetSlider.setColour(Slider::thumbColourId, Colours::white);
    //drywetSlider.setColour(Slider::trackColourId, Colours::lightgrey);
    drywetSlider.onValueChange = [this] {processor.ChangeMixingLevel((float)drywetSlider.getValue() / 100.0f);};
    
    //default parameters setting
    SetDefaultParameters();
    
    setSize (400, 300);
}

DistortionOverdrivePluginAudioProcessorEditor::~DistortionOverdrivePluginAudioProcessorEditor()
{
    delete OverdriveComponent;
    delete DistortionComponent;
}

//==============================================================================
void DistortionOverdrivePluginAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::white);
    getLookAndFeel().setColour(Slider::trackColourId, Colours::lightgrey);
    getLookAndFeel().setColour(Label::textColourId, Colours::whitesmoke);
}

void DistortionOverdrivePluginAudioProcessorEditor::resized()
{
    auto gap = getWidth() * 0.02f;
    auto width = (getWidth() - gap * 3)/2;
    auto height = getHeight() - gap * 2;
    OverdriveComponent->setBounds (gap, gap, width, height);
    DistortionComponent->setBounds (gap * 2 + width, gap, width, height);
    
    auto effectTypeButtonShift = getWidth() * 0.2f;
    isOverdriveButton.setBounds (getWidth()/2 - effectTypeButtonShift, gap * 3, effectTypeButtonShift * 2, 40);
    
    auto rotateSliderRadius = getWidth()*0.12f;
    drywetSlider.setBounds(getWidth()/2 - rotateSliderRadius, getHeight() - rotateSliderRadius * 2.2f, rotateSliderRadius * 2, rotateSliderRadius * 2);
}

void DistortionOverdrivePluginAudioProcessorEditor::ChangeTypeOfEffect()
{
    buttonProps.ChangeStateIdx();
    isOverdriveButton.setImages(false, true, true, buttonProps.states[buttonProps.currentStateIdx], 1.0f, Colours::transparentWhite, Image(), 1.0f, Colours::transparentWhite, Image(), 1.0f, Colours::transparentWhite);
    isOverdrive = !isOverdrive;
    processor.ChangeEffect(isOverdrive);
}

void DistortionOverdrivePluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (OverdriveComponent->ExtractSliderIdx(slider) != -1)
        processor.ChangeParameter(true, slider->getValue());
    if (DistortionComponent->ExtractSliderIdx(slider) != -1)
        processor.ChangeParameter(false, slider->getValue());
}

void DistortionOverdrivePluginAudioProcessorEditor::SetDefaultParameters()
{
    processor.ChangeMixingLevel((float)drywetSlider.getValue() / 100.0f);
    processor.ChangeEffect(isOverdrive);
    processor.ChangeParameter(true, sliderOverdrive[0].defaultValue);
    processor.ChangeParameter(false, sliderDistortion[0].defaultValue);
}
