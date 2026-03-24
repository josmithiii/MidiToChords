/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class PluginProcessor  : public juce::AudioProcessor
{
public:
  //==============================================================================
  PluginProcessor();
  ~PluginProcessor() override;

  //==============================================================================
  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;
  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  //==============================================================================
  std::array<int,12> getPitchClassesPresent() {
    const juce::SpinLock::ScopedLockType lock(pitchClassLock);
    return pitchClassesPresent;
  }

  /// Returns the lowest MIDI note number currently held, or -1 if none.
  int getLowestHeldNote() {
    const juce::SpinLock::ScopedLockType lock(pitchClassLock);
    for (int i = 0; i < 128; i++)
      if (heldNotes[i] > 0)
        return i;
    return -1;
  }

  juce::MidiKeyboardState& getMidiKeyboardState() {
    return midiKeyboardState;
  }

  /// Historical note range (min/max MIDI note ever seen since last clear).
  std::pair<int,int> getNoteRange() {
    return { minNoteEver.load(), maxNoteEver.load() };
  }

  /// When true, note-offs are ignored for display — notes accumulate until cleared.
  std::atomic<bool> freezeNotes { false };

  /// Clear all internal note tracking state (called by editor reset).
  void clearAllNotes();

  /// Persistent UI state — survives DAW save/load
  juce::ValueTree uiState { "MidiToChordsState" };

  /// Set by setStateInformation so the editor can re-read restored state.
  std::atomic<bool> stateRestored { false };

private:

  juce::SpinLock pitchClassLock;
  std::array<int,12> pitchClassesPresent { 0 };
  std::array<int,128> heldNotes { 0 };

  void updateNoteState(int noteNumber, bool noteOn);
  juce::MidiKeyboardState midiKeyboardState;
  std::atomic<int> minNoteEver { -1 };
  std::atomic<int> maxNoteEver { -1 };

  /// True when the DAW transport was playing in the previous processBlock call.
  bool wasPlaying { false };
  /// True after transport stops — suppresses note-offs until transport resumes.
  bool transportPaused { false };
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
