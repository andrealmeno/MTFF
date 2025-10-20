#pragma once

#include <JuceHeader.h>
#include "Distortion.h"

class MTFFAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    MTFFAudioProcessor();
    ~MTFFAudioProcessor() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override {return true;}
    const juce::String getName() const override{return JucePlugin_Name;}
    bool acceptsMidi() const override {return false;}
    bool producesMidi() const override {return false;}
    bool isMidiEffect() const override {return false;}
    double getTailLengthSeconds() const override {return 0.0;}
    int getNumPrograms() override {return 1;}
    int getCurrentProgram() override {return 0;}
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override {return {};}
    void changeProgramName (int index, const juce::String& newName) override {}
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    void parameterChanged(const String& paramID,float newValue) override;
    Distortion disto;
    
    AudioProcessorValueTreeState parameters;
    double sampleRate = 0.0;
    float inputGain = 20.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MTFFAudioProcessor)
};
