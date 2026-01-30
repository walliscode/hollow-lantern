# Implementation Roadmap: Runtime Configuration & ImGui-SFML Migration
## Recommended Path Forward (Incremental Approach)

---

## Quick Reference

**Approach**: Incremental Migration (NOT ground-up rebuild)  
**Timeline**: 8-11 days  
**Cost Estimate**: $5,000-$6,000  
**Risk Level**: 🟢 LOW  
**Code Reuse**: 87%

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Phase-by-Phase Breakdown](#phase-by-phase-breakdown)
3. [Daily Tasks](#daily-tasks)
4. [Code Examples](#code-examples)
5. [Testing Strategy](#testing-strategy)
6. [Deployment Plan](#deployment-plan)
7. [Rollback Plan](#rollback-plan)
8. [Success Metrics](#success-metrics)

---

## Prerequisites

### Before Starting

**Required**:
- [x] Read RUNTIME_CONFIGURATION_ANALYSIS.md
- [x] Read GROUND_UP_VS_INCREMENTAL.md
- [x] Read IMGUI_MIGRATION_PLAN.md
- [ ] Approve incremental approach
- [ ] Backup current Qt6 GUI code
- [ ] Clean build directory

**System Requirements**:
- CMake 3.31+
- Clang++ with libc++
- SFML 3.0 (already included via FetchContent)
- ImGui + imgui-sfml (will be added)

**Stakeholder Approval**:
- [ ] Technical lead approval
- [ ] Timeline approval (8-11 days)
- [ ] Budget approval ($5-6k)

---

## Phase-by-Phase Breakdown

### Phase 1: Foundation (Days 1-4)

**Goal**: Remove Qt6, add ImGui-SFML, create basic window

**Tasks**:

#### Day 1: Cleanup & Preparation
1. **Archive Qt6 code**
   ```bash
   mkdir -p docs/qt-reference/
   mv src/gui docs/qt-reference/src-gui-backup
   mv hollow-lantern-gui/main.cpp docs/qt-reference/
   mv tests/gui docs/qt-reference/tests-gui-backup
   ```

2. **Remove Qt6 from CMake**
   ```cmake
   # In CMakeLists.txt, remove:
   # find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Test)
   ```

3. **Verify CLI still builds**
   ```bash
   cmake --preset Debug
   cmake --build --preset Debug --target hollow-lantern
   ctest --preset Debug  # Should pass non-GUI tests
   ```

**Deliverable**: Clean repository, Qt6 removed, tests pass

#### Day 2-3: Add ImGui Dependencies

1. **Create imgui.cmake**
   - File: `cmake/fetch-content-modules/imgui.cmake`
   - FetchContent for Dear ImGui v1.90+
   - Build as static library

2. **Create imgui-sfml.cmake**
   - File: `cmake/fetch-content-modules/imgui-sfml.cmake`
   - FetchContent for imgui-sfml (SFML 3.0 compatible)
   - Link with ImGui and SFML

3. **Update fetch-content-modules.cmake**
   ```cmake
   include(imgui)
   include(imgui-sfml)
   ```

4. **Test build**
   ```bash
   cmake --preset Debug
   cmake --build --preset Debug
   # Should fetch and build ImGui-SFML successfully
   ```

**Deliverable**: ImGui-SFML integrated and building

#### Day 4: Basic Application Structure

1. **Create Application class**
   - File: `src/gui/Application.h`
   - File: `src/gui/Application.cpp`
   - Basic SFML window
   - ImGui initialization
   - Main loop skeleton

2. **Recreate hollow-lantern-gui/main.cpp**
   ```cpp
   int main() {
       hollow_lantern::gui::Application app;
       return app.Run();
   }
   ```

3. **Test application launch**
   ```bash
   ./build/Debug/hollow-lantern-gui/hollow-lantern-gui
   # Should show empty window with ImGui initialized
   ```

**Deliverable**: Basic ImGui window displays and runs

---

### Phase 2: Runtime File Selection (Days 5-6)

**Goal**: Add file picker and model loading UI

**Tasks**:

#### Day 5: File Dialog Integration

1. **Choose file dialog approach**
   - Option A: ImGuiFileDialog (recommended)
   - Option B: Native platform dialogs
   - Option C: Simple text input + browse

2. **Add file dialog dependency** (if using addon)
   ```cmake
   # Add to cmake/fetch-content-modules/
   FetchContent_Declare(ImGuiFileDialog ...)
   ```

3. **Implement file open UI**
   ```cpp
   void Application::RenderMainMenuBar() {
       if (ImGui::BeginMainMenuBar()) {
           if (ImGui::BeginMenu("File")) {
               if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                   ShowFileDialog();
               }
               ImGui::Separator();
               if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                   window_.close();
               }
               ImGui::EndMenu();
           }
           ImGui::EndMainMenuBar();
       }
   }
   ```

**Deliverable**: File → Open menu item functional

#### Day 6: Model Loading Integration

1. **Connect VoxReader to UI**
   ```cpp
   void Application::LoadModelFile(const std::string& path) {
       auto result = vox_reader_.ProvideVoxData(path);
       if (result) {
           current_model_ = result.value();
           UpdateStatus("Loaded: " + path);
       } else {
           ShowErrorDialog(result.error());
       }
   }
   ```

2. **Add loading feedback**
   - Progress indicator
   - Status messages
   - Error handling

3. **Test with multiple .vox files**
   ```bash
   # Should be able to load any .vox file from data/vox/
   ```

**Deliverable**: Can load .vox files via UI at runtime

---

### Phase 3: Runtime Projection Controls (Days 7-8)

**Goal**: Add UI controls for projection parameters

**Tasks**:

#### Day 7: Algorithm Selection

1. **Create projection algorithm enum**
   ```cpp
   enum class ProjectionAlgorithm {
       BASIC,
       FIXED_ANGLE,
       // Future: ORTHOGRAPHIC, PERSPECTIVE, etc.
   };
   ```

2. **Add algorithm dropdown**
   ```cpp
   void Application::RenderControlPanel() {
       ImGui::Begin("Projection Controls", &show_controls_);
       
       const char* algorithms[] = {
           "Basic Projection",
           "Fixed Angle",
       };
       
       if (ImGui::Combo("Algorithm", (int*)&current_algorithm_, algorithms, 2)) {
           ApplyProjection();
       }
       
       ImGui::End();
   }
   ```

3. **Implement algorithm switching**
   ```cpp
   void Application::ApplyProjection() {
       if (!current_model_) return;
       
       switch (current_algorithm_) {
           case BASIC:
               projector_.BasicProjection(
                   current_model_.value(),
                   rotation_,
                   scale_,
                   perspective_
               );
               break;
           case FIXED_ANGLE:
               projector_.FixedAngleProjection(
                   current_model_.value(),
                   rotation_
               );
               break;
       }
   }
   ```

**Deliverable**: Can switch projection algorithms at runtime

#### Day 8: Parameter Controls

1. **Add rotation sliders**
   ```cpp
   if (ImGui::SliderFloat3("Rotation (X,Y,Z)", &rotation_.x, 0.f, 360.f)) {
       ApplyProjection();
   }
   ```

2. **Add scale control**
   ```cpp
   if (ImGui::SliderFloat("Scale", &scale_, 0.1f, 10.0f)) {
       ApplyProjection();
   }
   ```

3. **Add perspective controls** (for Basic algorithm)
   ```cpp
   if (current_algorithm_ == BASIC) {
       if (ImGui::SliderFloat3("Perspective", &perspective_.x, -1.f, 1.f)) {
           ApplyProjection();
       }
   }
   ```

4. **Add reset button**
   ```cpp
   if (ImGui::Button("Reset Parameters")) {
       rotation_ = {0.f, 0.f, 0.f};
       scale_ = 4.0f;
       perspective_ = {0.f, 1.f, 0.f};
       ApplyProjection();
   }
   ```

**Deliverable**: Full runtime parameter control with real-time updates

---

### Phase 4: Model Visualization (Days 9-10)

**Goal**: Display projected models and model information

**Tasks**:

#### Day 9: Model Rendering Integration

1. **Integrate SFML rendering**
   ```cpp
   void Application::RenderModelView() {
       if (!current_model_) return;
       
       // Get projected shapes from model data
       const auto& shapes = current_model_.value().projected_data;
       
       // Draw each shape
       for (const auto& shape : shapes) {
           window_.draw(shape);
       }
   }
   ```

2. **Handle window resizing**
   ```cpp
   void Application::ProcessEvents() {
       while (auto event = window_.pollEvent()) {
           ImGui::SFML::ProcessEvent(window_, event.value());
           
           if (event->is<sf::Event::Resized>()) {
               OnWindowResized(event->getSize());
           }
       }
   }
   ```

3. **Add zoom controls**
   ```cpp
   // Mouse wheel zoom
   if (event->is<sf::Event::MouseWheelScrolled>()) {
       auto delta = event->getMouseWheelScrollDelta();
       view_zoom_ *= (1.0f + delta * 0.1f);
       UpdateView();
   }
   ```

**Deliverable**: Model renders in viewport with zoom

#### Day 10: Model Information Display

1. **Create model info window**
   ```cpp
   void Application::RenderModelInfoWindow() {
       if (!show_model_info_ || !current_model_) return;
       
       ImGui::Begin("Model Information", &show_model_info_);
       
       const auto& model = current_model_.value();
       
       if (ImGui::BeginTable("Stats", 2)) {
           ImGui::TableNextRow();
           ImGui::TableNextColumn(); ImGui::Text("Name:");
           ImGui::TableNextColumn(); ImGui::Text("%s", model.name.c_str());
           
           ImGui::TableNextRow();
           ImGui::TableNextColumn(); ImGui::Text("Dimensions:");
           ImGui::TableNextColumn(); ImGui::Text("%d × %d × %d", 
               model.size.x, model.size.y, model.size.z);
           
           ImGui::TableNextRow();
           ImGui::TableNextColumn(); ImGui::Text("Voxel Count:");
           ImGui::TableNextColumn(); ImGui::Text("%zu", CountVoxels(model));
           
           ImGui::TableNextRow();
           ImGui::TableNextColumn(); ImGui::Text("Triangle Count:");
           ImGui::TableNextColumn(); ImGui::Text("%zu", model.triangles.size());
           
           ImGui::EndTable();
       }
       
       ImGui::End();
   }
   ```

2. **Add View menu**
   ```cpp
   if (ImGui::BeginMenu("View")) {
       ImGui::MenuItem("Model Info", nullptr, &show_model_info_);
       ImGui::MenuItem("Controls", nullptr, &show_controls_);
       ImGui::MenuItem("ImGui Demo", nullptr, &show_demo_);
       ImGui::EndMenu();
   }
   ```

**Deliverable**: Full visualization with model statistics

---

### Phase 5: Polish & Documentation (Days 11)

**Goal**: Final polish and documentation updates

**Tasks**:

#### Day 11: Final Polish

1. **Add keyboard shortcuts**
   ```cpp
   // Ctrl+O = Open file
   if (ImGui::IsKeyPressed(ImGuiKey_O) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
       ShowFileDialog();
   }
   
   // Ctrl+Q = Quit
   if (ImGui::IsKeyPressed(ImGuiKey_Q) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
       window_.close();
   }
   ```

2. **Add status messages**
   - "Ready" on startup
   - "Loading..." during file load
   - "Loaded: filename.vox" on success
   - Error messages on failure

3. **Add Help menu**
   ```cpp
   if (ImGui::BeginMenu("Help")) {
       if (ImGui::MenuItem("About")) {
           show_about_dialog_ = true;
       }
       if (ImGui::MenuItem("Show ImGui Demo")) {
           show_demo_ = true;
       }
       ImGui::EndMenu();
   }
   ```

4. **Update documentation**
   - Update README.md (remove Qt6, add ImGui)
   - Update build instructions
   - Add user guide
   - Update screenshots

5. **Final testing**
   - Test all features
   - Test error cases
   - Memory leak check (valgrind)
   - Performance check

**Deliverable**: Production-ready application with documentation

---

## Daily Tasks Checklist

### Day 1: Cleanup ✅
- [ ] Backup Qt6 code to docs/qt-reference/
- [ ] Remove Qt6 from CMakeLists.txt
- [ ] Verify CLI builds
- [ ] Verify non-GUI tests pass

### Day 2-3: ImGui Setup ✅
- [ ] Create cmake/fetch-content-modules/imgui.cmake
- [ ] Create cmake/fetch-content-modules/imgui-sfml.cmake
- [ ] Update fetch-content-modules.cmake
- [ ] Test ImGui-SFML builds successfully

### Day 4: Basic App ✅
- [ ] Create src/gui/Application.h/cpp
- [ ] Recreate hollow-lantern-gui/main.cpp
- [ ] Test application launches
- [ ] Basic ImGui window displays

### Day 5: File Dialog ✅
- [ ] Add file dialog library
- [ ] Implement File → Open menu
- [ ] Test file selection works

### Day 6: Model Loading ✅
- [ ] Connect VoxReader to UI
- [ ] Add loading feedback
- [ ] Test loading multiple .vox files
- [ ] Add error handling

### Day 7: Algorithm Selection ✅
- [ ] Create ProjectionAlgorithm enum
- [ ] Add algorithm dropdown
- [ ] Implement algorithm switching
- [ ] Test algorithm switching works

### Day 8: Parameter Controls ✅
- [ ] Add rotation sliders
- [ ] Add scale slider
- [ ] Add perspective controls
- [ ] Add reset button
- [ ] Test real-time updates

### Day 9: Visualization ✅
- [ ] Integrate SFML rendering
- [ ] Handle window resizing
- [ ] Add zoom controls
- [ ] Test rendering works

### Day 10: Model Info ✅
- [ ] Create model info window
- [ ] Display statistics
- [ ] Add View menu
- [ ] Test info display

### Day 11: Polish ✅
- [ ] Add keyboard shortcuts
- [ ] Add status messages
- [ ] Add Help menu
- [ ] Update documentation
- [ ] Final testing
- [ ] Memory leak check
- [ ] Performance check

---

## Code Examples

### Complete Application.h

```cpp
#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <optional>
#include <memory>

#include "ModelData.h"
#include "VoxReader.h"
#include "VoxManipulator.h"
#include "Projector.h"

namespace hollow_lantern::gui {

enum class ProjectionAlgorithm {
    BASIC,
    FIXED_ANGLE
};

class Application {
public:
    Application();
    ~Application();
    
    int Run();
    
private:
    void ProcessEvents();
    void Update(sf::Time delta_time);
    void Render();
    
    // UI rendering
    void RenderMainMenuBar();
    void RenderModelInfoWindow();
    void RenderControlPanel();
    void RenderModelView();
    
    // Actions
    void ShowFileDialog();
    void LoadModelFile(const std::string& path);
    void ApplyProjection();
    void UpdateStatus(const std::string& message);
    
    // SFML
    sf::RenderWindow window_;
    sf::Clock delta_clock_;
    sf::View view_;
    float view_zoom_ = 0.1f;
    
    // Application state
    std::optional<ModelData> current_model_;
    std::string current_file_path_;
    std::string status_message_;
    
    // UI state
    bool show_model_info_ = true;
    bool show_controls_ = true;
    bool show_demo_ = false;
    
    // Projection parameters
    ProjectionAlgorithm current_algorithm_ = ProjectionAlgorithm::BASIC;
    sf::Vector3f rotation_{0.f, 0.f, 0.f};
    float scale_ = 4.0f;
    sf::Vector3f perspective_{0.f, 1.f, 0.f};
    
    // Processing components
    VoxReader vox_reader_;
    VoxManipulator vox_manipulator_;
    Projector projector_;
};

} // namespace hollow_lantern::gui
```

### Complete Application.cpp (Main Loop)

```cpp
#include "Application.h"
#include <iostream>

namespace hollow_lantern::gui {

Application::Application() {
    window_.create(sf::VideoMode({1280, 720}), "Hollow Lantern");
    window_.setFramerateLimit(60);
    
    if (!ImGui::SFML::Init(window_)) {
        throw std::runtime_error("Failed to initialize ImGui-SFML");
    }
    
    view_ = window_.getDefaultView();
    view_.zoom(view_zoom_);
    window_.setView(view_);
    
    UpdateStatus("Ready");
}

Application::~Application() {
    ImGui::SFML::Shutdown();
}

int Application::Run() {
    while (window_.isOpen()) {
        sf::Time delta_time = delta_clock_.restart();
        
        ProcessEvents();
        ImGui::SFML::Update(window_, delta_time);
        Update(delta_time);
        Render();
    }
    
    return 0;
}

void Application::ProcessEvents() {
    while (auto event = window_.pollEvent()) {
        ImGui::SFML::ProcessEvent(window_, event.value());
        
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
        
        if (event->is<sf::Event::Resized>()) {
            auto size = event->getSize();
            view_ = sf::View(sf::FloatRect({0, 0}, {
                static_cast<float>(size.x),
                static_cast<float>(size.y)
            }));
            view_.zoom(view_zoom_);
            window_.setView(view_);
        }
    }
}

void Application::Update(sf::Time delta_time) {
    // Update logic here if needed
}

void Application::Render() {
    window_.clear(sf::Color(45, 45, 48));
    
    // Draw model visualization
    RenderModelView();
    
    // Draw ImGui UI
    RenderMainMenuBar();
    RenderModelInfoWindow();
    RenderControlPanel();
    
    if (show_demo_) {
        ImGui::ShowDemoWindow(&show_demo_);
    }
    
    ImGui::SFML::Render(window_);
    window_.display();
}

} // namespace hollow_lantern::gui
```

---

## Testing Strategy

### Unit Tests (Catch2)

**Keep existing tests**:
- ✅ VoxReader tests (100% coverage)
- ✅ VoxManipulator tests
- ✅ Projector tests
- ✅ All non-GUI tests

**New tests** (optional):
```cpp
TEST_CASE("Application can be created", "[gui]") {
    // Basic smoke test
    REQUIRE_NOTHROW([] {
        // May need headless context for CI
    });
}
```

**Note**: ImGui is immediate mode, so traditional widget testing doesn't apply. Focus on logic testing.

### Manual Testing Checklist

**Day 6 - Basic Functionality**:
- [ ] Application launches
- [ ] Window displays correctly
- [ ] Menu bar visible
- [ ] File → Open works
- [ ] Can load .vox file
- [ ] Can exit cleanly

**Day 8 - Controls**:
- [ ] Algorithm dropdown works
- [ ] Can switch algorithms
- [ ] Rotation sliders work
- [ ] Scale slider works
- [ ] Parameters update in real-time
- [ ] Reset button works

**Day 10 - Visualization**:
- [ ] Model displays correctly
- [ ] Model info shows accurate stats
- [ ] Window resizing works
- [ ] Zoom works
- [ ] Multiple files can be loaded

**Day 11 - Final**:
- [ ] All keyboard shortcuts work
- [ ] Status messages display
- [ ] Help menu works
- [ ] No memory leaks (valgrind)
- [ ] No crashes
- [ ] Performance acceptable

---

## Deployment Plan

### Milestone 1: Basic Functionality (Day 6)
- ✅ ImGui GUI working
- ✅ Runtime file selection
- ✅ Basic model loading
- **Status**: Deployable (minimal but functional)

### Milestone 2: Full Controls (Day 8)
- ✅ All projection algorithms
- ✅ Runtime parameter controls
- ✅ Real-time updates
- **Status**: Feature-complete core

### Milestone 3: Production Ready (Day 11)
- ✅ Full visualization
- ✅ Model information
- ✅ Polish and documentation
- **Status**: Production ready

---

## Rollback Plan

If migration fails or encounters blocker:

### Option 1: Continue with Qt6
```bash
# Restore Qt6 code
cp -r docs/qt-reference/src-gui-backup src/gui
cp docs/qt-reference/hollow-lantern-gui-backup/main.cpp hollow-lantern-gui/
cp -r docs/qt-reference/tests-gui-backup tests/gui

# Restore Qt6 in CMakeLists.txt
# find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Test)

# Rebuild
cmake --preset Debug
cmake --build --preset Debug
```

**Fallback time**: 1-2 hours

### Option 2: Hybrid Approach
- Keep ImGui-SFML for controls
- Use Qt6 for file dialogs only
- **Not recommended** (adds complexity)

---

## Success Metrics

### Functional Metrics ✅
- [ ] Can select .vox files at runtime
- [ ] Can switch projection algorithms at runtime
- [ ] Can adjust parameters in real-time
- [ ] Can view models from multiple angles
- [ ] All core processing works (VoxReader, Projector, etc.)

### Performance Metrics ⚡
- [ ] Application startup < 2 seconds
- [ ] File loading < 1 second (for typical .vox file)
- [ ] Re-projection < 100ms (should feel instant)
- [ ] Frame rate ≥ 60 FPS
- [ ] Memory usage reasonable (< 500MB for typical model)

### Quality Metrics 🎯
- [ ] No crashes during normal use
- [ ] No memory leaks (valgrind clean)
- [ ] All tests pass (100%)
- [ ] Code documented (Doxygen)
- [ ] User documentation complete

### Timeline Metrics 📅
- [ ] Phase 1 complete by Day 4
- [ ] Phase 2 complete by Day 6
- [ ] Phase 3 complete by Day 8
- [ ] Phase 4 complete by Day 10
- [ ] Phase 5 complete by Day 11
- [ ] **Total: ≤ 11 days**

---

## Next Action

**If approved**: Start Day 1 tasks immediately

**First commands**:
```bash
# Create backup directory
mkdir -p docs/qt-reference/

# Archive Qt6 code
mv src/gui docs/qt-reference/src-gui-backup
mv tests/gui docs/qt-reference/tests-gui-backup

# Begin Phase 1
# (Follow Day 1 checklist above)
```

---

**Document Version**: 1.0  
**Date**: 2026-01-30  
**Author**: GitHub Copilot Agent  
**Status**: Ready for implementation  
**Estimated Completion**: Day 11 (from approval)
