#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class BandpassFilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BandpassFilterAudioProcessorEditor (BandpassFilterAudioProcessor&);
    ~BandpassFilterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider frequencySlider;
    juce::Slider qKnob;
    juce::Label frequencyLabel;
    juce::Label qLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qKnobAttachment;
    BandpassFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandpassFilterAudioProcessorEditor)
};