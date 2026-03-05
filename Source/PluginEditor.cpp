/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p) // xtor
  : AudioProcessorEditor (&p)
  , guiBounds(0,0,400,150)
  , controlButtonBounds(0,0,400,40)
  , controlButtons(controlButtonBounds, this)
  , audioProcessor (p)
  , midiKeyboardComponent(audioProcessor.getMidiKeyboardState(),
                          juce::KeyboardComponentBase::horizontalKeyboard)
  , chordLabel(controlButtons.getChordLabel())
  , intervalsLabel(controlButtons.getIntervalsLabel())
  , pitchClassesLabel(controlButtons.getPitchClassesLabel())
{
  setSize (200, 150);

  pitchClassesLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), 60.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
  pitchClassesLabel.setJustificationType (juce::Justification::centred);
  pitchClassesLabel.setEditable (false, false, false);
  pitchClassesLabel.setColour (juce::TextEditor::textColourId, juce::Colours::black);
  pitchClassesLabel.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

  intervalsLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), 60.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
  intervalsLabel.setJustificationType (juce::Justification::centred);
  intervalsLabel.setEditable (false, false, false);
  intervalsLabel.setColour (juce::TextEditor::textColourId, juce::Colours::black);
  intervalsLabel.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

  chordLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), 120.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
  chordLabel.setJustificationType (juce::Justification::centred);
  chordLabel.setEditable (false, false, false);
  chordLabel.setColour (juce::TextEditor::textColourId, juce::Colours::black);
  chordLabel.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

  addAndMakeVisible(controlButtons);
  addAndMakeVisible(chordLabel);
  addAndMakeVisible(intervalsLabel);
  addAndMakeVisible(pitchClassesLabel);
  midiKeyboardComponent.setLowestVisibleKey(/* "C0" really C2 */ 24);
  addAndMakeVisible(midiKeyboardComponent);
  setResizable(/* allowHostToResize */ true, /* useBottomRightCornerResizer */ true);

  juce::Rectangle<int> r = juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
  int w = r.getWidth() - 30;
  int h = r.getHeight() - 50;
  setResizeLimits (100, 75, w, h);
  resized();
  startTimerHz (/* frameRateHz */ 10);
}

PluginEditor::~PluginEditor()
{
}

static int arrayCountNonzero(PitchClasses a) {
  int count = 0;
  for (auto ai : a)
    if (ai != 0)
      count++;
  return count;
}

static int arrayIndexOfFirstNonzero(PitchClasses a) {
  for (size_t i = 0; i < a.size(); i++)
    if (a[i] != 0)
      return static_cast<int>(i);
  return -1;
}

static int arrayIndexOfNthNonzero(PitchClasses a, int n) {
  for (size_t i = 0; i < a.size(); i++) {
    if (a[i] != 0) {
      n--;
      if (n <= 0)
        return static_cast<int>(i);
    }
  }
  return -1;
}

static void arrayClip(PitchClasses& a, int minVal, int maxVal) {
  for (size_t n = 0; n < a.size(); n++) {
    if (a[n] > maxVal)
      a[n] = maxVal;
    else if (a[n] < minVal)
      a[n] = minVal;
  }
}

static juce::String arrayToString(PitchClasses a) {
  juce::String str = "[";
  for (size_t n = 0; n < a.size(); n++) {
    str += juce::String(a[n]);
    if (n < a.size() - 1)
      str += juce::String(", ");
  }
  str += juce::String("]");
  return str;
}

void PluginEditor::rotatePitchClassesPresent() {
  for (int i = 0; i < 12; i++) {
    int iDest = i - currentKey;
    if (iDest < 0)
      iDest += 12;
    jassert(iDest >= 0 && iDest < 12);
    rotatedPitchClassesPresent[iDest] = pitchClassesPresent[i];
  }
}

juce::String PluginEditor::getCurrentRootKeyString() {
  return keyNames[currentKey];
}

juce::String PluginEditor::getCurrentKeyString() {
  return getCurrentRootKeyString();
}

//==============================================================================

void PluginEditor::setRootKey(juce::String key) {
  bool found = false;
  for (int i = 0; i < keyNames.size(); i++) {
    if (key == keyNames[i]) {
      currentKey = i;
      found = true;
      DBG("Root key set to " << currentKey);
    }
  }
  jassert(found);
}

void PluginEditor::reset() {
  const juce::ScopedLock myScopedLock (resetLock);
  audioProcessor.getMidiKeyboardState().reset();
  pitchClassesPresent.fill(0);
  rotatedPitchClassesPresent.fill(0);
  lowestHeldNote = -1;
  pitchClassesNew = true;
  frameCount = 0;
}

