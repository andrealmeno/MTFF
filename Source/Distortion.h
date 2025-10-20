#pragma once
#include <JuceHeader.h>
#include "Filter.h"

class Distortion {
    public:
    Distortion(bool defaultBypas = false) : bypass(defaultBypas){}
    ~Distortion(){}

    void prepareToPlay(dsp::ProcessSpec& spec, double sampleRate){
        hpf.prepareToPlay(spec);
        
        inputGain.prepare(spec);
        inputGain.setRampDurationSeconds(0.02f);
        inputGain.setGainDecibels(15.0f);
        
        os.prepare(spec);
        dsp::ProcessSpec osSpec = spec;
        const auto osRate = os.getOversampledFrequency();
        const auto factor = osRate / spec.sampleRate;
        osSpec.maximumBlockSize = spec.maximumBlockSize * factor;
        
        bpf.prepareToPlay(osSpec);
        
        lsf.prepareToPlay(osSpec);
        hsf.prepareToPlay(osSpec);
        
        mz.prepareToPlay(osSpec);
        
        eq.prepareToPlay(spec);
        
        level.prepare(spec);
        level.setRampDurationSeconds(0.02f);
        level.setGainLinear(1.0f);
        
    }
    
    void processBlock(AudioBuffer<float>& buffer){
        
        dsp::AudioBlock<float> block(buffer);
        dsp::ProcessContextReplacing<float> context(block);
        
        if (!bypass){
            hpf.processBlock(context);
            
            inputGain.process(context);
            
            auto upBlock = os.saturation(context);
            dsp::ProcessContextReplacing<float> osContext(upBlock);
            
            bpf.processBlock(osContext);
            
            lsf.processBlock(osContext);
            hsf.processBlock(osContext);
            
            mz.processBlock(osContext);
            
            os.downSampling(context);
            
            eq.processBlock(context);
            level.process(context);
        }
    }
    
    void releaseResources(){
        hpf.releaseResources();
        bpf.releaseResources();
        lsf.releaseResources();
        hsf.releaseResources();
        mz.releaseResources();
        os.releaseResources();
        eq.releaseResources();
    }
    
    void setBypass(bool newValue){
        bypass = newValue;
        DBG("PREMUTO");
    }
    
    void parameterChangedDistortion(const String& paramID,float newValue){
        if (paramID == "HSG") {
            hsf.setGain(newValue);
        }

        if (paramID == "LSG") {
            lsf.setGain(newValue);
        }
        if (paramID == "DG"){
            mz.setDrive(newValue);
        }
        if (paramID =="LEVEL"){
            level.setGainDecibels(newValue);
        }
        if (paramID == "EG"){
            eq.setGain(newValue);
        }
        if (paramID == "EF"){
            eq.setFrequency(newValue);
        }
        if (paramID == "BP"){
            setBypass(newValue);
        }
    }
    

    private:
    HighPassFilter hpf;
    OverSampler os;
    BandPassFilter bpf;
    LowShelvFilter lsf;
    HighShelvFilter hsf;
    MetalZone mz;
    EQ eq;

    bool bypass;
    
    dsp::Gain<float> inputGain;
    dsp::Gain<float> level;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Distortion);
};
