/*
  ==============================================================================
  ChordRecognizer - Implementation
  ==============================================================================
*/

#include "ChordRecognizer.h"
#include <algorithm>
#include <cmath>

ChordRecognizer::ChordRecognizer()
{
    initializeChordDatabase();
}

void ChordRecognizer::initializeChordDatabase()
{
    // Priority levels:
    // 100+ : Basic triads and common chords (prefer these)
    // 80-99: Common 7th chords
    // 60-79: Extended chords (9th, 11th, 13th)
    // 40-59: Altered and less common chords
    // 20-39: Unusual voicings
    // 1-19 : Catch-all patterns

    chordDatabase = {
        // ============================================
        // TRIADS (Priority 100+)
        // ============================================

        // Major triad: 1 3 5
        {"", {0, 4, 7}, 110},

        // Minor triad: 1 b3 5
        {"m", {0, 3, 7}, 110},

        // Diminished triad: 1 b3 b5
        {"dim", {0, 3, 6}, 105},

        // Augmented triad: 1 3 #5
        {"+", {0, 4, 8}, 105},
        {"aug", {0, 4, 8}, 104},

        // Suspended 4: 1 4 5
        {"sus4", {0, 5, 7}, 100},
        {"sus", {0, 5, 7}, 99},

        // Suspended 2: 1 2 5
        {"sus2", {0, 2, 7}, 100},

        // Power chord: 1 5
        {"5", {0, 7}, 95},

        // ============================================
        // SEVENTH CHORDS (Priority 80-99)
        // ============================================

        // Major 7: 1 3 5 7
        {"maj7", {0, 4, 7, 11}, 90},
        {"M7", {0, 4, 7, 11}, 89},

        // Minor 7: 1 b3 5 b7
        {"m7", {0, 3, 7, 10}, 90},
        {"-7", {0, 3, 7, 10}, 89},

        // Dominant 7: 1 3 5 b7
        {"7", {0, 4, 7, 10}, 92},

        // Diminished 7: 1 b3 b5 bb7
        {"dim7", {0, 3, 6, 9}, 88},
        {"o7", {0, 3, 6, 9}, 87},

        // Half-diminished (minor 7 flat 5): 1 b3 b5 b7
        {"m7b5", {0, 3, 6, 10}, 88},
        {"ø7", {0, 3, 6, 10}, 87},

        // Minor-major 7: 1 b3 5 7
        {"mMaj7", {0, 3, 7, 11}, 85},
        {"m(maj7)", {0, 3, 7, 11}, 84},

        // Augmented major 7: 1 3 #5 7
        {"maj7#5", {0, 4, 8, 11}, 82},
        {"+maj7", {0, 4, 8, 11}, 81},

        // Augmented 7: 1 3 #5 b7
        {"7#5", {0, 4, 8, 10}, 82},
        {"+7", {0, 4, 8, 10}, 81},
        {"aug7", {0, 4, 8, 10}, 80},

        // 7sus4: 1 4 5 b7
        {"7sus4", {0, 5, 7, 10}, 85},
        {"7sus", {0, 5, 7, 10}, 84},

        // 7sus2: 1 2 5 b7
        {"7sus2", {0, 2, 7, 10}, 85},

        // ============================================
        // SIXTH CHORDS (Priority 80-89)
        // ============================================

        // Major 6: 1 3 5 6
        {"6", {0, 4, 7, 9}, 86},
        {"maj6", {0, 4, 7, 9}, 85},

        // Minor 6: 1 b3 5 6
        {"m6", {0, 3, 7, 9}, 86},

        // 6/9: 1 3 5 6 9
        {"6/9", {0, 4, 7, 9, 14}, 75},
        {"6add9", {0, 4, 7, 9, 2}, 74},

        // ============================================
        // NINTH CHORDS (Priority 60-79)
        // ============================================

        // Major 9: 1 3 5 7 9
        {"maj9", {0, 4, 7, 11, 14}, 78},
        {"maj9", {0, 4, 7, 11, 2}, 77}, // 9 as 2

        // Minor 9: 1 b3 5 b7 9
        {"m9", {0, 3, 7, 10, 14}, 78},
        {"m9", {0, 3, 7, 10, 2}, 77},

        // Dominant 9: 1 3 5 b7 9
        {"9", {0, 4, 7, 10, 14}, 80},
        {"9", {0, 4, 7, 10, 2}, 79},

        // Add 9: 1 3 5 9 (no 7)
        {"add9", {0, 4, 7, 14}, 76},
        {"add9", {0, 4, 7, 2}, 75},
        {"add2", {0, 2, 4, 7}, 74},

        // Minor add 9: 1 b3 5 9
        {"madd9", {0, 3, 7, 14}, 76},
        {"madd9", {0, 3, 7, 2}, 75},

        // 7b9: 1 3 5 b7 b9
        {"7b9", {0, 4, 7, 10, 13}, 72},
        {"7b9", {0, 4, 7, 10, 1}, 71},

        // 7#9: 1 3 5 b7 #9
        {"7#9", {0, 4, 7, 10, 15}, 72},
        {"7#9", {0, 4, 7, 10, 3}, 71},

        // ============================================
        // ELEVENTH CHORDS (Priority 55-65)
        // ============================================

        // Major 11: 1 3 5 7 9 11
        {"maj11", {0, 4, 7, 11, 14, 17}, 62},
        {"maj11", {0, 4, 7, 11, 2, 5}, 61},

        // Minor 11: 1 b3 5 b7 9 11
        {"m11", {0, 3, 7, 10, 14, 17}, 62},
        {"m11", {0, 3, 7, 10, 2, 5}, 61},

        // Dominant 11: 1 3 5 b7 9 11
        {"11", {0, 4, 7, 10, 14, 17}, 64},
        {"11", {0, 4, 7, 10, 2, 5}, 63},

        // 9sus4 / 11 (no 3): 1 4 5 b7 9
        {"9sus4", {0, 5, 7, 10, 14}, 60},
        {"9sus4", {0, 5, 7, 10, 2}, 59},

        // Add 11: 1 3 5 11
        {"add11", {0, 4, 7, 17}, 58},
        {"add11", {0, 4, 7, 5}, 57},
        {"add4", {0, 4, 5, 7}, 56},

        // ============================================
        // THIRTEENTH CHORDS (Priority 50-60)
        // ============================================

        // Major 13: 1 3 5 7 9 11 13
        {"maj13", {0, 4, 7, 11, 14, 17, 21}, 55},
        {"maj13", {0, 4, 7, 11, 2, 5, 9}, 54},

        // Minor 13: 1 b3 5 b7 9 11 13
        {"m13", {0, 3, 7, 10, 14, 17, 21}, 55},
        {"m13", {0, 3, 7, 10, 2, 5, 9}, 54},

        // Dominant 13: 1 3 5 b7 9 13 (often no 11)
        {"13", {0, 4, 7, 10, 14, 21}, 58},
        {"13", {0, 4, 7, 10, 2, 9}, 57},

        // ============================================
        // ALTERED CHORDS (Priority 45-55)
        // ============================================

        // 7b5: 1 3 b5 b7
        {"7b5", {0, 4, 6, 10}, 52},

        // maj7b5: 1 3 b5 7
        {"maj7b5", {0, 4, 6, 11}, 50},

        // 7#11: 1 3 5 b7 #11
        {"7#11", {0, 4, 7, 10, 18}, 48},
        {"7#11", {0, 4, 7, 10, 6}, 47},

        // 9#11: 1 3 5 b7 9 #11
        {"9#11", {0, 4, 7, 10, 14, 18}, 48},
        {"9#11", {0, 4, 7, 10, 2, 6}, 47},

        // 13b9: 1 3 5 b7 b9 13
        {"13b9", {0, 4, 7, 10, 13, 21}, 46},
        {"13b9", {0, 4, 7, 10, 1, 9}, 45},

        // alt (altered dominant): 1 3 b7 b9 #9 b5/#5
        {"alt", {0, 4, 10, 1, 3, 6}, 40},
        {"7alt", {0, 4, 10, 1, 3, 8}, 40},

        // ============================================
        // PARTIAL/INCOMPLETE CHORDS (Priority 20-40)
        // ============================================

        // Major no 5: 1 3
        {"(no5)", {0, 4}, 30},

        // Minor no 5: 1 b3
        {"m(no5)", {0, 3}, 30},

        // 7 no 5: 1 3 b7
        {"7(no5)", {0, 4, 10}, 35},

        // 7 no 3: 1 5 b7
        {"7(no3)", {0, 7, 10}, 35},

        // maj7 no 5: 1 3 7
        {"maj7(no5)", {0, 4, 11}, 35},

        // maj7 no 3: 1 5 7
        {"maj7(no3)", {0, 7, 11}, 35},

        // m7 no 5: 1 b3 b7
        {"m7(no5)", {0, 3, 10}, 35},

        // Quartal voicings
        {"quartal", {0, 5, 10}, 25},  // stacked 4ths
        {"quartal", {0, 5, 10, 15}, 25},
    };
}

