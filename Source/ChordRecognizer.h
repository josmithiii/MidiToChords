/*
  ==============================================================================
  ChordRecognizer - Comprehensive chord identification from pitch classes
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <string>

class ChordRecognizer
{
public:
    using PitchClassSet = std::array<int, 12>;

    struct ChordDefinition {
        std::string name;           // e.g., "", "m", "7", "maj7", "dim7"
        std::vector<int> intervals; // semitones from root (0-11)
        int priority;               // higher = preferred when multiple match
    };

    struct ChordResult {
        juce::String fullName;      // e.g., "Cmaj7", "F#m7b5"
        juce::String rootName;      // e.g., "C", "F#"
        juce::String quality;       // e.g., "maj7", "m7b5"
        juce::String bassNote;      // e.g., "E" for C/E (empty if root position)
        int rootPitchClass;         // 0-11
        int bassPitchClass;         // 0-11, same as root if root position
        float matchScore;           // 0.0 to 1.0, how well the chord matches
        bool isInversion;           // true if bass note != root
    };

    ChordRecognizer();

    /**
     * Identify the chord from pitch classes present.
     * @param pitchClasses Array of 12 ints, non-zero means that pitch class is present
     * @param lowestMidiNote The lowest MIDI note currently held (-1 if unknown)
     * @return ChordResult with identified chord info
     */
    ChordResult identify(const PitchClassSet& pitchClasses, int lowestMidiNote = -1) const;

    static juce::String pitchClassName(int pitchClass, bool preferFlats = false);
    static juce::String intervalName(int semitones);

private:
    std::vector<ChordDefinition> chordDatabase;

    void initializeChordDatabase();

    std::pair<const ChordDefinition*, float> matchWithRoot(
        const PitchClassSet& pitchClasses,
        int root) const;

    std::vector<int> pitchClassesToIntervals(
        const PitchClassSet& pitchClasses,
        int root) const;

    float calculateMatchScore(
        const std::vector<int>& intervals,
        const ChordDefinition& chord) const;
};
