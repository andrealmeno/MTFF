#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"


//==============================================================================
MTFFAudioProcessorEditor::MTFFAudioProcessorEditor (MTFFAudioProcessor& p, AudioProcessorValueTreeState& vst)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vst)
{
    setUpSlider(levelSlider, 20, 30, 100, 140);
    setUpSlider(lowGainSlider, 140, 30, 100, 140);
    setUpSlider(highGainSlider, 260, 30, 100, 140);
    setUpSlider(eqFreqSlider, 165, 200, 50, 80);
    setUpSlider(eqGainSlider, 285, 200, 50, 80);
    setUpSlider(driveSlider, 380, 30, 100, 140);
    
  
    bypassButton.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    bypassButton.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(&bypassButton);
    bypassButton.setBounds(150, 480, 200, 80);

    
        // Attachments agli slider
    levelAttachment   = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameLevel, levelSlider);
    lowGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameLowShelvGain, lowGainSlider);
    highGainAttachment= std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameHighShelvGain, highGainSlider);
    eqFreqAttachment  = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameEqFreq, eqFreqSlider);
    eqGainAttachment  = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameEqGain, eqGainSlider);
    driveAttachment   = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameDriveGain, driveSlider);
    bypassAttachment = std::make_unique<SliderAttachment>(valueTreeState,Parameters::nameBypass,bypassButton);
    
    
    this->setLookAndFeel(&myTheme); //sistemare la look and Feel perché l'hai palesemente copiata

    setSize (500, 600);  // w - h
}

MTFFAudioProcessorEditor::~MTFFAudioProcessorEditor()
{
    this->setLookAndFeel(nullptr);
}

void MTFFAudioProcessorEditor::paint (juce::Graphics& g)
{
    //g.setGradientFill(ColourGradient(Colour(0xff505050),270,25,Colour(0xff303030),300,385,false));
    g.fillAll (Colours::black);
    myTheme.drawTwoBoxes(g, *this);
    g.setColour (Colour(0xFFE88131));
    g.setFont (juce::FontOptions (15.0f));
    g.drawText("Level", 20, 180, 100, 20, Justification::centred, true);
    g.drawText("Low Shelv Gain", 140, 180, 100, 20, Justification::centred, true);
    g.drawText("High Shelv Gain", 260, 180, 100, 20, Justification::centred, true);
    g.drawText("EQ Freq", 150, 280, 80, 20, Justification::centred, true);
    g.drawText("EQ Gain", 270, 280, 80, 20, Justification::centred, true);
    g.drawText("Drive", 380, 180, 100, 20, Justification::centred, true);
    g.drawText("Bypass", 150, 480, 200, 20, Justification::centred);

    myTheme.insideTextBox(g);


    
    
}

void MTFFAudioProcessorEditor::resized()
{

}

void MTFFAudioProcessorEditor::setUpSlider(Slider& slider, int x, int y, int w, int h){
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    //slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(&slider);
    slider.setBounds(x, y, w, h);
}
