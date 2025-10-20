#pragma once
#include <JuceHeader.h>

using filtro = dsp::IIR::Filter<float>;
using coeffFiltro = dsp::IIR::Coefficients<float>;


#define TARGET_SAMPLING_RATE 192000.0


class Filters {
    public:
    Filters() {}
    virtual ~Filters() {}
    virtual void prepareToPlay(dsp::ProcessSpec& spec) {
        filterChain.prepare(spec);
    }
    
    virtual void processBlock(dsp::ProcessContextReplacing<float>& context) {
        filterChain.process(context);
    }
    
    virtual void releaseResources() {
        filterChain.reset();
    }
    
    protected:

    dsp::ProcessorChain<filtro, filtro> filterChain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filters)
};

class HighPassFilter : public Filters {
    public:
    HighPassFilter(){}
    
    void prepareToPlay(dsp::ProcessSpec& spec) override{
        filterChain.prepare(spec);
        auto coef = coeffFiltro::makeHighPass(spec.sampleRate, 106.0f);
        filterChain.get<0>().coefficients = coef;
        filterChain.get<1>().coefficients = coef;
    }
    
}; // attenuo circa di 20dB, molto più di quanto richiesto ovvero 6dB



class OverSampler{
    public:
    OverSampler(){}
    ~OverSampler(){}
    
    void prepare(const dsp::ProcessSpec& spec) noexcept
    {
       specs = spec;
       
       oversamplingFactor = jmax(roundToInt(TARGET_SAMPLING_RATE / specs.sampleRate), 1);
       overSampledRate = specs.sampleRate * oversamplingFactor;
       
       oversampler.reset(new dsp::Oversampling<float>(
                                                      specs.numChannels,
                                                      log2(oversamplingFactor),
                                                      dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple,
                                                      true,
                                                      true));
       
       oversampler->initProcessing(specs.maximumBlockSize);
       oversampler->reset();
    }
    
    auto upSampling(dsp::ProcessContextReplacing<float>& context){
        auto&& inBlock = context.getInputBlock();
        return oversampler->processSamplesUp(inBlock);
        
    }
    
    auto downSampling(dsp::ProcessContextReplacing<float>& context){
        auto&& outBlock = context.getOutputBlock();
        return oversampler->processSamplesDown(outBlock);
    }
    
    auto saturation(dsp::ProcessContextReplacing<float>& context) noexcept //ritorna un audioblock
    
    {
       auto oversampledBlock = upSampling(context);
       auto len = oversampledBlock.getNumSamples();
       auto numChannels = oversampledBlock.getNumChannels();
       for (int ch = 0; ch < numChannels; ++ch)
        {
           auto* data = oversampledBlock.getChannelPointer(ch);
           
           for (size_t i = 0; i < len; ++i)
               data[i] = tanh(gain * data[i]) * invAtanGain;
        }
       return oversampledBlock;
    }
    
    int getlatency() const noexcept
    {
       return roundToInt(oversampler->getLatencyInSamples());
    }
    
    double getOversampledFrequency(){
        return overSampledRate;
    }
    
    void setGain(float newValue){
        gain = newValue;
        jassert(gain != 0.0f);
        invAtanGain = 1.0f / atan(gain);
    }
    
    void releaseResources(){
        oversampler->reset();
    }
    
    private:
    double overSampledRate = 0;
    dsp::ProcessSpec specs{0,0,0};
    std::unique_ptr<dsp::Oversampling<float>> oversampler;
    int oversamplingFactor = 1;
    int latency = 0;
    
    float gain = 15.0f;
    float invAtanGain = 1.0f / atan(1.0f);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverSampler)
        //    finire di scrivere le tre classi up down e saturation e dichiarare i membri privati della classe
};

class BandPassFilter : public Filters{
    public:
    BandPassFilter(){}
    ~BandPassFilter(){}
    
    void prepareToPlay(dsp::ProcessSpec& spec) override{
        filterChain.prepare(spec);
        auto coef = coeffFiltro::makeBandPass(spec.sampleRate, 1200.0f,1.0f);
        filterChain.get<0>().coefficients = coef;
    }
    
    
};



class LowShelvFilter : public Filters {
    public:
    LowShelvFilter(float defaultLowGain = 0.0f)
    : lowGain(defaultLowGain)
    {
       lowSmoothGain.setCurrentAndTargetValue(lowGain);
    }
    
    void prepareToPlay(dsp::ProcessSpec& spec) override
    {
       sampleRate = spec.sampleRate;
       filterChain.prepare(spec);
       
       updateCoefficients(Decibels::decibelsToGain(lowGain));
       
       lowSmoothGain.reset(sampleRate, 0.0001f);
       
    }
    
    void processBlock(dsp::ProcessContextReplacing<float>& context) override
    {
       if(lowSmoothGain.isSmoothing()){
           float currentGain = Decibels::decibelsToGain(lowSmoothGain.getNextValue());
           updateCoefficients(currentGain);
       }
       filterChain.process(context);
       
    }
    
    void setGain(float newValue)
    {
       lowGain = newValue;
       lowSmoothGain.setTargetValue(lowGain);
    }
    
    private:
    
    void updateCoefficients(float gain) {
        auto coeff = coeffFiltro::makeLowShelf(sampleRate, freq, q, gain);
        
        filterChain.get<0>().coefficients = coeff;
        filterChain.get<1>().coefficients = coeff;
    }
    
