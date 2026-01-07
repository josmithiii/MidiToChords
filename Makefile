# Makefile wrapper for CMake build

BUILD_DIR := build
CMAKE := cmake

.PHONY: all debug release clean distclean rebuild help

all: release

debug:
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	$(CMAKE) --build $(BUILD_DIR)

release:
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	$(CMAKE) --build $(BUILD_DIR)

clean:
	$(CMAKE) --build $(BUILD_DIR) --target clean

distclean:
	rm -rf $(BUILD_DIR)

rebuild: distclean release

help:
	@echo "Targets:"
	@echo "  all       - Build release (default)"
	@echo "  debug     - Configure and build debug"
	@echo "  release   - Configure and build release"
	@echo "  clean     - Clean build artifacts"
	@echo "  distclean - Remove entire build directory"
	@echo "  rebuild   - distclean + release"
	@echo ""
	@echo "Output:"
	@echo "  $(BUILD_DIR)/MidiToChords_artefacts/Standalone/MidiToChords.app"
	@echo "  $(BUILD_DIR)/MidiToChords_artefacts/AU/MidiToChords.component"
