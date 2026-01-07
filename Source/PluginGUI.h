#pragma once

#include <JuceHeader.h>

//#include "PluginEditor.h"
class PluginEditor;

class PluginGUI  : public juce::Component,
                   public juce::Button::Listener,
                   public juce::ComboBox::Listener
{
public:

  PluginGUI (juce::Rectangle<int> initialBounds, PluginEditor* editorP, std::function<void()>& resetLambdaR);
  ~PluginGUI() override;

  void paint (juce::Graphics& g) override;
  void resized() override;
  void buttonClicked (juce::Button* buttonThatWasClicked) override;
  void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;

private:
  PluginEditor* editor;
  std::function<void()>& resetLambda;

  std::unique_ptr<juce::GroupComponent> guiGroup;
  std::unique_ptr<juce::TextButton> resetButton;
  std::unique_ptr<juce::TextButton> chordDisplayerButton;
  std::unique_ptr<juce::Label> chordDisplayer; // See https://forum.juce.com/t/open-a-new-window/40347/2
  std::unique_ptr<juce::ComboBox> rootKeyComboBox;
  std::unique_ptr<juce::Label> rootKeyLabel;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGUI)
};
