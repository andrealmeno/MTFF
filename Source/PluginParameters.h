#pragma once
#include <JuceHeader.h>

namespace Parameters {
    static const String nameLowShelvGain = "LSG";
    static const String nameHighShelvGain = "HSG";
    static const String nameDriveGain = "DG";
    static const String nameLevel = "LEVEL";
    static const String nameEqGain = "EG";
    static const String nameEqFreq = "EF";
    static const String nameBypass = "BP";
    
    
    static const float defaultLowShelvGain = 0.0f;
    static const float defaultHighShelvGain = 0.0f;
    static const float defaultDriveGain = 5.0f;
    static const float defaultLevel = 0.0f;
    static const float defaultEqGain= 0.0f;
    static const float defaultEqFreq= 2600.0f;
    static const float defaultBypass= false;
    
    
    
    
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout(){
        
        std::vector<std::unique_ptr<RangedAudioParameter>> params;
        
        int id = 1;
        
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameLowShelvGain,id++),"Low Shelv Gain (dB)", NormalisableRange<float>(-15.0f,15.0f,0.1f),defaultLowShelvGain));
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameHighShelvGain,id++),"High Shelv Gain (dB)", NormalisableRange<float>(-15.0f,15.0f,0.1f),defaultHighShelvGain));
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameDriveGain,id++),"Drive Gain (dB)", NormalisableRange<float>(1.0f,10.0f,0.1f),defaultDriveGain));
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameLevel,id++),"Level (dB)", NormalisableRange<float>(-60.0f,12.0f,0.1f),defaultLevel));
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameEqGain,id++),"Eq Gain (dB)", NormalisableRange<float>(-15.0f,15.0f,0.1f),defaultEqGain));
        params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameEqFreq,id++),"Eq freq (Hz)", NormalisableRange<float>(200.0f,5000.0f,0.1f),defaultEqFreq));params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameBypass,id++),"Bypass",defaultBypass));
        
        return {params.begin(),params.end()};
        
    }
    
    static void addListenerToAllParameters(AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* lister){
        std::unique_ptr<XmlElement>xml(valueTreeState.copyState().createXml());
        for (auto* elemet : xml->getChildWithTagNameIterator("PARAM")){
            const String& id = elemet->getStringAttribute("id");
            valueTreeState.addParameterListener(id, lister);
        }
    }
}
