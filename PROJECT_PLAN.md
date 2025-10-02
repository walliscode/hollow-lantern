# Hollow Lantern Qt6 GUI Conversion - Project Plan

## Executive Summary
This document outlines the plan to convert Hollow Lantern from a basic SFML-based application into a modern, user-friendly Qt6 GUI application. The conversion will enable runtime file selection, real-time feedback on model data, and provide a professional interface for working with voxel models.

## Current State Analysis

### What Works
- ✅ VoxReader successfully parses MagicaVoxel .vox files
- ✅ VoxManipulator processes and meshes voxel data
- ✅ Projector creates 2D projections from 3D voxel data
- ✅ DataExporter outputs model data to JSON format
- ✅ Unit tests pass (100% success rate)
- ✅ Build system configured with CMake presets
- ✅ C++23 features working with libc++

### Current Limitations
- ❌ File selection hardcoded in main.cpp ("colour_cube")
- ❌ No visual feedback during processing
- ❌ No display of model statistics (size, triangles, etc.)
- ❌ Basic SFML window with limited interactivity
- ❌ No error handling UI (errors only to console)
- ❌ Manual recompilation needed to load different files

## Project Goals

### Primary Objectives
1. **Runtime File Selection**: Allow users to select .vox files through a file dialog
2. **Model Statistics Display**: Show comprehensive information about loaded models
3. **Professional UI**: Replace SFML window with Qt6-based interface
4. **User Feedback**: Provide progress indicators and status messages
5. **Error Handling**: Display errors in user-friendly dialogs

### Secondary Objectives
- Recent files menu for quick access
- Save/export functionality with format options
- Keyboard shortcuts for common operations
- Responsive UI during long operations
- Help/documentation menu

## Implementation Phases

### Phase 1: Build System & Dependencies ✅ COMPLETED
**Status**: Completed  
**Tasks**:
- [x] Fix C++23 std::expected compilation issues (added libc++ support)
- [x] Verify all tests pass
- [x] Document build requirements and dependencies
- [x] Create copilot instructions document

**Duration**: Completed  
**Deliverables**: 
- Working build system with libc++
- Updated CMakeLists.txt
- Comprehensive copilot instructions

---

### Phase 2: Qt6 Integration Setup ✅ COMPLETED
**Status**: Completed  
**Duration**: Completed

**Tasks**:
- [x] Add Qt6 as a CMake dependency
  ```cmake
  find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Test)
  ```
- [x] Create new `gui/` directory under `src/`
- [x] Set up Qt6 build configuration with MOC/UIC support
- [x] Create basic QMainWindow application skeleton
- [x] Verify Qt6 application builds and runs
- [x] Create comprehensive test suite for GUI components
- [x] Configure tests for headless environments

**Dependencies**:
- Qt6 development packages (qt6-base-dev) ✅ Installed
- Qt6 OpenGL support (qt6-opengl-dev) ✅ Installed
- Qt6 Test framework ✅ Integrated

**Files Created**:
- `src/gui/MainWindow.h/cpp` - Main application window ✅
- `src/gui/CMakeLists.txt` - GUI build configuration ✅
- `hollow-lantern-gui/main.cpp` - GUI application entry point ✅
- `hollow-lantern-gui/CMakeLists.txt` - GUI application build config ✅
- `tests/gui/MainWindow.test.cpp` - Comprehensive GUI tests ✅
- `tests/gui/CMakeLists.txt` - GUI test configuration ✅

**Success Criteria**:
- ✅ Qt6 window displays successfully
- ✅ Application can be launched and closed
- ✅ Qt signals/slots infrastructure working
- ✅ Comprehensive test coverage (10 GUI tests)
- ✅ All tests passing (14/14 - 100%)

---

### Phase 3: File Selection & Loading UI
**Status**: Not Started  
**Estimated Duration**: 2-3 days

**Tasks**:
1. Implement file dialog for .vox file selection
   - Default to `data/vox/` directory
   - Filter to show only .vox files
   - Display file size before loading
