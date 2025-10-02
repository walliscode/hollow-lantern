# Hollow Lantern

<!--toc:start-->

- [Hollow Lantern](#hollow-lantern)
  - [Overview](#overview)
  - [Building](#building)
  - [Testing](#testing)
  - [Qt6 GUI Development](#qt6-gui-development)
  <!--toc:end-->

## Overview

Hollow Lantern converts 3D voxel model data (MagicaVoxel .vox format) into 2D projections and triangle meshes. The project is currently being converted from a basic SFML visualization to a full-featured Qt6 GUI application.

### Features
- Reads MagicaVoxel .vox files
- Processes and hollows voxel data
- Generates triangle meshes from voxel models
- Creates 2D projections from 3D data
- Exports model data to JSON format

## Building

### Prerequisites
- CMake 3.31+
- Clang++ with libc++ (required for C++23 std::expected)
- System dependencies (Linux):
  ```bash
  sudo apt-get install -y \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
    libgl1-mesa-dev libudev-dev libfreetype-dev \
    libogg-dev libvorbis-dev libflac-dev libopenal-dev \
    libcgal-dev libc++-18-dev libc++abi-18-dev
  ```

### Build Commands
```bash
# Configure (first time)
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run tests
ctest --preset Debug
```

The project uses CMake presets (Debug and Release) configured in `CMakePresets.json`.

## Testing

Testing is carried out using the Catch2 testing framework. Each component has its own test executable for better modularity and parallel execution.

### Running Tests

```bash
# Run all tests
ctest --preset Debug

# Build and run tests in one command
cmake --workflow --preset Debug
```

Current test coverage:
- VoxReader: File loading and parsing
- VoxManipulator: Hollowing and meshing operations
- Projector: 3D to 2D projection
- Configuration: Directory path handling

All tests must pass before committing changes.

## Qt6 GUI Development

The project is being converted to a Qt6-based GUI application. See the following documents for development guidance:

### Documentation
- **[Copilot Instructions](.github/copilot-instructions.md)**: Complete development guide including:
  - Build system and dependencies
  - Code standards and architecture patterns
  - Component documentation
  - Testing guidelines
  - Common pitfalls and solutions

- **[Project Plan](PROJECT_PLAN.md)**: Detailed 7-phase implementation plan:
  1. ✅ Build System & Dependencies (Completed)
  2. Qt6 Integration Setup
  3. File Selection & Loading UI
  4. Model Statistics Display
  5. Model Visualization
  6. Integration & Polish
  7. Testing & Documentation

### Planned Features
- Runtime .vox file selection via file dialog
- Model statistics display (dimensions, voxel count, triangle count, memory usage)
- Professional Qt6 GUI with menu bar and status updates
- Progress indicators during loading/processing
- Error handling with user-friendly dialogs
- Recent files menu and keyboard shortcuts

### Current Status
Phase 1 (Build System & Dependencies) is complete:
- ✅ C++23 std::expected support with libc++
- ✅ All existing tests passing
- ✅ Comprehensive documentation
- ✅ Ready for Qt6 integration

**Next Step**: Begin Phase 2 (Qt6 Integration Setup)

## Contributing

Please follow the existing code style and architecture patterns:
- Use C++23 features (std::expected for error handling)
- Follow naming conventions (PascalCase for classes/methods, snake_case for variables)
- Add Doxygen comments for public APIs
- Write unit tests for new functionality
- Update documentation when adding components

See [Copilot Instructions](.github/copilot-instructions.md) for detailed guidelines.
