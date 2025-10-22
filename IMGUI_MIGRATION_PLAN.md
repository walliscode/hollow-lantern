# ImGui-SFML Migration Plan
## From Qt6 to imgui-sfml Integration

### Executive Summary

This document outlines the complete plan to migrate Hollow Lantern from Qt6 to imgui-sfml for the GUI framework. The project will pivot from using Qt widgets to using Dear ImGui rendered through SFML, which aligns better with the existing SFML-based rendering pipeline and provides a more lightweight, immediate-mode UI suitable for runtime parameter adjustment.

---

## Why imgui-sfml?

### Current State
- **hollow-lantern** (CLI): Uses SFML 3.0 for visualization
- **hollow-lantern-gui**: Uses Qt6 with QMainWindow architecture
- Split codebase with two different GUI approaches

### Problems with Current Approach
1. **Heavy dependency**: Qt6 is a large framework for relatively simple UI needs
2. **Dual graphics systems**: SFML for rendering + Qt for UI creates complexity
3. **Signal/slot overhead**: Retained mode UI adds boilerplate for simple interactions
4. **Build complexity**: Qt MOC/UIC preprocessing required
5. **Not optimized for runtime parameters**: Qt better suited for traditional desktop apps

### Benefits of imgui-sfml
1. **Single graphics stack**: SFML handles both rendering and UI
2. **Immediate mode**: Simpler state management, easier runtime parameter tuning
3. **Lightweight**: Much smaller than Qt, faster compile times
4. **Native integration**: imgui-sfml designed specifically for SFML
5. **Perfect for tools**: ImGui is industry standard for game/graphics tool UIs
6. **Already popular**: Used by developers worldwide for similar applications

---

## Architecture Comparison

### Current Qt Architecture
```
QApplication
└── MainWindow (QMainWindow)
    ├── Menu Bar (QMenuBar)
    │   └── File Menu (QMenu)
    │       ├── Open Action (QAction) → QFileDialog
    │       └── Exit Action (QAction)
    ├── Central Widget (QWidget)
    │   └── [Planned: Model viewer]
    └── Status Bar (QStatusBar)

Signals/Slots:
- FileOpenRequested(QString)
- ExitRequested()
- UpdateStatus(QString)
```

### Proposed imgui-sfml Architecture
```
SFML RenderWindow (main loop)
├── Event handling (SFML + ImGui)
├── ImGui::SFML::Update()
└── Rendering
    ├── ImGui UI Elements
    │   ├── Main Menu Bar (ImGui::BeginMainMenuBar)
    │   │   └── File Menu
    │   │       ├── Open... → Native file dialog or ImGui file browser
    │   │       └── Exit
    │   ├── Model Info Window (ImGui::Begin("Model Info"))
    │   │   └── Statistics display
    │   └── Control Panel (ImGui::Begin("Controls"))
    │       └── Runtime parameters
    └── Model Visualization (SFML draw calls)
        └── Existing projection rendering
```

**Key Differences:**
- **No signal/slot system**: Direct function calls
- **Immediate mode**: UI rebuilt every frame
- **Integrated rendering**: ImGui draws to same SFML target
- **Single event loop**: SFML handles everything

---

## Detailed Migration Steps

### Phase 1: Preparation & Cleanup ⚠️ FIRST STEP

**Goal**: Remove Qt dependencies and prepare for imgui-sfml

**Tasks:**
1. **Document current Qt functionality**
   - Screenshot existing Qt GUI (if built)
   - Document all features (menu items, signals, behavior)
   - Save as reference in `docs/qt-reference/`

2. **Remove Qt from build system**
   - Remove `find_package(Qt6 ...)` from `/CMakeLists.txt`
   - Remove Qt-specific CMake variables (CMAKE_AUTOMOC, etc.)
   - Remove Qt6 dependencies from all CMakeLists.txt files

3. **Archive Qt code**
   - Move `/src/gui/` to `/docs/qt-reference/src-gui-backup/`
   - Move `/hollow-lantern-gui/` content to `/docs/qt-reference/hollow-lantern-gui-backup/`
   - Move `/tests/gui/` to `/docs/qt-reference/tests-gui-backup/`
   - Keep backups for reference but remove from build