2. Create loading progress dialog
   - Show during file parsing
   - Update during mesh processing
   - Cancelable operations
3. Implement error handling dialogs
   - File not found errors
   - Invalid format errors
   - Parse errors with details
4. Add status bar for operation feedback

**Files to Create**:
- `src/gui/FileDialog.h/cpp` - Custom file selection
- `src/gui/ProgressDialog.h/cpp` - Loading progress
- `src/gui/StatusBar.h/cpp` - Status information

**UI Components**:
```
MainWindow
├── Menu Bar
│   └── File
│       ├── Open...         (Ctrl+O)
│       ├── Recent Files    →
│       ├── ───────────────
│       └── Exit            (Ctrl+Q)
└── Status Bar
    └── "Ready" / "Loading..." / "Processing..."
```

**Success Criteria**:
- Users can select any .vox file at runtime
- Progress is shown during loading
- Errors are displayed in dialogs
- Status bar updates during operations

---

### Phase 4: Model Statistics Display
**Status**: Not Started  
**Estimated Duration**: 2-3 days

**Tasks**:
1. Create information panel widget
2. Extract and calculate statistics from ModelData:
   - File name and path
   - File size (bytes, KB, MB)
   - Voxel dimensions (X × Y × Z)
   - Total voxels count
   - Visible voxels count (is_visible = true)
   - Triangle count after meshing
   - Estimated memory usage
3. Format statistics for display
4. Update statistics when new model loads
5. Add copy-to-clipboard functionality

**Files to Create**:
- `src/gui/ModelInfoPanel.h/cpp` - Statistics display widget
- `src/gui/ModelStatistics.h/cpp` - Statistics calculation helper

**UI Layout**:
```
┌─────────────────────────────────────────┐
│ Model Information                       │
├─────────────────────────────────────────┤
│ Name:           colour_cube.vox         │
│ File Size:      5.0 KB                  │
│ Dimensions:     16 × 16 × 16            │
│ Total Voxels:   4096                    │
│ Visible Voxels: 896                     │
│ Triangles:      1234                    │
│ Memory:         ~2.5 MB                 │
└─────────────────────────────────────────┘
```

**Statistics to Compute**:
```cpp
struct ModelStatistics {
    std::string name;
    std::string file_path;
    size_t file_size_bytes;
    sf::Vector3i dimensions;
    size_t total_voxels;
    size_t visible_voxels;
    size_t triangle_count;
    size_t estimated_memory_bytes;
    
    // Helper methods
    std::string GetFormattedFileSize() const;
    std::string GetFormattedMemory() const;
    double GetFillPercentage() const;
};
```

**Success Criteria**:
- All statistics accurately calculated
- Information updates when model changes
- Professional formatting and layout
- Statistics persist for currently loaded model

---

### Phase 5: Model Visualization
**Status**: Not Started  
**Estimated Duration**: 3-5 days

**Tasks**:
1. Choose visualization approach:
   - **Option A**: Port SFML rendering to QPainter
   - **Option B**: Use QOpenGLWidget with existing projection
   - **Option C**: Hybrid approach with QImage from SFML data
2. Implement chosen visualization method
3. Display 2D projection in central widget
4. Maintain existing projection logic
5. Add zoom and pan controls
6. Implement view refresh on model change

**Files to Create**:
- `src/gui/ModelViewer.h/cpp` - Main visualization widget
- `src/gui/ViewerControls.h/cpp` - Zoom/pan controls
- `src/gui/RenderAdapter.h/cpp` - Bridge between backend and Qt

**Technical Decisions**:
- **Recommended**: Option B (QOpenGLWidget)
  - Best performance
  - Native Qt integration
  - Can reuse existing projection math
  - Supports future 3D interaction

**UI Layout**:
```
┌──────────────────────┬────────────────┐
│                      │ Model Info     │
│                      │                │
│   Model Viewer       │ [Statistics]   │
│   (Projection)       │                │
│                      │                │
│                      │ [View Controls]│
└──────────────────────┴────────────────┘
```

