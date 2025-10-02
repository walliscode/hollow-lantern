# Copilot Instructions for Hollow Lantern Qt6 GUI Development

## Project Overview
Hollow Lantern is a C++23 application that processes 3D voxel data (from .vox files) and converts it into 2D projections. The project currently has a basic SFML-based visualization and needs to be converted into a full-featured Qt6 GUI application.

## Build System
- **CMake** (minimum version 3.31)
- **Compiler**: Clang++ with libc++ (required for C++23 std::expected support)
- **Build Presets**: Debug and Release configurations available via CMakePresets.json
- **Build Commands**:
  ```bash
  cmake --preset Debug
  cmake --build --preset Debug
  ctest --preset Debug
  ```

## Code Standards

### Language & Style
- **C++ Standard**: C++23 (strictly enforced)
- **Compiler Requirements**: Must use Clang with libc++ for std::expected support
- **Naming Conventions**:
  - Classes: PascalCase (e.g., `VoxReader`, `ModelData`)
  - Functions/Methods: PascalCase (e.g., `ProvideVoxData`, `HollowAndMesh`)
  - Variables: snake_case (e.g., `model_data`, `voxel_data`)
  - Constants: UPPER_SNAKE_CASE
- **Documentation**: Use Doxygen-style comments with `///` for public APIs
- **Headers**: Include guards should use `#pragma once`

### Architecture Patterns
- **Error Handling**: Use `std::expected<T, std::string>` for operations that can fail
- **File I/O**: Use binary streams for .vox file parsing
- **Data Structures**: 
  - `ModelData` struct holds all voxel and projection data
  - Separate readers, manipulators, and structures into different namespaces/directories

## Project Structure

```
hollow-lantern/
├── config/              # Build configuration (directory paths)
├── data/
│   ├── vox/            # Input .vox files
│   └── export/         # JSON output files
├── src/
│   ├── readers/        # VoxReader, DataExporter
│   ├── manipulators/   # VoxManipulator, Projector
│   └── structures/     # ModelData, Voxel, Triangle structs
├── hollow-lantern/     # Main application
├── tests/              # Catch2 unit tests
└── CMakeLists.txt      # Root build configuration
```

## Key Components

### VoxReader
- **Location**: `src/readers/VoxReader.h/cpp`
- **Purpose**: Reads MagicaVoxel .vox files and extracts voxel data
- **Key Method**: `ProvideVoxData(std::string model_name, bool testing = false)`
- **Returns**: `std::expected<ModelData, std::string>`
- **File Format**: Handles VOX format with MAIN, SIZE, XYZI, and RGBA chunks

### ModelData Structure
- **Location**: `src/structures/ModelData.h`
- **Contains**:
  - `name`: Model filename
  - `size`: 3D dimensions (sf::Vector3i)
  - `voxel_data`: 3D vector of Voxel structs
  - `projected_data`: 2D vertex arrays for visualization
  - `triangle_data`: Processed triangle data
  - `triangles`: Vector of Triangle structs
  - `masks`: 6 directional masks for face culling

### VoxManipulator
- **Location**: `src/manipulators/VoxManipulator.h/cpp`
- **Purpose**: Processes voxel data (hollowing, meshing)
- **Key Method**: `HollowAndMesh(ModelData& model_data)`

### Projector
- **Location**: `src/manipulators/Projector.h/cpp`
- **Purpose**: Projects 3D voxel data to 2D representations
- **Key Methods**:
  - `BasicProjection(ModelData& model_data, ...)`
  - `FixedAngleProjection(ModelData& model_data, ...)`

## Dependencies

### Required Libraries
- **SFML** (currently used): Graphics, System modules
- **GLM**: Vector math operations
- **nlohmann/json**: JSON export functionality
- **CGAL**: Computational geometry (required)
- **Catch2**: Unit testing framework
- **Qt6** (to be added): Widgets, Core modules for GUI

### System Dependencies
- X11 libraries (Linux): libx11-dev, libxrandr-dev, libxcursor-dev, libxi-dev
- OpenGL: libgl1-mesa-dev
- Audio (SFML): libogg-dev, libvorbis-dev, libflac-dev, libopenal-dev
- Graphics: libfreetype-dev, libudev-dev

## Qt6 Integration Goals

### File Selection & Loading
- Implement `QFileDialog` for selecting .vox files at runtime
- Support browsing to data/vox/ directory by default
- Display file information before loading (file size, name)

### Model Information Display
- **Post-load Statistics**:
  - Model name
  - File size (bytes)
  - Voxel dimensions (width × height × depth)
  - Number of voxels (total and visible)
  - Number of triangles after meshing
  - Memory usage estimate
  
### Visualization
- Display 2D projection of the model (using existing projection logic)
- Consider using QOpenGLWidget or custom QPainter rendering
- Preserve current rotation/projection capabilities

### UI Components to Implement
- Main window with menu bar
- File menu: Open, Recent Files, Exit
- Status bar showing current file and processing status
- Central widget: Model viewer
- Side panel: Model statistics and information
- Progress indicators for long operations (loading, processing)

## Testing Guidelines
- Write unit tests using Catch2 framework
- Place tests in corresponding subdirectories under `tests/`
- Test file naming: `<Component>.test.cpp`
- Run tests before committing: `ctest --preset Debug`
- Maintain 100% test pass rate

## Common Pitfalls & Solutions

### Build Issues
- **std::expected not found**: Ensure libc++ is being used (check CMakeLists.txt)
- **SFML dependencies missing**: Install required system libraries
- **CMake cache issues**: Delete `build/` directory and reconfigure

### File Loading
- .vox files must be in `data/vox/` or `data/test_data/vox/` (for tests)
- File paths are configured in `config/directory_paths.h`
- Use `testing=true` parameter in tests to use test data directory

### Memory Management
- `ModelData` contains large 3D vectors - be mindful of copies
- Prefer passing by reference where possible
- Use move semantics for large data transfers

## Future Enhancements (Beyond Initial Qt6 Conversion)
- Support for multiple models simultaneously
- Animation/rotation controls
- Export to various 2D/3D formats
- Color palette editing
- Batch processing capabilities
- Drag-and-drop file loading
- Thumbnail previews in file selector

## Contributing
- Follow existing code style and patterns
- Add Doxygen comments for all public APIs
- Write tests for new functionality
- Update this document when adding new components
- Keep CMakeLists.txt organized by component

## Additional Resources
- MagicaVoxel format: https://github.com/ephtracy/voxel-model
- Qt6 Documentation: https://doc.qt.io/qt-6/
- C++23 Features: https://en.cppreference.com/w/cpp/23
