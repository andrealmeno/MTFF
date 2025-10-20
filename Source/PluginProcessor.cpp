#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"


MTFFAudioProcessor::MTFFAudioProcessor() : parameters(*this, nullptr, "MTFF", Parameters::createParameterLayout())
{
    Parameters::addListenerToAllParameters(parameters, this);
}

MTFFAudioProcessor::~MTFFAudioProcessor()
{
}


void MTFFAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    disto.prepareToPlay(spec,sampleRate);
}

void MTFFAudioProcessor::releaseResources()
{
    disto.releaseResources();
}


void MTFFAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    disto.processBlock(buffer);

}


juce::AudioProcessorEditor* MTFFAudioProcessor::createEditor(){
    return new MTFFAudioProcessorEditor (*this,parameters);
}

void MTFFAudioProcessor::getStateInformation (juce::MemoryBlock& destData){
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MTFFAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}


void MTFFAudioProcessor::parameterChanged(const String& paramID, float newValue){

    disto.parameterChangedDistortion(paramID,newValue);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){
    return new MTFFAudioProcessor();
}