juce::String ChordRecognizer::pitchClassName(int pitchClass, bool preferFlats)
{
    pitchClass = ((pitchClass % 12) + 12) % 12; // normalize to 0-11

    static const char* sharpNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    static const char* flatNames[] =  {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

    return preferFlats ? flatNames[pitchClass] : sharpNames[pitchClass];
}

juce::String ChordRecognizer::intervalName(int semitones)
{
    semitones = ((semitones % 12) + 12) % 12;

    static const char* names[] = {"1", "b2", "2", "b3", "3", "4", "b5", "5", "#5", "6", "b7", "7"};
    return names[semitones];
}

int ChordRecognizer::findLowestPitchClass(const PitchClassSet& pitchClasses) const
{
    // In the context of a chord, we can't know the actual bass note
    // without octave information. We'll return the first non-zero pitch class
    // starting from C (0).
    for (int i = 0; i < 12; i++) {
        if (pitchClasses[i] != 0)
            return i;
    }
    return -1; // no notes
}

std::vector<int> ChordRecognizer::pitchClassesToIntervals(
    const PitchClassSet& pitchClasses,
    int root) const
{
    std::vector<int> intervals;

    for (int i = 0; i < 12; i++) {
        if (pitchClasses[i] != 0) {
            int interval = (i - root + 12) % 12;
            intervals.push_back(interval);
        }
    }

    std::sort(intervals.begin(), intervals.end());
    return intervals;
}

float ChordRecognizer::calculateMatchScore(
    const std::vector<int>& intervals,
    const ChordDefinition& chord) const
{
    if (intervals.empty() || chord.intervals.empty())
        return 0.0f;

    // Normalize chord intervals to 0-11 range
    std::vector<int> chordIntervals;
    for (int i : chord.intervals) {
        chordIntervals.push_back(i % 12);
    }
    std::sort(chordIntervals.begin(), chordIntervals.end());

    // Remove duplicates from chord intervals
    chordIntervals.erase(std::unique(chordIntervals.begin(), chordIntervals.end()),
                         chordIntervals.end());

    // Count matches and misses
    int matches = 0;
    int extraNotes = 0;
    int missingNotes = 0;

    // Check which chord intervals are present
    for (int ci : chordIntervals) {
        bool found = std::find(intervals.begin(), intervals.end(), ci) != intervals.end();
        if (found) {
            matches++;
        } else {
            missingNotes++;
        }
    }

    // Check for extra notes not in the chord definition
    for (int pi : intervals) {
        bool found = std::find(chordIntervals.begin(), chordIntervals.end(), pi) != chordIntervals.end();
        if (!found) {
            extraNotes++;
        }
    }

    // Must have the root
    bool hasRoot = std::find(intervals.begin(), intervals.end(), 0) != intervals.end();
    if (!hasRoot)
        return 0.0f;

    // For a perfect match: all chord intervals present, no extra notes
    float matchRatio = static_cast<float>(matches) / chordIntervals.size();
    float extraPenalty = static_cast<float>(extraNotes) * 0.1f;
    float missingPenalty = static_cast<float>(missingNotes) * 0.15f;

    float score = matchRatio - extraPenalty - missingPenalty;

    // Bonus for exact match
    if (matches == chordIntervals.size() && extraNotes == 0)
        score += 0.2f;

    // Add priority bonus (normalized)
    score += chord.priority * 0.001f;

    return std::max(0.0f, std::min(1.0f, score));
}

std::pair<const ChordRecognizer::ChordDefinition*, float>
ChordRecognizer::matchWithRoot(const PitchClassSet& pitchClasses, int root) const
{
    std::vector<int> intervals = pitchClassesToIntervals(pitchClasses, root);

    if (intervals.empty())
        return {nullptr, 0.0f};

    // Must have root present
    if (std::find(intervals.begin(), intervals.end(), 0) == intervals.end())
        return {nullptr, 0.0f};

    const ChordDefinition* bestMatch = nullptr;
    float bestScore = 0.0f;

    for (const auto& chord : chordDatabase) {
        float score = calculateMatchScore(intervals, chord);
        if (score > bestScore) {
            bestScore = score;
            bestMatch = &chord;
        }
    }

    return {bestMatch, bestScore};
}

ChordRecognizer::ChordResult ChordRecognizer::identify(const PitchClassSet& pitchClasses) const
{
    ChordResult result;
    result.fullName = "";
    result.rootName = "";
    result.quality = "";
    result.bassNote = "";
    result.rootPitchClass = -1;
    result.bassPitchClass = -1;
    result.matchScore = 0.0f;
    result.isInversion = false;

    // Count notes present
    int noteCount = 0;
    for (int i = 0; i < 12; i++) {
        if (pitchClasses[i] != 0)
            noteCount++;
    }

    if (noteCount == 0)
        return result;

    if (noteCount == 1) {
        // Single note - just return the note name
        int pc = findLowestPitchClass(pitchClasses);
        result.rootPitchClass = pc;
        result.bassPitchClass = pc;
        result.rootName = pitchClassName(pc);
        result.fullName = result.rootName;
        result.matchScore = 1.0f;
        return result;
    }

    // Try each pitch class as potential root
    const ChordDefinition* bestChord = nullptr;
    float bestScore = 0.0f;
    int bestRoot = 0;

    for (int root = 0; root < 12; root++) {
        if (pitchClasses[root] == 0)
            continue; // Root must be present

        auto [chord, score] = matchWithRoot(pitchClasses, root);

        if (score > bestScore) {
            bestScore = score;
            bestChord = chord;
            bestRoot = root;
        }
    }

    if (bestChord == nullptr || bestScore < 0.3f) {
        // No good match - return interval description
        result.fullName = "?";
        return result;
    }

    // Determine bass note (lowest pitch class present)
    int bassPC = findLowestPitchClass(pitchClasses);

    // Build result
    result.rootPitchClass = bestRoot;
    result.bassPitchClass = bassPC;
    result.matchScore = bestScore;

    // Determine if we should use flats based on the root
    bool useFlats = (bestRoot == 1 || bestRoot == 3 || bestRoot == 6 ||
                     bestRoot == 8 || bestRoot == 10); // Db, Eb, Gb, Ab, Bb

    result.rootName = pitchClassName(bestRoot, useFlats);
    result.quality = bestChord->name;

    // Check for inversion
    if (bassPC != bestRoot) {
        result.isInversion = true;
        result.bassNote = pitchClassName(bassPC, useFlats);
        result.fullName = result.rootName + result.quality + "/" + result.bassNote;
    } else {
        result.fullName = result.rootName + result.quality;
    }

    return result;
}
