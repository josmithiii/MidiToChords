#include "ControlButtons.h"

#include "PluginEditor.h"

ControlButtons::ControlButtons (juce::Rectangle<int> initialBounds, PluginEditor* editorP) : editor(editorP)
{
  addAndMakeVisible (&resetButton);
  resetButton.setTooltip (TRANS("Clear all MIDI state in plugin"));
  resetButton.setButtonText (TRANS("!"));
  resetButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
  resetButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
  resetButton.onClick = [this]() { editor->reset(); };

  // resetButton.onClick = set by PluginEditor.cpp

  // Keep these in sync with setToggleState usage in ./ControlButtons.h
  showKeyboardButton.setToggleState (true, juce::dontSendNotification);
  showPitchClassesButton.setToggleState (true, juce::dontSendNotification);
  showIntervalsButton.setToggleState (true, juce::dontSendNotification);
  showChordButton.setToggleState (true, juce::dontSendNotification);

  addAndMakeVisible (&showKeyboardButton);
  showKeyboardButton.setTooltip (TRANS("Toggle MIDI Keyboard display"));
  showKeyboardButton.setButtonText("K");
  showKeyboardButton.onClick = [this]() { showKeyboard = 1 - showKeyboard; editor->resized(); DBG("showKeyboard set to " << showKeyboard); };

  addAndMakeVisible (&showPitchClassesButton);
  showPitchClassesButton.setTooltip (TRANS("Toggle current-PitchClasses display"));
  showPitchClassesButton.setButtonText("P");
  showPitchClassesButton.onClick = [this]() { showPitchClasses = 1 - showPitchClasses; editor->resized(); DBG("showPitchClasses set to " << showPitchClasses); };

  addAndMakeVisible (&showIntervalsButton);
  showIntervalsButton.setTooltip (TRANS("Toggle played-Intervals display"));
  showIntervalsButton.setButtonText("I");
  showIntervalsButton.onClick = [this]() { showIntervals = 1 - showIntervals; editor->resized(); DBG("showIntervals set to " << showIntervals); };

  addAndMakeVisible (&showChordButton);
  showChordButton.setTooltip (TRANS("Toggle chord display"));
  showChordButton.setButtonText (TRANS("C"));
  showChordButton.onClick = [this]() { showChord = 1 - showChord; editor->resized(); DBG("showChord set to " << showChord); };

  addAndMakeVisible (rootKeyComboBox);
  rootKeyComboBox.setTooltip (TRANS("Root key assumed for chord spellings when reasonable"));
  rootKeyComboBox.setEditableText (false);
  rootKeyComboBox.setJustificationType (juce::Justification::centredLeft);
  rootKeyComboBox.setTextWhenNothingSelected (TRANS("C"));
  rootKeyComboBox.setTextWhenNoChoicesAvailable (TRANS("Root Key"));
  rootKeyComboBox.addItem (TRANS("C"), 1);
  rootKeyComboBox.addItem (TRANS("C#"), 2);
  rootKeyComboBox.addItem (TRANS("D"), 3);
  rootKeyComboBox.addItem (TRANS("Eb"), 4);
  rootKeyComboBox.addItem (TRANS("E"), 5);
  rootKeyComboBox.addItem (TRANS("F"), 6);
  rootKeyComboBox.addItem (TRANS("F#"), 7);
  rootKeyComboBox.addItem (TRANS("G"), 8);
  rootKeyComboBox.addItem (TRANS("Ab"), 9);
  rootKeyComboBox.addItem (TRANS("A"), 10);
  rootKeyComboBox.addItem (TRANS("Bb"), 11);
  rootKeyComboBox.onChange = [this]() { rootKeyComboBoxChanged(); editor->resized(); DBG("Root key set to " << rootKeyComboBox.getText()); };

  addAndMakeVisible (&chordLabel);
  chordLabel.setTooltip (TRANS("Chord Display"));
  chordLabel.setText (TRANS("I am some chord"),
                              juce::dontSendNotification);
  addAndMakeVisible (&chordLabel);
  chordLabel.setTooltip (TRANS("Chord Display"));
  chordLabel.setText (TRANS("I am some chord name of arbitrary size and length"),
                      juce::dontSendNotification);

  addAndMakeVisible (&intervalsLabel);
  intervalsLabel.setTooltip (TRANS("Intervals"));
  intervalsLabel.setText (TRANS("I am some chord spelled in intervals"),
                          juce::dontSendNotification);

  addAndMakeVisible (&pitchClassesLabel);
  pitchClassesLabel.setTooltip (TRANS("PitchClasses"));
  pitchClassesLabel.setText (TRANS("I am some chord indicated in pitchClasses present"),
                                   juce::dontSendNotification);

  fontSize = 24;
  fontSizeSlider.setValue (fontSize);

  addAndMakeVisible (&fontSizeSlider);
  fontSizeSlider.setTooltip (TRANS("Set Font Size"));
  fontSizeSlider.setRange (5, 240, 1);
  fontSizeSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 30, 20);
  fontSizeSlider.onValueChange = [this]() {
    fontSize = fontSizeSlider.getValue();
    editor->resized(); // calls updateStrings
    DBG("Font size set to " << fontSize);
  };

  setBounds(initialBounds); // triggers resized
}

ControlButtons::~ControlButtons()
{
}

//==============================================================================
void ControlButtons::paint (juce::Graphics& g)
{
  g.fillAll (juce::Colour (0xff323e44));
}

void ControlButtons::resized()
{
#if ORIGINAL_GUI == 1
  resetButton.setBoundsRelative(0.1,0.25,0.2,0.2); // (x,y,w,h)
  showChordButton->setBoundsRelative(0.1,0.5,0.2,0.2);
  rootKeyLabel->setBoundsRelative(0.5,0.25,0.2,0.2);
  rootKeyComboBox->setBoundsRelative(0.7,0.25,0.2,0.2);
 \FIXME: need pitch-classes and intervals
#else
  juce::FlexBox fb;
  fb.flexWrap = juce::FlexBox::Wrap::wrap;
  fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
  fb.alignContent = juce::FlexBox::AlignContent::flexStart;
  fb.alignItems = juce::FlexBox::AlignItems::flexStart;
  fb.items.add ( juce::FlexItem(resetButton).withMinWidth (20.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(rootKeyComboBox).withMinWidth (100.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(showPitchClassesButton).withMinWidth (20.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(showIntervalsButton).withMinWidth (20.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(showChordButton).withMinWidth (20.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(showKeyboardButton).withMinWidth (20.0f).withMinHeight (20.0f) );
  fb.items.add ( juce::FlexItem(fontSizeSlider).withMinWidth (100.0f).withMinHeight (20.0f) );
  fb.performLayout (getLocalBounds().toFloat());
#endif
}

void ControlButtons::rootKeyComboBoxChanged ()
{
  DBG("WRITE comboBoxChanged ()");
  // int newKeyIndex = rootKeyComboBox->getSelectedId();
  //!!! rootKeyLambda(rootKeyComboBox->getText());
  editor->setRootKey(rootKeyComboBox.getText());
}
