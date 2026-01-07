# Makefile wrapper for CMake build

BUILD_DIR := build
CMAKE := cmake

.PHONY: h help doc all d debug r release c clean dc distclean rb rebuild i install

h help: doc

all: release

d debug:
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	$(CMAKE) --build $(BUILD_DIR)

r release:
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	$(CMAKE) --build $(BUILD_DIR)

c clean:
	$(CMAKE) --build $(BUILD_DIR) --target clean

dc distclean:
	rm -rf $(BUILD_DIR)

rb rebuild: distclean release

os open-standalone:
	open $(BUILD_DIR)/MidiToChords_artefacts/Release/Standalone/MidiToChords.app

i install:
	cp -R $(BUILD_DIR)/MidiToChords_artefacts/Release/AU/MidiToChords.component ~/Library/Audio/Plug-Ins/Components/

doc:
	@echo "Targets:"
	@echo "  all       - Build release (default)"
	@echo "  debug     - Configure and build debug"
	@echo "  release   - Configure and build release"
	@echo "  clean     - Clean build artifacts"
	@echo "  distclean - Remove entire build directory"
	@echo "  rebuild   - distclean + release"
	@echo "  install   - Copy AU to ~/Library/Audio/Plug-Ins/Components/"
	@echo ""
	@echo "Output:"
	@echo "  $(BUILD_DIR)/MidiToChords_artefacts/Release/Standalone/MidiToChords.app"
	@echo "  $(BUILD_DIR)/MidiToChords_artefacts/Release/AU/MidiToChords.component"
