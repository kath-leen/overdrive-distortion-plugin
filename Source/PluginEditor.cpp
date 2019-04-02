#include "PluginProcessor.h"
#include "PluginEditor.h"

const Image DistortionOverdrivePluginAudioProcessorEditor::EMPTY_IMAGE = Image();

EffectComponent::EffectComponent(const String& componentName, const std::vector<SliderInitValues> sliderValues) :
    effectName(componentName)
{
    for (auto i = 0; i < sliderValues.size(); ++i)
    {
        std::shared_ptr<Slider> sliderPtr = std::make_shared<Slider>();
        addAndMakeVisible(*sliderPtr);
        sliderPtr->setRange(sliderValues[i].startValue, sliderValues[i].stopValue, sliderValues[i].intervalValue);
        sliderPtr->setValue(sliderValues[i].defaultValue);
        sliderPtr->setTextValueSuffix (sliderValues[i].suffix);
        sliderPtr->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 40, 20);
        sliderPtr->addListener(sliderValues[i].listener);
        sliderMap[sliderValues[i].sliderName] = sliderPtr;
        
        std::shared_ptr<Label> labelPtr = std::make_shared<Label>();
        addAndMakeVisible(*labelPtr);
        labelPtr->setFont(Font(14.0f));
        labelPtr->setText (sliderValues[i].sliderName, dontSendNotification);
        labelPtr->attachToComponent(sliderPtr.get(), false);
        labelPtr->setJustificationType(Justification::bottomRight);
        sliderLabelMap[sliderValues[i].sliderName] = labelPtr;
    }
    
    addAndMakeVisible(nameLabel);
    nameLabel.setFont(Font(18.0f));
    nameLabel.setText(effectName, dontSendNotification);
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
    
    int sliderHeightGap = 30;
    double startingPoint = r.getHeight()/2 - sliderHeightGap/2 - (sliderHeightGap/2) * (sliderMap.size() - 1);
    for (auto &it : sliderMap)
    {
        it.second->setBounds(sideGap, startingPoint, r.getWidth() - 2*sideGap, sliderHeightGap);
        startingPoint += sliderHeightGap;
    }
}

String EffectComponent::ExtractSliderName(Slider* sliderPtr)
{
    String outputStr = String();
    for (auto &it : sliderMap)
    {
        if (it.second.get() == sliderPtr)
        {
            outputStr = it.first;
            break;
        }
    }
    return outputStr;
}

double EffectComponent::GetSliderValue(const String& sliderName)
{
    std::shared_ptr<Slider> sl = sliderMap.at(sliderName);
    return sl->getValue();
}

//==============================================================================
//==============================================================================

