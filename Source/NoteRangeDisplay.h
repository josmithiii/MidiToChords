// NoteRangeDisplay.h — Shows "Range: C₂ - F#₅" with per-note coloring.
// Red if a note falls outside the GeoKeys grid range, white if in range.

#pragma once

#include <JuceHeader.h>

class NoteRangeDisplay : public juce::Component
{
public:
    void setRange(int lo, int hi, int gridMin, int gridMax)
    {
        if (lo == lowNote && hi == highNote && gridMin == gMin && gridMax == gMax)
            return;
        lowNote = lo; highNote = hi; gMin = gridMin; gMax = gridMax;
        repaint();
    }

    void setFontSize(float size)
    {
        if (fontSize != size) { fontSize = size; repaint(); }
    }

    void paint(juce::Graphics& g) override
    {
        static const char* noteNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
        auto toStr = [](int midi) -> juce::String {
            if (midi < 0) return {};
            int pc = midi % 12;
            int oct = midi / 12 - 1;
            juce::String s(noteNames[pc]);
            if (oct < 0) {
                s += juce::String::charToString((juce::juce_wchar)0x208B); // subscript minus
                oct = -oct;
            }
            if (oct >= 10)
                s += juce::String::charToString((juce::juce_wchar)(0x2080 + oct / 10));
            s += juce::String::charToString((juce::juce_wchar)(0x2080 + oct % 10));
            return s;
        };

        auto font = juce::Font(fontSize);
        g.setFont(font);

        juce::String loStr = toStr(lowNote);
        juce::String hiStr = toStr(highNote);
        bool loRed = lowNote >= 0 && (lowNote < gMin || lowNote > gMax);
        bool hiRed = highNote >= 0 && (highNote < gMin || highNote > gMax);

        float x = 2.0f;
        float h = (float)getHeight();

        // "Range: " prefix in white
        g.setColour(juce::Colours::white);
        juce::String prefix("Range: ");
        g.drawText(prefix, (int)x, 0, (int)font.getStringWidthFloat(prefix) + 2, (int)h,
                   juce::Justification::centredLeft, false);
        x += font.getStringWidthFloat(prefix);

        g.setColour(loRed ? juce::Colours::red : juce::Colours::white);
        g.drawText(loStr, (int)x, 0, (int)font.getStringWidthFloat(loStr) + 4, (int)h,
                   juce::Justification::centredLeft, false);
        x += font.getStringWidthFloat(loStr);

        g.setColour(juce::Colours::white);
        juce::String sep(" - ");
        g.drawText(sep, (int)x, 0, (int)font.getStringWidthFloat(sep) + 4, (int)h,
                   juce::Justification::centredLeft, false);
        x += font.getStringWidthFloat(sep);

        g.setColour(hiRed ? juce::Colours::red : juce::Colours::white);
        g.drawText(hiStr, (int)x, 0, (int)font.getStringWidthFloat(hiStr) + 4, (int)h,
                   juce::Justification::centredLeft, false);
    }

private:
    int lowNote = -1, highNote = -1, gMin = 0, gMax = 127;
    float fontSize = 14.0f;
};
