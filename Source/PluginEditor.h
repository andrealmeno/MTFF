#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyTheme.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class MTFFAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MTFFAudioProcessorEditor (MTFFAudioProcessor&,AudioProcessorValueTreeState&);
    ~MTFFAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    void setUpSlider(Slider& slider, int x, int y, int w, int h);
    
    MTFFAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;
    
    Slider levelSlider;
    Slider lowGainSlider;
    Slider highGainSlider;
    Slider eqFreqSlider;
    Slider eqGainSlider;
    Slider driveSlider;
    Slider bypassButton;

    
    MyLookAndFeel myTheme;
    
    std::unique_ptr<SliderAttachment> levelAttachment;
    std::unique_ptr<SliderAttachment> lowGainAttachment;
    std::unique_ptr<SliderAttachment> highGainAttachment;
    std::unique_ptr<SliderAttachment> eqFreqAttachment;
    std::unique_ptr<SliderAttachment> eqGainAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> bypassAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MTFFAudioProcessorEditor)
};