4. **Clean build artifacts**
   - Remove `build/` directory entirely
   - Clear CMake cache
   - Prepare for fresh build

**Deliverables:**
- [ ] Qt6 completely removed from CMake configuration
- [ ] Qt code backed up and removed from active source tree
- [ ] Clean repository ready for imgui-sfml

**Validation:**
- CMake configuration should succeed without Qt6
- `hollow-lantern` (CLI) should still build successfully
- All non-GUI tests should pass

---

### Phase 2: Add imgui-sfml Dependencies

**Goal**: Integrate ImGui and imgui-sfml through CMake FetchContent

**Tasks:**
1. **Create imgui FetchContent module**
   - File: `/cmake/fetch-content-modules/imgui.cmake`
   - Fetch Dear ImGui from official repository
   - Use stable release tag (v1.90.0 or later)

2. **Create imgui-sfml FetchContent module**
   - File: `/cmake/fetch-content-modules/imgui-sfml.cmake`
   - Fetch from https://github.com/SFML/imgui-sfml
   - Configure for SFML 3.0 compatibility
   - Link with ImGui and SFML

3. **Update fetch-content-modules.cmake**
   - Add `include(imgui)`
   - Add `include(imgui-sfml)`
   - Ensure proper dependency order

4. **Update root CMakeLists.txt**
   - Verify SFML is configured correctly
   - No need to find_package (using FetchContent)

**Example imgui.cmake:**
```cmake
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.0
    GIT_SHALLOW ON
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    
    # ImGui doesn't have CMakeLists, create interface library
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
    )
    
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    target_compile_features(imgui PUBLIC cxx_std_23)
endif()
```

**Example imgui-sfml.cmake:**
```cmake
include(FetchContent)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG master  # Check for latest stable tag
    GIT_SHALLOW ON
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

FetchContent_MakeAvailable(imgui-sfml)
```

**Deliverables:**
- [ ] imgui.cmake created and working
- [ ] imgui-sfml.cmake created and working
- [ ] Dependencies fetch successfully
- [ ] ImGui-SFML builds with SFML 3.0

**Validation:**
- `cmake --preset Debug` succeeds
- ImGui and ImGui-SFML targets available
- No build errors

---

### Phase 3: Create New GUI Structure

**Goal**: Rebuild GUI application using SFML + ImGui

**Tasks:**
1. **Recreate /src/gui/ directory structure**
   - Keep namespace: `hollow_lantern::gui`
   - Use C++23 features consistently
   - Follow existing code style

2. **Create Application.h/cpp**
   - File: `/src/gui/Application.h`
   - File: `/src/gui/Application.cpp`
   - Main application class managing SFML window
   - ImGui context initialization
   - Main rendering loop

3. **Create GuiState.h**
   - File: `/src/gui/GuiState.h`
   - Simple struct holding UI state
   - No need for complex state machine (immediate mode)
   - Contains: file paths, model data pointers, UI flags

4. **Create ModelViewer.h/cpp**
   - File: `/src/gui/ModelViewer.h`
   - File: `/src/gui/ModelViewer.cpp`
   - Handles SFML rendering of projections
   - Wraps existing projection display logic
   - Renders under ImGui UI

5. **Create UiComponents.h/cpp**
   - File: `/src/gui/UiComponents.h`
   - File: `/src/gui/UiComponents.cpp`
   - Reusable ImGui components
   - Functions for: menu bar, file dialog, stats window, controls

6. **Update /src/gui/CMakeLists.txt**
   - Link with ImGui-SFML
   - Link with existing readers/manipulators
   - No MOC/UIC needed

