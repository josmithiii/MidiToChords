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
    // , fontSizeSliderLabel(controlButtons.getFontSizeSlider())
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

  // midiKeyboardComponent.setLowestVisibleKey(/* "C2" really C4 */ 48);
  addAndMakeVisible(controlButtons); // Control bar along the top
  // addAndMakeVisible(controlButtons.getOriginalDisplay()); // Let it do everything (not adjustable)
  addAndMakeVisible(chordLabel);
  // enough: addAndMakeVisible(chordLabel2);
  addAndMakeVisible(intervalsLabel);
  addAndMakeVisible(pitchClassesLabel);
  // addAndMakeVisible(fontSizeSliderLabel);
  midiKeyboardComponent.setLowestVisibleKey(/* "C0" really C2 */ 24);
  addAndMakeVisible(midiKeyboardComponent);
  setResizable(/* allowHostToResize */ true, /* useBottomRightCornerResizer */ true);

  juce::Rectangle<int> r = juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
  int w = r.getWidth() - 30;
  int h = r.getHeight() - 50;
  setResizeLimits (100, 75, w, h); // FIXME: make min size larger for any product
  resized(); // needed to get keyboard where it goes
  startTimerHz (/* frameRateHz */ 10); // audio visualizer update rate
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
  for (int i=0; i<a.size(); i++)
    if (a[i] != 0)
      return i;
  return -1;
}

/**
 * Return 0-based index of n'th nonzero element, where n is 1-based.
 * Output is -1 if less than n nonzero elements were found.
 */
static int arrayIndexOfNthNonzero(PitchClasses a, int n) {
  for (int i=0; i<a.size(); i++) {
    if (a[i] != 0) {
      n--;
      if (n <= 0)
        return i;
    }
  }
  return -1;
}

static void arrayClip(PitchClasses& a, int minVal, int maxVal) {
  int N = int(a.size());
  for (int n=0; n<N; n++) {
    if (a[n] > maxVal) {
      a[n] = maxVal;
    } else if (a[n] < minVal) {
      a[n] = minVal;
    }
  }
}

// not this easy: template<typename T, typename N>
static bool arraysEqual(PitchClasses a, PitchClasses b) {
  int N = a.size();
  jassert(N == b.size());
  bool equal = true;
  for (int n=0; n<N; n++) {
    if (a[n] != b[n]) {
      equal = false;
      break;
    }
  }
  return equal;
}

static juce::String arrayToString(PitchClasses a) {
  int N = a.size();
  juce::String str = "[";
  for (int n=0; n<N; n++) {
    str += juce::String(a[n]);
    if (n < N-1) {
      str += juce::String(", ");
    }
  }
  str += juce::String("]");
  return str;
}

void PluginEditor::rotatePitchClassesPresent() {
  for (int i=0; i<12; i++) {
    int iDest = i - currentKey;
    if (iDest < 0) {
      iDest += 12;
    }
    jassert(iDest >= 0 && iDest < 12);
    rotatedPitchClassesPresent[iDest] = pitchClassesPresent[i];
    // findClosestChord();

  }
}

juce::String PluginEditor::getCurrentRootKeyString() {
  juce::String rootKey = keyNames[currentKey];
  return rootKey;
}

juce::String PluginEditor::getCurrentKeyString() {
  juce::String ks = getCurrentRootKeyString();
  if (currentKeyMinor) {
    ks += juce::String("m");
  }
  return ks;
}

//==============================================================================

