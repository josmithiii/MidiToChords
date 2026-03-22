#pragma once

#include <JuceHeader.h>

class PluginEditor;

class ControlButtons  : public juce::Component
{
public:

  ControlButtons (juce::Rectangle<int> initialBounds, PluginEditor* editorP);
  ~ControlButtons() override;

  int getShowPitchClasses() { return showPitchClasses; }
  int getShowIntervals() { return showIntervals; }
  int getShowChord() { return showChord; }
  int getShowKeyboard() { return showKeyboard; }
  bool getSustainMeasure() { return sustainMeasure; }
  void paint (juce::Graphics& g) override;
  void resized() override;

  juce::Label&  getChordLabel() { return chordLabel; }
  juce::Label&  getIntervalsLabel() { return intervalsLabel; }
  juce::Label&  getPitchClassesLabel() { return pitchClassesLabel; }
  int getFontSize() { return fontSize; }

  void saveToState(juce::ValueTree& state);
  void loadFromState(const juce::ValueTree& state);

private:
  PluginEditor* editor;

  juce::TextButton resetButton;
  juce::ToggleButton showKeyboardButton;
  juce::ToggleButton showPitchClassesButton;
  juce::ToggleButton showIntervalsButton;
  juce::ToggleButton showChordButton;
  juce::ToggleButton sustainMeasureButton;
  juce::Slider fontSizeSlider;
  int fontSize;

  // Keep these in sync with setToggleState usage in ./ControlButtons.cpp
  // Better yet, GET THESE FROM THE BUTTONS DIRECTLY:
  // bool juce::Button::getToggleState()
  // BUT must convert bool to 0 or 1 and nothing else:
  int showKeyboard = 1;
  int showPitchClasses = 1;
  int showIntervals = 1;
  int showChord = 1;
  bool sustainMeasure = false;   // Sustain notes to end of measure for display

  juce::ComboBox rootKeyComboBox;
  void rootKeyComboBoxChanged();

  juce::Label pitchClassesLabel;
  juce::Label intervalsLabel;
  juce::Label chordLabel;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlButtons)
};