**Example Application.h skeleton:**
```cpp
#pragma once

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>
#include <optional>

#include "ModelData.h"
#include "VoxReader.h"
#include "VoxManipulator.h"
#include "Projector.h"

namespace hollow_lantern::gui {

/// @brief Main GUI application class
class Application {
public:
    Application();
    ~Application();
    
    /// @brief Run the application main loop
    /// @return Exit code
    int Run();
    
private:
    void ProcessEvents();
    void Update(sf::Time delta_time);
    void Render();
    
    void RenderMainMenuBar();
    void RenderModelInfoWindow();
    void RenderControlPanel();
    void RenderModelView();
    
    void LoadModelFile(const std::string& filename);
    void ShowFileOpenDialog();
    
    // SFML Window
    sf::RenderWindow window_;
    sf::Clock delta_clock_;
    
    // Application state
    std::optional<ModelData> current_model_;
    std::string current_file_path_;
    bool show_model_info_ = true;
    bool show_controls_ = true;
    bool show_demo_window_ = false;
    
    // Processing components
    VoxReader vox_reader_;
    VoxManipulator vox_manipulator_;
    Projector projector_;
};

} // namespace hollow_lantern::gui
```

**Deliverables:**
- [ ] Application class created
- [ ] Basic ImGui window structure
- [ ] SFML + ImGui initialization working
- [ ] Clean separation of concerns

**Validation:**
- GUI library builds successfully
- Can create Application instance
- Links properly with dependencies

---

### Phase 4: Implement Core GUI Features

**Goal**: Implement file loading and model display

**Tasks:**
1. **File selection UI**
   - ImGui menu bar with File → Open
   - Option A: Use native file dialog (platform-specific)
   - Option B: Use ImGui file browser addon
   - Option C: Simple text input + browse button
   - **Recommended**: Start with text input, add browser later

2. **Model loading integration**
   - Connect UI to VoxReader
   - Show loading progress (if possible)
   - Handle errors with ImGui popups
   - Display success/failure status

3. **Statistics display**
   - ImGui window showing model info
   - Format statistics nicely
   - Use ImGui tables for clean layout
   - Real-time updates

4. **Model visualization**
   - Render SFML projections
   - ImGui overlay doesn't interfere
   - Handle window resizing
   - Maintain aspect ratio

5. **Runtime controls**
   - Projection angle sliders (ImGui::SliderFloat)
   - Zoom controls
   - Rotation parameters
   - Real-time updates to projection

**Example UI code:**
```cpp
void Application::RenderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                ShowFileOpenDialog();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                window_.close();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Model Info", nullptr, &show_model_info_);
            ImGui::MenuItem("Controls", nullptr, &show_controls_);
            ImGui::MenuItem("ImGui Demo", nullptr, &show_demo_window_);
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Application::RenderModelInfoWindow() {
    if (!show_model_info_ || !current_model_) return;
    
    ImGui::Begin("Model Information", &show_model_info_);
    
    if (ImGui::BeginTable("ModelStats", 2)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Name:");
        ImGui::TableNextColumn(); ImGui::Text("%s", current_model_->name.c_str());
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Dimensions:");
        ImGui::TableNextColumn(); ImGui::Text("%d × %d × %d", 
            current_model_->size.x,
            current_model_->size.y,
            current_model_->size.z);
        
        // ... more stats
        
        ImGui::EndTable();
    }
    
    ImGui::End();
}
```

**Deliverables:**
- [ ] File open functionality working
- [ ] Model statistics displayed
- [ ] Projection visualization integrated
- [ ] Runtime controls implemented

**Validation:**
- Can load .vox files from UI
- Statistics display correctly
- Model renders properly
- Controls affect visualization in real-time

---

### Phase 5: Rebuild Application Entry Point

**Goal**: Create new hollow-lantern-gui executable

**Tasks:**
1. **Update /hollow-lantern-gui/main.cpp**
   - Remove Qt includes
   - Create Application instance
   - Run application
   - Minimal error handling

2. **Update /hollow-lantern-gui/CMakeLists.txt**
   - Remove Qt MOC/UIC
   - Link with gui library
   - Link with ImGui-SFML
   - Link with SFML::Graphics

3. **Configure as GUI application**
   - Set proper executable type
   - No console window (on Windows: WIN32)
   - Set application icon (future)

**Example main.cpp:**
```cpp
/// @file main.cpp
/// @brief Entry point for Hollow Lantern ImGui GUI application

#include "gui/Application.h"
#include <iostream>
#include <exception>

int main() {
    try {
        hollow_lantern::gui::Application app;
        return app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        return 1;
    }
}
```

