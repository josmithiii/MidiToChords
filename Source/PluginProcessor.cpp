/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#if USE_PGM == 0
#include "PluginEditor.h"
#endif

//==============================================================================
PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  :
#if USE_PGM == 1
  MagicProcessor
#else
  AudioProcessor
#endif
  (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
   .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
   )
#endif
{
#if USE_PGM == 1
  magicState.setGuiValueTree (BinaryData::MidiChords_xml, BinaryData::MidiChords_xmlSize);
#endif
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
  return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int PluginProcessor::getNumPrograms()
{
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
  // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
  return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
  return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
}

void PluginProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused (layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
      && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void PluginProcessor:: updatePitchClassesPresent(int keyNum)
{
  int inc = 1;
  if (keyNum < 0) {
    keyNum = - keyNum;
    inc = -1;
  }
  while (keyNum >= 12) {
    keyNum -= 12;
  }
  pitchClassesPresent.at(keyNum) += inc;
  jassert(pitchClassesPresent.at(keyNum) >= 0);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  // PROCESS MIDI DATA

  //PM: juce::MidiBuffer processedMidi;
  int time;
  juce::MidiMessage m;
  juce::MidiMessage pitchbendMessage;
  float pitchbendRangeInSemitones = 2.0f; // THIS HAS TO AGREE WITH YOUR YOUR SYNTH ENGINE
  for (const auto meta : midiMessages) {
    m = meta.getMessage();
    midiKeyboardState.processNextMidiEvent(m);
    time = static_cast<int>(m.getTimeStamp());
    float centsSharp = 150.0f; // EXAMPLE: Shift pitch up one semitone + one quarter tone
    if (m.isNoteOn()) {
      int noteNumber = m.getNoteNumber();
      updatePitchClassesPresent(noteNumber);
      float noteFreqHz = m.getMidiNoteInHertz(noteNumber);
      // Implementation: return frequencyOfA * std::pow (2.0, (noteNumber - 69) / 12.0);
      float newFreqHz = noteFreqHz * powf(2.0f, centsSharp/1200.0f);
      // Where is the function for converting frequency in Hz to integer note number + pitchBend?:
      float newNoteNumberFloat = 69.0f + 12.0f * std::log2(newFreqHz/440.0f);
      float newNoteNumberFloor = floorf(newNoteNumberFloat);
      float pitchBendNN = newNoteNumberFloat - newNoteNumberFloor;
      if (fabsf(pitchBendNN) > 3.0f/1200.0f) { // 3 cents is a pretty reasonable difference-limen for fundamental frequency
        // Send pitchBend message
        juce::uint16 pitchWheelPosition = juce::MidiMessage::pitchbendToPitchwheelPos (pitchBendNN, pitchbendRangeInSemitones);
        pitchbendMessage = juce::MidiMessage::pitchWheel(m.getChannel(), pitchWheelPosition);
        //PM: processedMidi.addEvent (pitchbendMessage, time);
      }
      //PM: m = juce::MidiMessage::noteOn(m.getChannel(), int(newNoteNumberFloor), m.getVelocity()); // addEvent below
    } else if (m.isNoteOff()) {
      // Cancel pitchbend?
      updatePitchClassesPresent(-m.getNoteNumber());
    } else if (m.isAftertouch()) {
      // Do something with aftertouch?
    } else if (m.isPitchWheel()) {
      // You could save the last note number and add pitchbend to that and convert according to your transformation
    }
    //PM: processedMidi.addEvent (m, time);
  } // loop over MIDI messages
  //PM: midiMessages.swapWith (processedMidi);
  //PM: dumpMidiMessages(midiMessages, "Outgoing MIDI");
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if USE_PGM == 0

//==============================================================================

bool PluginProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
  return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
}

#endif

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new PluginProcessor();
}

