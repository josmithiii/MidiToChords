#include "PluginGUI.h"

#include "PluginEditor.h"

PluginGUI::PluginGUI (juce::Rectangle<int> initialBounds, PluginEditor* editorP, std::function<void()>& resetLambdaR)
  : editor(editorP), resetLambda(resetLambdaR)
{
  guiGroup.reset (new juce::GroupComponent ("guiGroup", TRANS("GUI")));
  addAndMakeVisible (guiGroup.get());
  guiGroup->setBounds (initialBounds);

  resetButton.reset (new juce::TextButton ("resetButton"));
  addAndMakeVisible (resetButton.get());
  resetButton->setTooltip (TRANS("Clear all MIDI state in plugin"));
  resetButton->setButtonText (TRANS("Reset"));
  resetButton->addListener (this);

  chordDisplayerButton.reset (new juce::TextButton ("chordDisplayerButton"));
  addAndMakeVisible (chordDisplayerButton.get());
  chordDisplayerButton->setTooltip (TRANS("Was pop out chord display"));
  chordDisplayerButton->setButtonText (TRANS("This Button Intentionally Left Blank"));
  chordDisplayerButton->addListener (this);

  rootKeyComboBox.reset (new juce::ComboBox ("rootKeyComboBox"));
  addAndMakeVisible (rootKeyComboBox.get());
  rootKeyComboBox->setTooltip (TRANS("Root key assumed for chord spellings when reasonable"));
  rootKeyComboBox->setEditableText (false);
  rootKeyComboBox->setJustificationType (juce::Justification::centredLeft);
  rootKeyComboBox->setTextWhenNothingSelected (TRANS("C"));
  rootKeyComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
  rootKeyComboBox->addItem (TRANS("C"), 1);
  rootKeyComboBox->addItem (TRANS("C#"), 2);
  rootKeyComboBox->addItem (TRANS("D"), 3);
  rootKeyComboBox->addItem (TRANS("Eb"), 4);
  rootKeyComboBox->addItem (TRANS("E"), 5);
  rootKeyComboBox->addItem (TRANS("F"), 6);
  rootKeyComboBox->addItem (TRANS("F#"), 7);
  rootKeyComboBox->addItem (TRANS("G"), 8);
  rootKeyComboBox->addItem (TRANS("Ab"), 9);
  rootKeyComboBox->addItem (TRANS("A"), 10);
  rootKeyComboBox->addItem (TRANS("Bb"), 11);
  rootKeyComboBox->addListener (this);

  rootKeyLabel.reset (new juce::Label ("rootKeyLabel", TRANS("Root Key:\n")));
  addAndMakeVisible (rootKeyLabel.get());
  rootKeyLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
  rootKeyLabel->setJustificationType (juce::Justification::centredRight);
  rootKeyLabel->setEditable (false, false, false);
  rootKeyLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
  rootKeyLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

  chordDisplayer.reset (new juce::Label ("chordDisplayer", TRANS("Chord" /* \nGoes\nHERE" */)));
  // wait: addAndMakeVisible (chordDisplayer.get());
  chordDisplayer->setFont (juce::Font (64.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
  chordDisplayer->setJustificationType (juce::Justification::centred);
  chordDisplayer->setEditable (false, false, false);
  chordDisplayer->setColour (juce::TextEditor::textColourId, juce::Colours::black);
  chordDisplayer->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
  chordDisplayer->setSize (initialBounds.getWidth(), initialBounds.getHeight());

  setBounds(initialBounds); // triggers resized
}

PluginGUI::~PluginGUI()
{
  guiGroup = nullptr;
  resetButton = nullptr;
  chordDisplayerButton = nullptr;
  rootKeyComboBox = nullptr;
  rootKeyLabel = nullptr;
  chordDisplayer = nullptr;
}

//==============================================================================
void PluginGUI::paint (juce::Graphics& g)
{
  g.fillAll (juce::Colour (0xff323e44));
}

void PluginGUI::resized()
{
#if ORIGINAL_GUI == 1
  guiGroup->setBoundsRelative(.05,.05,.9,.9);
  resetButton->setBoundsRelative(0.1,0.25,0.2,0.2); // (x,y,w,h)
  chordDisplayerButton->setBoundsRelative(0.1,0.5,0.2,0.2);
  rootKeyLabel->setBoundsRelative(0.5,0.25,0.2,0.2);
  rootKeyComboBox->setBoundsRelative(0.7,0.25,0.2,0.2);
#else
  guiGroup->setBoundsRelative(.05,.05,.9,.9);
  juce::FlexBox fb;
  fb.flexWrap = juce::FlexBox::Wrap::wrap;
  fb.justifyContent = juce::FlexBox::JustifyContent::center;
  fb.alignContent = juce::FlexBox::AlignContent::center; // was stretch - alignment of grid-matrix as a whole
  fb.alignItems = juce::FlexBox::AlignItems::center;  // was stretch - alignment within the grid-matrix
  fb.items.add (juce::FlexItem (*resetButton.get()).withMinWidth (100.0f).withMinHeight (20.0f));
  fb.items.add (juce::FlexItem (*chordDisplayerButton.get()).withMinWidth (100.0f).withMinHeight (20.0f));
  fb.items.add (juce::FlexItem (*rootKeyComboBox.get()).withMinWidth (100.0f).withMinHeight (20.0f));
  fb.performLayout (getLocalBounds().toFloat());
#endif 
}

void PluginGUI::buttonClicked (juce::Button* buttonThatWasClicked)
{
  if (buttonThatWasClicked == resetButton.get())
    {
      resetLambda();
    } else if (buttonThatWasClicked == chordDisplayerButton.get()) {
    DBG("Pop out chord display");
    chordDisplayer->setBounds(getBounds());
    addAndMakeVisible (chordDisplayer.get());
    //chordDisplayer->bringToFront();
  }
}

void PluginGUI::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
  if (comboBoxThatHasChanged == rootKeyComboBox.get())
    {
      // int newKeyIndex = rootKeyComboBox->getSelectedId();
      //!!! rootKeyLambda(rootKeyComboBox->getText());
      editor->setRootKey(rootKeyComboBox->getText());
    }
}
