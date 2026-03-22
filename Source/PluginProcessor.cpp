/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  : AudioProcessor
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
  return 1;
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
}

void PluginProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginProcessor::updateNoteState(int noteNumber, bool noteOn)
{
  jassert(noteNumber >= 0 && noteNumber < 128);
  int pc = noteNumber % 12;
  int inc = noteOn ? 1 : -1;

  const juce::SpinLock::ScopedLockType lock(pitchClassLock);
  heldNotes[noteNumber] += inc;
  if (heldNotes[noteNumber] < 0)
    heldNotes[noteNumber] = 0;  // clamp: spurious note-offs must not go negative
  pitchClassesPresent[pc] += inc;
  if (pitchClassesPresent[pc] < 0)
    pitchClassesPresent[pc] = 0;  // clamp: spurious note-offs must not go negative
}

void PluginProcessor::clearAllNotes()
{
  const juce::SpinLock::ScopedLockType lock(pitchClassLock);
  heldNotes.fill(0);
  pitchClassesPresent.fill(0);
  midiKeyboardState.reset();
  transportPaused = false;
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  // Detect transport pause — freeze display when DAW stops
  bool isPlaying = false;
  if (auto* playHead = getPlayHead()) {
    auto pos = playHead->getPosition();
    if (pos.hasValue())
      isPlaying = pos->getIsPlaying();
  }
  if (wasPlaying && !isPlaying)
    transportPaused = true;   // just paused: suppress upcoming note-offs
  else if (!wasPlaying && isPlaying) {
    transportPaused = false;  // playing again: clear frozen display
    clearAllNotes();
  }
  wasPlaying = isPlaying;

  bool suppressNoteOff = freezeNotes.load() || transportPaused;

  for (const auto meta : midiMessages) {
    auto m = meta.getMessage();
    if (m.isNoteOn()) {
      midiKeyboardState.processNextMidiEvent(m);
      updateNoteState(m.getNoteNumber(), true);
    } else if (m.isNoteOff()) {
      if (!suppressNoteOff) {
        midiKeyboardState.processNextMidiEvent(m);
        updateNoteState(m.getNoteNumber(), false);
      }
    } else {
      if (suppressNoteOff && m.isController()
          && (m.getControllerNumber() == 123 || m.getControllerNumber() == 120))
        continue;
      midiKeyboardState.processNextMidiEvent(m);
    }
  }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================

bool PluginProcessor::hasEditor() const
{
  return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
  return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
  auto xml = uiState.createXml();
  if (xml != nullptr)
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  auto xml = getXmlFromBinary(data, sizeInBytes);
  if (xml != nullptr && xml->hasTagName(uiState.getType().toString())) {
    uiState = juce::ValueTree::fromXml(*xml);
    stateRestored.store(true);  // tell editor to re-read
  }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new PluginProcessor();
}
