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
    //
    // All intervals are in 0-11 (pitch class) range.

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

        // Suspended 4: 1 4 5
        {"sus4", {0, 5, 7}, 100},

        // Suspended 2: 1 2 5
        {"sus2", {0, 2, 7}, 100},

        // Power chord: 1 5
        {"5", {0, 7}, 95},

        // ============================================
        // SEVENTH CHORDS (Priority 80-99)
        // ============================================

        // Major 7: 1 3 5 7
        {"maj7", {0, 4, 7, 11}, 90},

        // Minor 7: 1 b3 5 b7
        {"m7", {0, 3, 7, 10}, 90},

        // Dominant 7: 1 3 5 b7
        {"7", {0, 4, 7, 10}, 92},

        // Diminished 7: 1 b3 b5 bb7(=6)
        {"dim7", {0, 3, 6, 9}, 88},

        // Half-diminished (minor 7 flat 5): 1 b3 b5 b7
        {"m7b5", {0, 3, 6, 10}, 88},

        // Minor-major 7: 1 b3 5 7
        {"mMaj7", {0, 3, 7, 11}, 85},

        // Augmented major 7: 1 3 #5 7
        {"maj7#5", {0, 4, 8, 11}, 82},

        // Augmented 7: 1 3 #5 b7
        {"7#5", {0, 4, 8, 10}, 82},

        // 7sus4: 1 4 5 b7
        {"7sus4", {0, 5, 7, 10}, 85},

        // 7sus2: 1 2 5 b7
        {"7sus2", {0, 2, 7, 10}, 85},

        // ============================================
        // SIXTH CHORDS (Priority 80-89)
        // ============================================

        // Major 6: 1 3 5 6
        {"6", {0, 4, 7, 9}, 86},

        // Minor 6: 1 b3 5 6
        {"m6", {0, 3, 7, 9}, 86},

        // 6/9: 1 3 5 6 9(=2)
        {"6/9", {0, 2, 4, 7, 9}, 75},

        // ============================================
        // NINTH CHORDS (Priority 60-79)
        // ============================================

        // Major 9: 1 3 5 7 9(=2)
        {"maj9", {0, 2, 4, 7, 11}, 78},

        // Minor 9: 1 b3 5 b7 9(=2)
        {"m9", {0, 2, 3, 7, 10}, 78},

        // Dominant 9: 1 3 5 b7 9(=2)
        {"9", {0, 2, 4, 7, 10}, 80},

        // Add 9: 1 3 5 9(=2), no 7
        {"add9", {0, 2, 4, 7}, 75},

        // Minor add 9: 1 b3 5 9(=2)
        {"madd9", {0, 2, 3, 7}, 75},

        // 7b9: 1 3 5 b7 b9(=1)
        {"7b9", {0, 1, 4, 7, 10}, 72},

        // 7#9: 1 3 5 b7 #9(=3)
        {"7#9", {0, 3, 4, 7, 10}, 72},

        // ============================================
        // ELEVENTH CHORDS (Priority 55-65)
        // ============================================

        // Major 11: 1 3 5 7 9(=2) 11(=5)
        {"maj11", {0, 2, 4, 5, 7, 11}, 62},

        // Minor 11: 1 b3 5 b7 9(=2) 11(=5)
        {"m11", {0, 2, 3, 5, 7, 10}, 62},

        // Dominant 11: 1 3 5 b7 9(=2) 11(=5)
        {"11", {0, 2, 4, 5, 7, 10}, 64},

        // 9sus4: 1 4 5 b7 9(=2)
        {"9sus4", {0, 2, 5, 7, 10}, 60},

        // Add 11: 1 3 5 11(=5)
        {"add11", {0, 4, 5, 7}, 57},

        // ============================================
        // THIRTEENTH CHORDS (Priority 50-60)
        // ============================================

        // Major 13: 1 3 5 7 9(=2) 11(=5) 13(=9)
        {"maj13", {0, 2, 4, 5, 7, 9, 11}, 55},

        // Minor 13: 1 b3 5 b7 9(=2) 11(=5) 13(=9)
        {"m13", {0, 2, 3, 5, 7, 9, 10}, 55},

        // Dominant 13: 1 3 5 b7 9(=2) 13(=9) (often no 11)
        {"13", {0, 2, 4, 7, 9, 10}, 58},

        // ============================================
        // ALTERED CHORDS (Priority 45-55)
        // ============================================

        // 7b5: 1 3 b5 b7
        {"7b5", {0, 4, 6, 10}, 52},

        // maj7b5: 1 3 b5 7
        {"maj7b5", {0, 4, 6, 11}, 50},

        // 7#11: 1 3 5 b7 #11(=6)
        {"7#11", {0, 4, 6, 7, 10}, 48},

        // 9#11: 1 3 5 b7 9(=2) #11(=6)
        {"9#11", {0, 2, 4, 6, 7, 10}, 47},

        // 13b9: 1 3 5 b7 b9(=1) 13(=9)
        {"13b9", {0, 1, 4, 7, 9, 10}, 46},

        // alt (altered dominant): 1 3 b7 b9(=1) #9(=3) b5(=6)
        {"alt", {0, 1, 3, 4, 6, 10}, 40},

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

        // Quartal voicings: stacked 4ths
        {"quartal", {0, 5, 10}, 25},
    };
}