**Deliverables:**
- [ ] hollow-lantern-gui builds successfully
- [ ] Application launches and runs
- [ ] Window displays with ImGui UI
- [ ] Can exit cleanly

**Validation:**
- `cmake --build --preset Debug` succeeds
- `./build/Debug/hollow-lantern-gui/hollow-lantern-gui` runs
- No crashes or memory leaks
- UI is responsive

---

### Phase 6: Testing Strategy

**Goal**: Ensure quality without over-testing immediate mode UI

**Important Note**: ImGui is immediate mode, so traditional widget testing doesn't apply. Focus on:
- Integration testing
- Functional testing
- Manual testing

**Tasks:**
1. **Simplify test approach**
   - Don't test ImGui rendering (already tested by ImGui)
   - Test application logic and state management
   - Test integration with VoxReader/Manipulator/Projector

2. **Create basic tests**
   - File: `/tests/gui/Application.test.cpp`
   - Test application creation
   - Test model loading logic
   - Test error handling

3. **Manual testing checklist**
   - Create checklist document
   - Cover all UI features
   - Document expected behavior
   - Include edge cases

4. **Update test configuration**
   - File: `/tests/gui/CMakeLists.txt`
   - Remove Qt Test dependency
   - Use Catch2 for basic tests
   - May need headless SFML context

**Example test structure:**
```cpp
#include <catch2/catch_test_macros.hpp>
#include "gui/Application.h"

TEST_CASE("Application can be created", "[gui]") {
    // Note: May need to mock SFML window for headless testing
    // Or skip GUI tests in CI environment
    
    SECTION("Construction succeeds") {
        REQUIRE_NOTHROW([] {
            // Application app;  // May need special setup
        });
    }
}

// Focus more on logic than UI
TEST_CASE("Model loading logic", "[gui][logic]") {
    // Test the underlying logic separate from UI
}
```

**Manual Testing Checklist:**
- [ ] Application launches successfully
- [ ] Window displays at correct size
- [ ] ImGui menu bar visible and functional
- [ ] File → Open works
- [ ] Can load .vox file from data/vox/
- [ ] Model statistics display correctly
- [ ] Model renders in viewport
- [ ] Runtime controls update visualization
- [ ] Can change projection angles
- [ ] Zoom controls work
- [ ] Window can be resized
- [ ] Can close windows and reopen them
- [ ] Exit works cleanly
- [ ] No memory leaks (valgrind)

**Deliverables:**
- [ ] Basic unit tests created
- [ ] Manual test checklist documented
- [ ] Testing approach validated

**Validation:**
- Tests compile and run
- Manual testing checklist complete
- No crashes during testing

---

### Phase 7: Documentation Updates

**Goal**: Update all documentation to reflect imgui-sfml approach

**Tasks:**
1. **Update README.md**
   - Remove Qt6 from dependencies
   - Add ImGui-SFML to dependencies
   - Update build instructions
   - Update feature list
   - Remove Qt-specific notes

2. **Update PROJECT_PLAN.md**
   - Mark Qt phases as "Superseded"
   - Add new phases for ImGui implementation
   - Update architecture diagrams
   - Revise timeline

3. **Update copilot-instructions.md**
   - Remove Qt conventions and patterns
   - Add ImGui patterns and best practices
   - Update component structure
   - Add ImGui-specific guidelines

4. **Create IMGUI_GUIDELINES.md**
   - ImGui best practices
   - Common patterns in codebase
   - How to add new UI features
   - Debugging tips

5. **Update inline documentation**
   - Doxygen comments in new files
   - Code examples
   - Usage notes

**Deliverables:**
- [ ] README.md updated
- [ ] PROJECT_PLAN.md revised
- [ ] copilot-instructions.md updated
- [ ] IMGUI_GUIDELINES.md created
- [ ] All code documented

**Validation:**
- Documentation is accurate
- Build instructions work
- Examples are correct

---

## Technical Specifications

### ImGui-SFML Integration Details

**Initialization:**
```cpp
// In Application constructor
window_.create(sf::VideoMode({1280, 720}), "Hollow Lantern");
window_.setFramerateLimit(60);

if (!ImGui::SFML::Init(window_)) {
    throw std::runtime_error("Failed to initialize ImGui-SFML");
}
```

