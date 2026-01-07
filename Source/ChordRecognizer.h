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

/**
 * ChordRecognizer identifies chords from a set of pitch classes.
 *
 * Features:
 * - Comprehensive chord database (triads, 7ths, 9ths, 11ths, 13ths, altered, etc.)
 * - Automatic root detection (tries all 12 possible roots)
 * - Inversion detection with slash chord notation (e.g., C/E)
 * - Score-based matching for best fit
 */
class ChordRecognizer
{
public:
    using PitchClassSet = std::array<int, 12>;

    struct ChordDefinition {
        std::string name;           // e.g., "", "m", "7", "maj7", "dim7"
        std::vector<int> intervals; // semitones from root: {0, 4, 7} for major triad
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
     * @return ChordResult with identified chord info
     */
    ChordResult identify(const PitchClassSet& pitchClasses) const;

    /**
     * Get the note name for a pitch class (0-11).
     * Uses sharps for black keys by default.
     */
    static juce::String pitchClassName(int pitchClass, bool preferFlats = false);

    /**
     * Get interval name for semitones from root.
     */
    static juce::String intervalName(int semitones);

private:
    std::vector<ChordDefinition> chordDatabase;

    void initializeChordDatabase();

    /**
     * Try to match pitch classes with a specific root.
     * Returns best matching chord definition and score.
     */
    std::pair<const ChordDefinition*, float> matchWithRoot(
        const PitchClassSet& pitchClasses,
        int root) const;

    /**
     * Convert pitch class set to interval set relative to a root.
     */
    std::vector<int> pitchClassesToIntervals(
        const PitchClassSet& pitchClasses,
        int root) const;

    /**
     * Find the lowest pitch class present (for bass note detection).
     */
    int findLowestPitchClass(const PitchClassSet& pitchClasses) const;

    /**
     * Calculate match score between intervals and chord definition.
     */
    float calculateMatchScore(
        const std::vector<int>& intervals,
        const ChordDefinition& chord) const;
};