juce::String ChordRecognizer::pitchClassName(int pitchClass, bool preferFlats)
{
    pitchClass = ((pitchClass % 12) + 12) % 12;

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

    const auto& chordIntervals = chord.intervals; // already 0-11, no normalization needed

    // Count matches and misses
    int matches = 0;
    int extraNotes = 0;
    int missingNotes = 0;

    for (int ci : chordIntervals) {
        if (std::find(intervals.begin(), intervals.end(), ci) != intervals.end())
            matches++;
        else
            missingNotes++;
    }

    for (int pi : intervals) {
        if (std::find(chordIntervals.begin(), chordIntervals.end(), pi) == chordIntervals.end())
            extraNotes++;
    }

    // Must have the root
    if (std::find(intervals.begin(), intervals.end(), 0) == intervals.end())
        return 0.0f;

    float matchRatio = static_cast<float>(matches) / static_cast<float>(chordIntervals.size());
    float extraPenalty = static_cast<float>(extraNotes) * 0.1f;
    float missingPenalty = static_cast<float>(missingNotes) * 0.15f;

    float score = matchRatio - extraPenalty - missingPenalty;

    // Bonus for exact match
    if (matches == static_cast<int>(chordIntervals.size()) && extraNotes == 0)
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

ChordRecognizer::ChordResult ChordRecognizer::identify(const PitchClassSet& pitchClasses, int lowestMidiNote) const
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
        int pc = (lowestMidiNote >= 0) ? (lowestMidiNote % 12) : -1;
        if (pc < 0) {
            for (int i = 0; i < 12; i++) {
                if (pitchClasses[i] != 0) { pc = i; break; }
            }
        }
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
            continue;

        auto [chord, score] = matchWithRoot(pitchClasses, root);

        if (score > bestScore) {
            bestScore = score;
            bestChord = chord;
            bestRoot = root;
        }
    }

    if (bestChord == nullptr || bestScore < 0.3f) {
        result.fullName = "?";
        return result;
    }

    // Use actual lowest MIDI note for accurate bass detection
    int bassPC = (lowestMidiNote >= 0) ? (lowestMidiNote % 12) : bestRoot;

    // Build result
    result.rootPitchClass = bestRoot;
    result.bassPitchClass = bassPC;
    result.matchScore = bestScore;

    bool useFlats = (bestRoot == 1 || bestRoot == 3 || bestRoot == 6 ||
                     bestRoot == 8 || bestRoot == 10);

    result.rootName = pitchClassName(bestRoot, useFlats);
    result.quality = bestChord->name;

    if (bassPC != bestRoot && pitchClasses[bassPC] != 0) {
        result.isInversion = true;
        result.bassNote = pitchClassName(bassPC, useFlats);
        result.fullName = result.rootName + result.quality + "/" + result.bassNote;
    } else {
        result.fullName = result.rootName + result.quality;
    }

    return result;
}