void PluginEditor::setRootKey(juce::String key) {
  bool found = false;
  for (int i=0; i<keyNames.size(); i++) {
    auto keyName = keyNames[i];
    if (key == keyName) {
      currentKey = i;  // FIXME: Change currentKey to currentRootKey or just rootKeyIndex;
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
  pitchClassesNew = true; // Need to paint this
  frameCount = 0;
}

//==============================================================================
void PluginEditor::timerCallback()
{
  const juce::GenericScopedTryLock<juce::CriticalSection> myScopedTryLock (resetLock);
  if (myScopedTryLock.isLocked()) {
    PitchClasses newPitchClassesPresent = audioProcessor.getPitchClassesPresent();
    arrayClip(newPitchClassesPresent,0,1); // don't care about how many right now
    // +  " and width is " + juce::String(getWidth());
    if (not arraysEqual(newPitchClassesPresent, pitchClassesPresent)) {
      pitchClassesPresent = newPitchClassesPresent;
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

/**
 * Returns 12-bit binary representation of pitch-classes in use.
 */
int PluginEditor::rotatedPitchClassesToBinary() {
  int binary = 0;
  for (int i=0; i<rotatedPitchClassesPresent.size(); i++) {
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
    for (int i=1; i<N; i++) {
      inz = arrayIndexOfNthNonzero(rotatedPitchClassesPresent,i+1);
      if (inz < 0)
        break;
      intervalString += ", " + intervalNames[inz];
    }
  }
  return intervalString;
}

juce::String PluginEditor::pitchClassesToChordString() {
  // Use the comprehensive ChordRecognizer
  ChordRecognizer::ChordResult result = chordRecognizer.identify(pitchClassesPresent);
  return result.fullName;
}

void PluginEditor::updateStrings () {

  // midiKeyboardComponent.paint(g);
  // g.setColour (juce::Colours::white);

  if (pitchClassesNew) {
    currentPitchClassesString = arrayToString(pitchClassesPresent);
    rotatedPitchClassesString = arrayToString(rotatedPitchClassesPresent);
    currentIntervalString = pitchClassesToIntervalString();
    currentChordString = pitchClassesToChordString();
    pitchClassesNew = false; // slight optimization
  }

  int fontSize = controlButtons.getFontSize();
  int chordFontSize = 2*fontSize;
  int lineSep = 2;
  int w = getWidth();
  int h = getHeight();
  int x = 0;
  int numLines = 5;
  int y = -fontSize * numLines/2; // 1st line shifts up by half the number of lines
  //g.drawFittedText (currentChord, getLocalBounds(), juce::Justification::centred, 1);
  static const juce::String pitchClassesIntro = juce::String("Pitch Classes Present: ");
  y += fontSize + lineSep;
  // g.drawFittedText (pitchClassesIntro + currentPitchClassesString, x, y, w, h, juce::Justification::centred, 1);
  y += fontSize + lineSep;
  static const juce::String rotatedPitchClassesIntro = juce::String("Pitch Classes Present, from Root Key:\n ");
  // g.drawFittedText (rotatedPitchClassesIntro + rotatedPitchClassesString, x, y, w, h, juce::Justification::centred, 1);
  pitchClassesLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), fontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  pitchClassesLabel.setText(TRANS(rotatedPitchClassesIntro + rotatedPitchClassesString), juce::dontSendNotification);
  y += fontSize + lineSep;
  static const juce::String rootKeyStringIntro = juce::String("For root key ");
  juce::String currentKeyString = "No Key Estimated";
  currentRootKeyString = getCurrentRootKeyString();
  // g.drawFittedText (rootKeyStringIntro + currentRootKeyString, x, y, w, h, juce::Justification::centred, 1);
  y += fontSize + lineSep;
  int chordSize = arrayCountNonzero(pitchClassesPresent);
  auto chordSizeString = juce::String(chordSize);
  auto intervalStringIntro = juce::String("the current ") + chordSizeString + juce::String("-note chord has intervals\n");
  // g.drawFittedText (intervalStringIntro, x, y, w, h, juce::Justification::centred, 1);
  // g.setFont (float(chordFontSize));
  y += (fontSize+chordFontSize)/2 + lineSep;
  // g.drawFittedText (currentIntervalString, x, y, w, h, juce::Justification::centred, 1);

  intervalsLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), fontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  intervalsLabel.setText(TRANS(rootKeyStringIntro + currentRootKeyString + "\n" + intervalStringIntro + currentIntervalString),
                        juce::dontSendNotification);

  y += chordFontSize + lineSep;
  // g.drawFittedText (currentChordString, x, y, w, h, juce::Justification::centred, 1);
  chordLabel.setFont (juce::Font (juce::Font::getDefaultSerifFontName(), chordFontSize, juce::Font::plain).withTypefaceStyle ("Regular"));
  chordLabel.setText(TRANS(currentChordString), juce::dontSendNotification);
  //controlButtons.getChordLabel().setTextSize(chordFontSize);
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
  // TEST HACK
  //std::cout << "paint ";
  //return;

  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
  juce::Grid grid;
  using Track = juce::Grid::TrackInfo;
  using Fr = juce::Grid::Fr;
  grid.templateColumns = { Track (Fr (1)) }; // all one column
  grid.templateRows = {   Track (Fr (1)) // control buttons along top
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
    //, juce::GridItem (pluginGUI.getIntervals()), // for TEST
    //, juce::GridItem (pluginGUI.getIntervals()),
    //, juce::GridItem (pluginGUI.getChord())
  };
  grid.performLayout (getLocalBounds());
}
