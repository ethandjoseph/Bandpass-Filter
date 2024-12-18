#include "PluginProcessor.h"
#include "PluginEditor.h"

BandpassFilterAudioProcessor::BandpassFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

BandpassFilterAudioProcessor::~BandpassFilterAudioProcessor()
{
}

const juce::String BandpassFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BandpassFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BandpassFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BandpassFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BandpassFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BandpassFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int BandpassFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BandpassFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BandpassFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void BandpassFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void BandpassFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    bandPassFilter.prepare(spec);
    bandPassFilter.reset();
    bandPassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
}

void BandpassFilterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BandpassFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Process Block ==============================================================================
void BandpassFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float frequency = apvts.getRawParameterValue("FREQUENCY")->load();
    float bandwidth = apvts.getRawParameterValue("R")->load();

    bandPassFilter.setCutoffFrequency(frequency);
    bandPassFilter.setResonance(bandwidth);
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);
    bandPassFilter.process(context);
}
//==============================================================================

bool BandpassFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BandpassFilterAudioProcessor::createEditor()
{
    return new BandpassFilterAudioProcessorEditor (*this);
}

void BandpassFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void BandpassFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BandpassFilterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FREQUENCY",               // Parameter ID
        "Centre Frequency",        // Parameter name
        juce::NormalisableRange{
                    20.f,          // rangeStart
                    20000.f,       // rangeEnd
                    0.1f,          // intervalValue
                    0.2f,          // skewFactor
                    false },       // useSymmetricSkew
                    -12.0f         // Default value
                    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "R",                // Parameter ID
        "Resonance",        // Parameter name
        0.707f,             // minValue
        1.000f,             // maxValue
        0.707f              // Default value
    ));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BandpassFilterAudioProcessor();
}