**Main Loop:**
```cpp
int Application::Run() {
    while (window_.isOpen()) {
        sf::Time delta_time = delta_clock_.restart();
        
        ProcessEvents();
        ImGui::SFML::Update(window_, delta_time);
        Update(delta_time);
        Render();
    }
    
    ImGui::SFML::Shutdown();
    return 0;
}
```

**Event Handling:**
```cpp
void Application::ProcessEvents() {
    while (auto event = window_.pollEvent()) {
        ImGui::SFML::ProcessEvent(window_, event.value());
        
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
        
        // Handle other events...
    }
}
```

**Rendering:**
```cpp
void Application::Render() {
    window_.clear(sf::Color(45, 45, 48));  // Dark gray background
    
    // Draw model visualization (SFML)
    RenderModelView();
    
    // Draw ImGui UI (overlay)
    RenderMainMenuBar();
    RenderModelInfoWindow();
    RenderControlPanel();
    
    ImGui::SFML::Render(window_);
    window_.display();
}
```

### File Structure After Migration

```
hollow-lantern/
├── CMakeLists.txt                  (Qt6 removed, imgui-sfml added)
├── cmake/
│   └── fetch-content-modules/
│       ├── imgui.cmake             (NEW)
│       ├── imgui-sfml.cmake        (NEW)
│       ├── SFML.cmake              (existing)
│       └── fetch-content-modules.cmake (updated)
├── src/
│   ├── gui/                        (REBUILT)
│   │   ├── Application.h/cpp       (NEW - main app class)
│   │   ├── GuiState.h              (NEW - UI state)
│   │   ├── ModelViewer.h/cpp       (NEW - visualization)
│   │   ├── UiComponents.h/cpp      (NEW - reusable widgets)
│   │   └── CMakeLists.txt          (updated for imgui-sfml)
│   ├── readers/                    (unchanged)
│   ├── manipulators/               (unchanged)
│   └── structures/                 (unchanged)
├── hollow-lantern-gui/
│   ├── main.cpp                    (REWRITTEN - no Qt)
│   └── CMakeLists.txt              (updated for imgui-sfml)
├── tests/
│   └── gui/
│       ├── Application.test.cpp    (NEW - simplified tests)
│       └── CMakeLists.txt          (updated - no Qt Test)
├── docs/
│   ├── IMGUI_GUIDELINES.md         (NEW)
│   └── qt-reference/               (NEW - archived Qt code)
│       ├── src-gui-backup/
│       ├── hollow-lantern-gui-backup/
│       └── tests-gui-backup/
└── README.md                       (updated)
```

---

## Migration Checklist

### Pre-Migration
- [ ] Build current Qt version successfully
- [ ] Run all tests (should pass)
- [ ] Screenshot Qt GUI for reference
- [ ] Document all Qt features
- [ ] Backup important files

### Phase 1: Cleanup
- [ ] Remove Qt from CMakeLists.txt
- [ ] Archive Qt source code
- [ ] Clean build directory
- [ ] Verify CLI app still builds

### Phase 2: Dependencies
- [ ] Create imgui.cmake
- [ ] Create imgui-sfml.cmake
- [ ] Update fetch-content-modules.cmake
- [ ] Test dependency fetch
- [ ] Verify ImGui-SFML builds

### Phase 3: Structure
- [ ] Create Application.h/cpp
- [ ] Create GuiState.h
- [ ] Create ModelViewer.h/cpp
- [ ] Create UiComponents.h/cpp
- [ ] Update gui/CMakeLists.txt
- [ ] Verify library builds

### Phase 4: Features
- [ ] Implement main menu bar
- [ ] Implement file open dialog
- [ ] Implement model loading
- [ ] Implement statistics window
- [ ] Implement model visualization
- [ ] Implement runtime controls
- [ ] Test all features

### Phase 5: Entry Point
- [ ] Rewrite hollow-lantern-gui/main.cpp
- [ ] Update hollow-lantern-gui/CMakeLists.txt
- [ ] Build GUI executable
- [ ] Test application launch
- [ ] Verify functionality

