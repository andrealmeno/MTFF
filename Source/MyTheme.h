#pragma once
#include <JuceHeader.h>
#define BORDER_WIDTH 1.5f
#define KNOB_SCALE 0.85f

class MyLookAndFeel : public LookAndFeel_V4{
    public:
    MyLookAndFeel(){}
    ~MyLookAndFeel(){}
    
    const Colour grey = Colour(0xFF212123);
    const Colour grey35Scuro = Colour(0xFF151517);
    const Colour grey75Scuro = Colour(0xFF080809);
    
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override{
           // Useful variables
       const auto radius = jmin(width, height) * 0.5 * KNOB_SCALE - BORDER_WIDTH * 0.5;
       const auto centreX = x + width * 0.5;
       const auto centreY = y + height * 0.5;
       const auto kx = centreX - radius;
       const auto ky = centreY - radius;
       const auto kw = radius * 2.0;
       
       g.fillEllipse(kx, ky, kw, kw);
        g.setColour(Colours::transparentBlack);
       g.drawEllipse(kx, ky, kw, kw, BORDER_WIDTH);
       
           // Draw pointer
       const auto pLen = radius * 0.333;
       const auto pThk = 2.0;
       
       Path p;
       p.addRectangle(pThk * -0.5, -radius, pThk, pLen);
       
       const auto angle = jmap(sliderPosProportional, rotaryStartAngle, rotaryEndAngle);
       p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
       
       g.setGradientFill(ColourGradient(Colours::white, kx, ky, Colours::white.darker(0.2), kx, ky + kw, false));
       
       g.fillPath(p);
       
           // Draw ticks
       Path originalShape, tempPath;
       const auto thikness = 2.0;
       originalShape.addEllipse(thikness * -0.5, radius / -KNOB_SCALE, thikness, thikness);
       g.setColour(Colours::grey);
       
       for (int i = 0; i < numTicks; ++i)
        {
           tempPath = originalShape;
           auto propPos = (float)i / (numTicks - 1);
           propPos = pow(propPos, slider.getSkewFactor());
           auto angle = jmap(propPos, rotaryStartAngle, rotaryEndAngle);
           tempPath.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
           g.fillPath(tempPath);
        }
       
    }
    
    void drawTwoBoxes(Graphics& g, Component& parent){
        int componentWidth = parent.getWidth();
        
        float paddingTop = 10.0f;
        float topBoxWidth = 480.0f;
        float topBoxHeight = 295.0f;
        
        float downBoxWidth = 480.0f;
        float downBoxHeight = 160.0f;
        float paddingInMezzo = 125.0f;
        
        Rectangle<int> topBox(componentWidth / 2 - topBoxWidth / 2,paddingTop,topBoxWidth,topBoxHeight);
        
        g.setColour(grey);
        g.fillRect(topBox);
        g.setColour(Colour(0xFFD0D1DA));
        g.drawRoundedRectangle(topBox.toFloat(), 5.0f, 2.5f);
        
        Rectangle<int> downBox(componentWidth / 2 - downBoxWidth / 2,topBox.getBottom() + paddingInMezzo,downBoxWidth,downBoxHeight);
        
        g.setColour(grey75Scuro);
        g.fillRect(downBox);
        g.setColour(Colour(0xFFD0D1DA));
        g.drawRoundedRectangle(downBox.toFloat(), 5.0f, 2.5f);
        
        
    } //top per i knob e down per bypass
    
    void insideTextBox(Graphics& g){
        
      
        

        
        g.setColour(grey35Scuro);
        g.fillRect(10, 312, 480, 110);
        g.setColour(Colour(0xFFD0D1DA));
        g.drawRoundedRectangle(10, 312, 480, 110,5,2.5f);
        g.setColour (Colour(0xFFE88131));
        //g.setFont(FontOptions(50.0f));
        g.setFont(FontOptions("Ethnocentric", 50.0f, juce::Font::bold));
        g.drawText("METAL FOFFONE",0, 313, 500, 100, Justification::centred);
        g.setFont(FontOptions("Ethnocentric",30.0f,Font::bold));
        g.drawText("MT-FF",378, 348, 400, 100, Justification::left);
    }
    
    void setNumThicks(int newValue){
    numTicks = newValue;
}
    
    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,float sliderPos, float minSliderPos, float maxSliderPos,const Slider::SliderStyle style, Slider& slider) override {
       auto trackHeight = 8.0f;
       auto thumbRadius = 16.0f;
       
           // Track
       g.setColour(Colours::darkgrey);
       g.fillRoundedRectangle((float)x, y + height * 0.5f - trackHeight * 0.5f,
                              (float)width, trackHeight, 2.0f);
       
           // Active part
       g.setColour(Colours::orange);
       g.fillRoundedRectangle((float)x, y + height * 0.5f - trackHeight * 0.5f,
                              sliderPos - (float)x, trackHeight, 2.0f);
       
           // Thumb
       g.setColour(Colours::white);
       g.fillEllipse(sliderPos - thumbRadius * 0.5f,
                     y + height * 0.5f - thumbRadius * 0.5f,
                     thumbRadius, thumbRadius);
    }
    
    private:
    
    int numTicks = 21;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyLookAndFeel)
};