**Success Criteria**:
- Model renders accurately
- Projection matches SFML version
- Smooth interaction (zoom/pan)
- Responsive updates

---

### Phase 6: Integration & Polish
**Status**: Not Started  
**Estimated Duration**: 2-3 days

**Tasks**:
1. Connect all UI components
2. Implement recent files functionality
   - Store in QSettings
   - Display in menu (max 10)
   - Handle missing files
3. Add keyboard shortcuts
4. Implement drag-and-drop file loading
5. Add application icon
6. Create About dialog
7. Polish UI appearance
8. Add tooltips and help text
9. Implement proper shutdown handling
10. Memory leak testing

**Files to Create**:
- `src/gui/AboutDialog.h/cpp` - About window
- `src/gui/Settings.h/cpp` - Application settings manager
- `resources/icon.png` - Application icon

**Keyboard Shortcuts**:
- `Ctrl+O`: Open file
- `Ctrl+Q`: Quit application
- `Ctrl+W`: Close current model
- `Ctrl++`: Zoom in
- `Ctrl+-`: Zoom out
- `Ctrl+0`: Reset zoom
- `F1`: Show help

**Success Criteria**:
- All features work together seamlessly
- No memory leaks detected
- Professional appearance
- Intuitive user experience
- All shortcuts functional

---

### Phase 7: Testing & Documentation
**Status**: Not Started  
**Estimated Duration**: 2-3 days

**Tasks**:
1. Create unit tests for GUI components
2. Manual testing checklist
3. Update README.md with GUI usage
4. Create user documentation
5. Add inline code documentation
6. Test on different screen resolutions
7. Performance testing with large models
8. Error path testing
9. Update CMakeLists.txt documentation
10. Create developer guide

**Files to Create**:
- `tests/gui/` - GUI component tests
- `docs/USER_GUIDE.md` - End-user documentation
- `docs/DEVELOPER_GUIDE.md` - Developer documentation
- `docs/screenshots/` - UI screenshots

**Testing Checklist**:
- [ ] Load various .vox files successfully
- [ ] Handle missing files gracefully
- [ ] Handle corrupted files gracefully
- [ ] Statistics calculate correctly
- [ ] Visualization renders correctly
- [ ] UI responsive during processing
- [ ] Memory usage reasonable
- [ ] No crashes or undefined behavior
- [ ] Recent files work correctly
- [ ] Drag-and-drop works

**Success Criteria**:
- All tests pass
- Documentation complete and accurate
- No critical bugs
- Ready for user acceptance testing

---

## Technical Architecture

### New Component Structure
```
src/
├── gui/                    # Qt6 GUI components (NEW)
│   ├── MainWindow.h/cpp
│   ├── ModelViewer.h/cpp
│   ├── ModelInfoPanel.h/cpp
│   ├── FileDialog.h/cpp
│   ├── ProgressDialog.h/cpp
│   ├── AboutDialog.h/cpp
│   ├── Settings.h/cpp
│   └── CMakeLists.txt
├── readers/                # Existing - no changes needed
├── manipulators/           # Existing - no changes needed
└── structures/             # Existing - may add statistics helper
```

### Data Flow
```
User Action (File Selection)
    ↓
MainWindow::OpenFile()
    ↓
VoxReader::ProvideVoxData() → ModelData
    ↓
VoxManipulator::HollowAndMesh(ModelData&)
    ↓
Projector::BasicProjection(ModelData&)
    ↓
ModelStatistics::Calculate(ModelData&)
    ↓
┌─────────────────┬─────────────────┐
│ ModelViewer     │ ModelInfoPanel  │
│ (Render)        │ (Display Stats) │
└─────────────────┴─────────────────┘
```

### Threading Considerations
- **Main Thread**: UI updates and user interaction
- **Worker Thread**: File loading and mesh processing
  - Use QThread or QtConcurrent
  - Emit signals for progress updates
  - Return results via signal/slot
  - Handle cancellation requests

