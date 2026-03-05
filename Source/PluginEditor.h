/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "ControlButtons.h"
#include "ChordRecognizer.h"

typedef std::array<int,12> PitchClasses;

//==============================================================================
class PluginEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
  PluginEditor (PluginProcessor&);
  ~PluginEditor() override;

  //==============================================================================
  void setRootKey(juce::String key);
  void reset();
  //==============================================================================
  void timerCallback() override;
  //==============================================================================
  void paint (juce::Graphics&) override;
  void resized() override;

private:
  juce::CriticalSection resetLock;
  juce::Rectangle<int> guiBounds;
  juce::Rectangle<int> controlButtonBounds;
  ControlButtons controlButtons;
  PluginProcessor& audioProcessor;
  juce::MidiKeyboardComponent midiKeyboardComponent;
  juce::String currentIntervalString = "No Intervals";
  juce::String currentChordString = "No Chord";
  juce::String currentRootKeyString = "C";
  juce::String currentKeyString = "No Key";
  juce::String currentPitchClassesString = "No Pitch Classes";
  juce::String rotatedPitchClassesString = "No Pitch Classes";
  PitchClasses pitchClassesPresent { 0 };
  PitchClasses rotatedPitchClassesPresent { 0 };
  int lowestHeldNote = -1;
  bool pitchClassesNew = true; // triggers initial paint
  long int frameCount = 0;
  int currentKey = 0; // C
  juce::StringArray keyNames { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B" };
  /*                            0    1     2    3     4    5    6     7    8     9    10    11    */
  juce::StringArray intervalNames { "1", "m2", "2", "m3", "3", "4", "b5", "5", "#5", "6", "d7", "M7" };
  /*                                 0    1     2    3     4    5    6     7    8     9    10    11    */

  void updateStrings();
  int rotatedPitchClassesToBinary();
  juce::String pitchClassesToIntervalString();
  juce::String pitchClassesToChordString();
  juce::String getCurrentKeyString();
  juce::String getCurrentRootKeyString();

  juce::Label& chordLabel;
  juce::Label& intervalsLabel;
  juce::Label& pitchClassesLabel;

  void rotatePitchClassesPresent();

  ChordRecognizer chordRecognizer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