### Phase 6: Testing
- [ ] Create basic unit tests
- [ ] Create manual test checklist
- [ ] Run all tests
- [ ] Perform manual testing
- [ ] Fix any issues

### Phase 7: Documentation
- [ ] Update README.md
- [ ] Update PROJECT_PLAN.md
- [ ] Update copilot-instructions.md
- [ ] Create IMGUI_GUIDELINES.md
- [ ] Document new code
- [ ] Review all documentation

### Post-Migration
- [ ] Full build test
- [ ] Full test suite run
- [ ] Manual feature verification
- [ ] Performance check
- [ ] Memory leak check
- [ ] Final documentation review

---

## Risk Assessment

### Technical Risks

**Risk 1: SFML 3.0 Compatibility**
- **Description**: imgui-sfml may not fully support SFML 3.0
- **Impact**: High
- **Probability**: Medium
- **Mitigation**: 
  - Check imgui-sfml releases for SFML 3.0 support
  - May need to use specific branch or fork
  - Fallback: temporarily use SFML 2.6

**Risk 2: ImGui Learning Curve**
- **Description**: Immediate mode UI is different from retained mode
- **Impact**: Low
- **Probability**: Medium
- **Mitigation**:
  - ImGui is well-documented
  - Many examples available
  - Built-in demo window for reference

**Risk 3: File Dialog Limitations**
- **Description**: ImGui doesn't have native file dialogs
- **Impact**: Medium
- **Probability**: High
- **Mitigation**:
  - Use imgui file browser addon
  - Or use platform-specific native dialogs
  - Or simple text input initially

**Risk 4: Testing Immediate Mode UI**
- **Description**: Traditional widget testing doesn't apply
- **Impact**: Low
- **Probability**: High
- **Mitigation**:
  - Focus on logic testing
  - Rely more on manual testing
  - This is acceptable for this project

### Schedule Risks

**Risk 1: Underestimated Complexity**
- **Impact**: Medium
- **Probability**: Low
- **Mitigation**: Phased approach allows for adjustment

**Risk 2: Dependency Issues**
- **Impact**: Medium
- **Probability**: Low
- **Mitigation**: FetchContent handles most issues automatically

---

## Expected Outcomes

### Benefits
1. **Lighter weight**: No Qt dependency, faster builds
2. **Unified graphics**: Single SFML stack
3. **Better for tools**: ImGui designed for this use case
4. **Easier to modify**: Immediate mode simplifies changes
5. **Runtime parameters**: Perfect for adjusting values on the fly
6. **Industry standard**: ImGui used by many game/graphics tools

### Trade-offs
1. **Less polish**: ImGui looks more "tool-like" than Qt
2. **Manual layout**: Need to code UI layout manually
3. **File dialogs**: No built-in native file dialogs
4. **Learning curve**: Different paradigm from Qt

### Success Criteria
- [ ] Application builds without Qt
- [ ] All core features working (file open, display, stats)
- [ ] Runtime parameter adjustment functional
- [ ] Performance equal or better than before
- [ ] Code is maintainable and well-documented

---

## Timeline Estimate

**Optimistic**: 3-4 days
- Experienced with ImGui
- No major blockers
- Smooth dependency integration

**Realistic**: 5-7 days
- Learning ImGui patterns
- Some trial and error
- Normal debugging

**Pessimistic**: 10-12 days
- SFML 3.0 compatibility issues
- Significant rework needed
- Extended testing phase

**Recommended**: Plan for 1 week with buffer

---

## Conclusion

This migration from Qt6 to imgui-sfml aligns the GUI approach with the existing SFML rendering pipeline and provides a more suitable framework for a tool-style application with runtime parameter adjustment. The immediate mode paradigm of ImGui simplifies state management and makes the codebase more maintainable.

The phased approach ensures we can validate each step before proceeding, and the comprehensive documentation will make the codebase accessible to future developers.

**Status**: Planning complete, awaiting approval to begin implementation.

---

**Document Version**: 1.0  
**Date**: 2025-10-22  
**Author**: Copilot Code Agent
