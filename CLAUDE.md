# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure and build (from project root)
cmake -B build
cmake --build build

# Clean rebuild
rm -rf build && cmake -B build && cmake --build build
```

Output artifacts:
- `build/MidiToChords_artefacts/Standalone/MidiToChords.app`
- `build/MidiToChords_artefacts/AU/MidiToChords.component`

## Project Overview

MidiToChords is a JUCE-based AU/Standalone MIDI plugin that displays chord names from MIDI input in real-time. It uses CMake for building and requires C++17.

## Architecture

**PluginProcessor** (`Source/PluginProcessor.cpp/.h`)
- Core JUCE AudioProcessor handling MIDI input
- Tracks which pitch classes (0-11) are currently held via `pitchClassesPresent` array
- Provides `MidiKeyboardState` for the visual keyboard component

**PluginEditor** (`Source/PluginEditor.cpp/.h`)
- Main GUI component with timer-based refresh (polls processor state)
- Hosts the MIDI keyboard display and control panel
- Delegates chord recognition to `ChordRecognizer`

**ChordRecognizer** (`Source/ChordRecognizer.cpp/.h`)
- Stateless chord identification from pitch class sets
- Database of 60+ chord types (triads, 7ths, 9ths, 11ths, 13ths, altered, add chords)
- Automatic root detection: tries all 12 pitch classes, returns best match with score
- Inversion detection with slash notation (e.g., C/E)

**ControlButtons** (`Source/ControlButtons.cpp/.h`)
- UI control panel with toggles for keyboard/pitches/intervals/chord display
- Root key selector combo box
- Font size slider

**PluginGUI** (`Source/PluginGUI.cpp/.h`)
- Legacy/alternative GUI component (kept for reference, may not be active)

## Key Data Flow

1. MIDI note on/off events arrive in `PluginProcessor::processBlock()`
2. Processor updates `pitchClassesPresent[12]` array and `MidiKeyboardState`
3. Editor's timer callback polls `getPitchClassesPresent()` on each frame
4. Changed pitch classes trigger `ChordRecognizer::identify()`
5. Best-match chord result displayed in GUI labels

## Dependencies

- JUCE framework (included as git submodule in `JUCE/`)
- No external dependencies beyond JUCE

## Platform

macOS 11+ required (set via `CMAKE_OSX_DEPLOYMENT_TARGET`)