```cpp
class ModelLoader : public QThread {
    Q_OBJECT
signals:
    void progressUpdate(int percent, QString message);
    void loadComplete(ModelData data);
    void loadError(QString error);
protected:
    void run() override;
};
```

## Dependencies & Requirements

### Build Dependencies
- **Existing**:
  - CMake 3.31+
  - Clang++ with libc++
  - SFML, GLM, nlohmann/json, CGAL, Catch2
- **New**:
  - Qt6 Core (6.0+)
  - Qt6 Widgets (6.0+)
  - Qt6 OpenGLWidgets (6.0+)

### System Requirements
- **Development**:
  - Linux with X11 support
  - 4GB RAM minimum (8GB recommended)
  - C++23 compatible compiler
- **Runtime**:
  - Linux with X11 or Wayland
  - OpenGL 3.3+ support
  - 100MB disk space
  - 2GB RAM minimum

## Risk Assessment

### Technical Risks
1. **Qt6 Integration Complexity**
   - *Mitigation*: Start with simple window, iterate
   - *Impact*: Medium
   - *Probability*: Low

2. **Performance with Large Models**
   - *Mitigation*: Use threading, optimize rendering
   - *Impact*: High
   - *Probability*: Medium

3. **SFML to Qt Rendering Translation**
   - *Mitigation*: Multiple implementation options prepared
   - *Impact*: Medium
   - *Probability*: Low

### Schedule Risks
1. **Underestimated Complexity**
   - *Mitigation*: Buffer time in estimates, phased approach
   - *Impact*: Medium
   - *Probability*: Medium

2. **Dependency Issues**
   - *Mitigation*: Document all dependencies, use FetchContent where possible
   - *Impact*: Low
   - *Probability*: Low

## Success Metrics

### Must-Have (MVP)
- ✅ User can select .vox files at runtime
- ✅ Model statistics displayed accurately
- ✅ 2D projection visualization works
- ✅ Error messages shown in dialogs
- ✅ Application builds on Linux

### Should-Have (Full Release)
- ✅ Recent files menu functional
- ✅ Progress indicators during operations
- ✅ Keyboard shortcuts implemented
- ✅ Professional UI appearance
- ✅ User documentation complete

### Nice-to-Have (Future Enhancements)
- ⬜ Multiple models open simultaneously
- ⬜ Animation/rotation controls
- ⬜ Export to multiple formats
- ⬜ Drag-and-drop support
- ⬜ Theme customization

## Timeline Estimate

### Optimistic: 12-15 days
- Assumes no major blockers
- Experienced with Qt6
- All dependencies available

### Realistic: 18-22 days
- Account for learning curve
- Some dependency issues
- Normal debugging time

### Pessimistic: 25-30 days
- Major technical challenges
- Significant rework needed
- Extended testing phase

**Recommended Schedule**: 3-4 weeks with buffer time

## Maintenance Plan

### Post-Launch
1. **Monitor Issues**
   - Set up issue tracking
   - Prioritize bug fixes
   - Collect user feedback

2. **Documentation Updates**
   - Keep documentation current
   - Add FAQ based on user questions
   - Create video tutorials

3. **Future Development**
   - Implement nice-to-have features
   - Performance optimizations
   - Cross-platform support (Windows, macOS)

## Conclusion

This project plan provides a structured approach to converting Hollow Lantern into a modern Qt6 GUI application. The phased approach allows for incremental progress with clear milestones and success criteria at each stage. The existing codebase is solid and well-tested, requiring only the addition of a GUI layer without modifying the core processing logic.

**Next Steps**:
1. Review and approve this plan
2. Begin Phase 2: Qt6 Integration Setup
3. Set up development environment with Qt6
4. Create initial GUI skeleton

---

**Document Version**: 1.0  
**Last Updated**: 2024  
**Status**: Planning Complete, Ready for Implementation
