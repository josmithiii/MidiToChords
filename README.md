# MidiToChords

A JUCE-based MIDI plugin that displays chord names from MIDI input in real-time.

## Features

- **Comprehensive chord recognition** - Identifies 60+ chord types including:
  - Triads: major, minor, diminished, augmented, sus2, sus4
  - 7th chords: maj7, m7, 7, dim7, m7b5, mMaj7, aug7, 7sus
  - Extended: 9th, 11th, 13th chords and their variants
  - Altered: 7b5, 7#5, 7b9, 7#9, and more
  - Add chords: add9, add11, madd9

- **Automatic root detection** - Tries all 12 pitch classes to find the best match
- **Inversion detection** - Shows slash chord notation (e.g., C/E)
- **Visual keyboard** - Displays currently held notes
- **Interval display** - Shows the intervals from the root

## Build Requirements

- CMake 3.16+
- C++17 compiler
- macOS 11+ (for macOS builds)

## Building

```bash
# Clone with submodules
git clone --recursive https://github.com/YOUR_USERNAME/MidiToChords.git
cd MidiToChords

# Build
cmake -B build
cmake --build build

# The plugin will be in:
# build/MidiToChords_artefacts/Standalone/MidiToChords.app
# build/MidiToChords_artefacts/AU/MidiToChords.component
```

If you already cloned without `--recursive`:
```bash
git submodule update --init --recursive
```

## Usage

1. **Standalone**: Run `MidiToChords.app` directly and select your MIDI input device
2. **AU Plugin**: Load `MidiToChords.component` as a MIDI effect in your DAW (Logic Pro, GarageBand, etc.)

## License

This project uses [JUCE](https://juce.com/), which has its own licensing terms.

## Author

Julius O. Smith III - [CCRMA, Stanford University](https://ccrma.stanford.edu/~jos/)
