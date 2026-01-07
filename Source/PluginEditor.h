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
typedef std::array<float,12> Chords;
enum ChordNames {
  //CN_Tri_Maj,  CN_Tri_Min,
  CN_Tri_Aug,  CN_Tri_Dim,  CN_Tri_Sus,
  CN_Maj7_Maj, CN_Maj7_Min, CN_Maj7_Aug, CN_Maj7_Dim, CN_Maj7_Sus,
  CN_Dom7_Maj, CN_Dom7_Min, CN_Dom7_Aug, CN_Dom7_Dim, CN_Dom7_Sus,
  CN_Maj6_Maj, CN_Maj6_Min, CN_Maj6_Aug, CN_Maj6_Dim, CN_Maj6_Sus,
  // same as augmented: CN_Min6_Dom, CN_Min6_Min, CN_Min6_Aug, CN_Min6_Dim, CN_Min6_Sus,

  /*  0 0   0 0 0
      1 0 1 0 1 0 0 */  CN_Tri_Maj,

      /*  0 1   0 0 0
          1 0 0 0 1 0 0 */  CN_Tri_Min,

          /* 100010010000 */	CN_Tri_Four_Maj,
          /* 100001000100 */	CN_Tri_Maj_2,	// 2nd inversion of 4
          CN_NUM_CHORDS };

//==============================================================================
/**
 */
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
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
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
  bool pitchClassesNew = true; // triggers initial paint
  long int frameCount = 0;
  // int currentKey = 9; // A
  int currentKey = 0; // C
  juce::StringArray keyNames { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B" };
  /*                            0    1     2    3     4    5    6     7    8     9    10    11    */
  juce::StringArray intervalNames { "1", "m2", "2", "m3", "3", "4", "b5", "5", "#5", "6", "d7", "M7" };
  /*                                 0    1     2    3     4    5    6     7    8     9    10    11    */
  bool currentKeyMinor = true; // Am

  void updateStrings();
  int rotatedPitchClassesToBinary();
  juce::String pitchClassesToIntervalString();
  juce::String pitchClassesToChordString();
  juce::String getCurrentKeyString();
  juce::String getCurrentRootKeyString();

  juce::Label& chordLabel;
  juce::Label& intervalsLabel;
  juce::Label& pitchClassesLabel;
  //  juce::Slider& fontSizeSliderLabel;

  /**
   * Apply rotation corresponding to current key, placing root note at index 0.
   */
  void rotatePitchClassesPresent();

  ChordRecognizer chordRecognizer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