DistortionOverdrivePluginAudioProcessorEditor::DistortionOverdrivePluginAudioProcessorEditor (DistortionOverdrivePluginAudioProcessor& p) :
        AudioProcessorEditor (&p),
        processor (p),
        sliderOverdrive({ SliderInitValues("Overdrive Level", this, 0, 100, 1, 0, "%") }),
        sliderDistortion({ SliderInitValues("Distortion Level", this, 0, 100, 1, 0, "%") }),
        overdriveComponent(new EffectComponent("Overdrive", sliderOverdrive)),
        distortionComponent(new EffectComponent("Distortion", sliderDistortion)),
        buttonProps(2),
        isOverdriveButton("effectTypeButton"),
        isOverdrive(true)
{
    //add components
    //overdriveComponent = new EffectComponent("Overdrive", sliderOverdrive);
    //distortionComponent = new EffectComponent("Distortion", sliderDistortion);
    addAndMakeVisible(*overdriveComponent);
    addAndMakeVisible(*distortionComponent);
    overdriveComponent->toBack();
    distortionComponent->toBack();
    
    //update images for the effect button
    buttonProps.states.push_back(ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/ActualProjects/DistortionOverdrivePlugin/Images/toggle-overdr.png")));
    buttonProps.states.push_back(ImageCache::getFromFile(File ("/Users/kathleen/Documents/JuceProjects/ActualProjects/DistortionOverdrivePlugin/Images/toggle-dist.png")));
    
    //set the effect button
    addAndMakeVisible(isOverdriveButton);
    isOverdriveButton.setButtonText("Override / Distortion");
    isOverdriveButton.setImages(false, true, true, buttonProps.states[buttonProps.currentStateIdx], 1.0f, Colours::transparentWhite, EMPTY_IMAGE, 1.0f, Colours::transparentWhite, EMPTY_IMAGE, 1.0f, Colours::transparentWhite);
    isOverdriveButton.onClick = [this] { ChangeTypeOfEffect(); };
    
    //set the dry/wet slider
    addAndMakeVisible(drywetSlider);
    drywetSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    drywetSlider.setRange(0, 100, 1);
    drywetSlider.setValue(50);
    drywetSlider.setTextValueSuffix("%");
    drywetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, true, 40, 20);
    drywetSlider.onValueChange = [this] {processor.SetMixingLevel((float)drywetSlider.getValue() / 100.0f);};
    
    //set dry/wet label (it is not attached to the slider because it must be under the slider)
    addAndMakeVisible(drywetLabel);
    drywetLabel.setFont(Font(14.0f));
    drywetLabel.setText("Dry/Wet", dontSendNotification);
    drywetLabel.setJustificationType(Justification::centredTop);
    
    //default parameters setting
    SetDefaultParameters();
    
    setSize (400, 300);
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
    overdriveComponent->setBounds (gap, gap, width, height);
    distortionComponent->setBounds (gap * 2 + width, gap, width, height);
    
    auto effectTypeButtonShift = getWidth() * 0.2f;
    isOverdriveButton.setBounds (getWidth()/2 - effectTypeButtonShift, gap * 3, effectTypeButtonShift * 2, 40);
    
    auto rotateSliderRadius = getWidth()*0.12f;
    float bottomShift = 0.3f;
    drywetSlider.setBounds(getWidth()/2 - rotateSliderRadius, getHeight() - rotateSliderRadius * (2 + bottomShift), rotateSliderRadius * 2, rotateSliderRadius * 2);
    drywetLabel.setBounds(getWidth()/2 - rotateSliderRadius, getHeight() - rotateSliderRadius * (1.7f * bottomShift), rotateSliderRadius * 2, rotateSliderRadius * (1.7f * bottomShift));
}

void DistortionOverdrivePluginAudioProcessorEditor::ChangeTypeOfEffect()
{
    buttonProps.ChangeStateIdx();
    isOverdriveButton.setImages(false, true, true, buttonProps.states[buttonProps.currentStateIdx], 1.0f, Colours::transparentWhite, EMPTY_IMAGE, 1.0f, Colours::transparentWhite, EMPTY_IMAGE, 1.0f, Colours::transparentWhite);
    isOverdrive = !isOverdrive;
    processor.SetEffect(isOverdrive);
}

void DistortionOverdrivePluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (!overdriveComponent->ExtractSliderName(slider).isEmpty())
        processor.SetParameter(DistortionOverdrivePluginAudioProcessor::Effects::overdrive, 0.5f - (0.49f * slider->getValue() / 100.0f)); // % -> overdrive level
    if (!distortionComponent->ExtractSliderName(slider).isEmpty())
        processor.SetParameter(DistortionOverdrivePluginAudioProcessor::Effects::distortion, 49 * slider->getValue() / 100 + 1); // % -> distortion threshold
}

void DistortionOverdrivePluginAudioProcessorEditor::SetDefaultParameters()
{
    processor.SetMixingLevel((float)drywetSlider.getValue() / 100.0f);
    processor.SetEffect(isOverdrive);
    processor.SetParameter(DistortionOverdrivePluginAudioProcessor::Effects::overdrive, 0.5f - (0.49f * sliderOverdrive[0].defaultValue / 100.0f)); // % -> overdrive level
    processor.SetParameter(DistortionOverdrivePluginAudioProcessor::Effects::distortion, 49 * sliderDistortion[0].defaultValue / 100 + 1); // % -> distortion threshold
}