    float lowGain = 0.0f;
    double sampleRate = 0.0f;
    float freq = 200.0f;
    float q = 0.7f;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> lowSmoothGain;
};

class HighShelvFilter : public Filters {
    public:
    HighShelvFilter(float defaultHighGain = 0.0f) : highGain(defaultHighGain){}
    ~HighShelvFilter(){
        highSmoothGain.setCurrentAndTargetValue(highGain);
    }
    
    void prepareToPlay(dsp::ProcessSpec& spec) override{
        sampleRate = spec.sampleRate;
        filterChain.prepare(spec);
        
        updateCoefficients(Decibels::decibelsToGain(highGain));
        
        highSmoothGain.reset(sampleRate, 0.0001f);

    }
    
    void processBlock(dsp::ProcessContextReplacing<float>& context) override {
        
        if (highSmoothGain.isSmoothing()){
            float currentGain = Decibels::decibelsToGain(highSmoothGain.getNextValue());
            updateCoefficients(currentGain);
        }
        filterChain.process(context);
    }
    
    void setGain(float newValue){
        highGain = newValue;
        highSmoothGain.setTargetValue(highGain);
    }
    
    private:
    
    void updateCoefficients(float gain){
        auto coeff = coeffFiltro::makeHighShelf(sampleRate, freq, q, gain);
        
        filterChain.get<0>().coefficients = coeff;
        filterChain.get<1>().coefficients = coeff;
    }
    
    float highGain = 0.0f;
    double sampleRate = 0.0;
    float freq = 2000.0f;
    float q = 0.7f;
    
    SmoothedValue<float,ValueSmoothingTypes::Linear> highSmoothGain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighShelvFilter);
};

class MetalZone {
    public:
    MetalZone() {
        driveSmoothed.setCurrentAndTargetValue(drive);
    }
    ~MetalZone() {}
    
    void prepareToPlay(dsp::ProcessSpec& spec) {
        sampleRate = spec.sampleRate;
        driveSmoothed.reset(sampleRate, 0.01f);
        
    }
    
        // DSP principale
    void processBlock(dsp::ProcessContextReplacing<float>& context) {
        auto& block = context.getOutputBlock();
        auto numChannels = block.getNumChannels();
        auto numSamples  = block.getNumSamples();
        
            // Soglia fissa di hard qclipping (simula conduzione diodi)
        constexpr float clipThreshold = 1.2f;
        
        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = block.getChannelPointer(ch);
            
            for (int i = 0; i < numSamples; ++i) {
                float g = driveSmoothed.getNextValue();
                
                float x = data[i] * g;
                
                
                if (x > clipThreshold) //Hard clipping: taglia a ±clipThreshold
                    x = clipThreshold;
                else if (x < -clipThreshold)
                    x = -clipThreshold;
                data[i] = x;
            }
        }
    }
    
    void releaseResources() {
        driveSmoothed.reset(sampleRate, 0.01f);
    }
    
    void setDrive(float newValue) {
        drive = newValue;
        driveSmoothed.setTargetValue(drive);
    }
    
    private:
    double sampleRate = 0.0;
    float drive = 2.0f; // valore iniziale del drive
    SmoothedValue<float, ValueSmoothingTypes::Linear> driveSmoothed;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetalZone)
};

class EQ : public Filters {
    public:
    EQ(float defaultEqGain = 0.0f, float defaultEqFreq = 2600.0f) : gain(defaultEqGain), freq(defaultEqFreq) {
        eqSmoothedGain.setCurrentAndTargetValue(gain);
        eqSmoothedFreq.setCurrentAndTargetValue(freq);
    }
    ~EQ(){}
    
    void prepareToPlay(dsp::ProcessSpec& spec) override{
        sampleRate = spec.sampleRate;
        
        filterChain.prepare(spec);

        updateCoefficients(freq, Decibels::decibelsToGain(gain));
        
        eqSmoothedGain.reset(sampleRate,0.0001f);
        eqSmoothedFreq.reset(sampleRate,0.0001f);
 
    }
    
    void processBlock(dsp::ProcessContextReplacing<float>& context) override{
        float currentGain = Decibels::decibelsToGain(eqSmoothedGain.getNextValue());
        float currentFreq = eqSmoothedFreq.getNextValue();
        
        updateCoefficients(currentFreq, currentGain);
        
        filterChain.process(context);
    }
    
    void setGain(float newValue){
        gain = newValue;
        eqSmoothedGain.setTargetValue(gain);
    }
    
    void setFrequency(float newValue){
        freq = newValue;
        eqSmoothedFreq.setTargetValue(freq);
    }
    
    private:
    
    void updateCoefficients(float frequency, float gain){
        auto coeff = coeffFiltro::makePeakFilter(sampleRate, frequency, q, gain);
        filterChain.get<0>().coefficients = coeff;
        filterChain.get<1>().coefficients = coeff;
    }
    
    double sampleRate = 0.0f;
    float gain = 0.0f;
    float freq = 2600.0f; //range da 200 a 5k Hz
    float q = 1.2f;
    
    SmoothedValue<float,ValueSmoothingTypes::Linear> eqSmoothedGain;
    SmoothedValue<float,ValueSmoothingTypes::Linear> eqSmoothedFreq;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQ)
};