//==============================================================================
void PluginEditor::timerCallback()
{
  const juce::GenericScopedTryLock<juce::CriticalSection> myScopedTryLock (resetLock);
  if (myScopedTryLock.isLocked()) {
    PitchClasses newPitchClassesPresent = audioProcessor.getPitchClassesPresent();
    int newLowestNote = audioProcessor.getLowestHeldNote();
    arrayClip(newPitchClassesPresent, 0, 1);
    if (newPitchClassesPresent != pitchClassesPresent || newLowestNote != lowestHeldNote) {
      pitchClassesPresent = newPitchClassesPresent;
      lowestHeldNote = newLowestNote;
      rotatePitchClassesPresent();
      pitchClassesNew = true;
      updateStrings();
      repaint();
    }
    frameCount++;
  } else {
    DBG("Timer callback skipped because we must be resetting");
  }
}

int PluginEditor::rotatedPitchClassesToBinary() {
  int binary = 0;
  for (size_t i = 0; i < rotatedPitchClassesPresent.size(); i++) {
    binary <<= 1;
    binary += rotatedPitchClassesPresent[i];
  }
  return binary;
}

juce::String PluginEditor::pitchClassesToIntervalString() {
  juce::String intervalString = "";
  int N = arrayCountNonzero(pitchClassesPresent);
  int inz = arrayIndexOfFirstNonzero(rotatedPitchClassesPresent);
  if (inz < 0) {
    intervalString = "---";
  } else {
    intervalString = intervalNames[inz];
    for (int i = 1; i < N; i++) {
      inz = arrayIndexOfNthNonzero(rotatedPitchClassesPresent, i + 1);
      if (inz < 0)
        break;
      intervalString += ", " + intervalNames[inz];
    }
  }
  return intervalString;
}

juce::String PluginEditor::pitchClassesToChordString() {
  ChordRecognizer::ChordResult result = chordRecognizer.identify(pitchClassesPresent, lowestHeldNote);
  return result.fullName;
}

void PluginEditor::updateStrings () {

  if (pitchClassesNew) {
    currentPitchClassesString = arrayToString(pitchClassesPresent);
    rotatedPitchClassesString = arrayToString(rotatedPitchClassesPresent);
    currentIntervalString = pitchClassesToIntervalString();
    currentChordString = pitchClassesToChordString();
    pitchClassesNew = false;
  }

  int fontSize = controlButtons.getFontSize();
  int chordFontSize = 2 * fontSize;
  int lineSep = 2;
  int y = -fontSize * 5 / 2;
  static const juce::String pitchClassesIntro = juce::String("Pitch Classes Present: ");
  y += fontSize + lineSep;
  y += fontSize + lineSep;
  static const juce::String rotatedPitchClassesIntro = juce::String("Pitch Classes Present, from Root Key:\n ");
  pitchClassesLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), fontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  pitchClassesLabel.setText(TRANS(rotatedPitchClassesIntro + rotatedPitchClassesString), juce::dontSendNotification);
  y += fontSize + lineSep;
  static const juce::String rootKeyStringIntro = juce::String("For root key ");
  currentRootKeyString = getCurrentRootKeyString();
  y += fontSize + lineSep;
  int chordSize = arrayCountNonzero(pitchClassesPresent);
  auto chordSizeString = juce::String(chordSize);
  auto intervalStringIntro = juce::String("the current ") + chordSizeString + juce::String("-note chord has intervals\n");
  y += (fontSize + chordFontSize) / 2 + lineSep;

  intervalsLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), fontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  intervalsLabel.setText(TRANS(rootKeyStringIntro + currentRootKeyString + "\n" + intervalStringIntro + currentIntervalString),
                        juce::dontSendNotification);

  y += chordFontSize + lineSep;
  chordLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), chordFontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  chordLabel.setText(TRANS(currentChordString), juce::dontSendNotification);
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
  g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
  juce::Grid grid;
  using Track = juce::Grid::TrackInfo;
  using Fr = juce::Grid::Fr;
  grid.templateColumns = { Track (Fr (1)) };
  grid.templateRows = {   Track (Fr (1))
                        , Track (Fr (2 * controlButtons.getShowPitchClasses()))
                        , Track (Fr (2 * controlButtons.getShowIntervals()))
                        , Track (Fr (2 * controlButtons.getShowChord()))
                        , Track (Fr (2 * controlButtons.getShowKeyboard()))
  };
  updateStrings();
  grid.items = {
      juce::GridItem (controlButtons)
    , juce::GridItem (pitchClassesLabel)
    , juce::GridItem (intervalsLabel)
    , juce::GridItem (chordLabel)
    , juce::GridItem (midiKeyboardComponent)
  };
  grid.performLayout (getLocalBounds());
}
