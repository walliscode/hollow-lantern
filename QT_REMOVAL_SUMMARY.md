# Qt6 Removal Summary

## Overview
This document provides a quick reference for all Qt-related components that need to be removed from the Hollow Lantern project.

---

## Files to Remove

### Source Files (Qt GUI)
```
/src/gui/MainWindow.h           - QMainWindow-based main window
/src/gui/MainWindow.cpp         - Implementation
/src/gui/CMakeLists.txt         - Build config with Qt MOC/UIC
```

**Recommended Action**: Archive to `/docs/qt-reference/src-gui-backup/` before deletion

### Application Files
```
/hollow-lantern-gui/main.cpp    - QApplication entry point
/hollow-lantern-gui/CMakeLists.txt - Build config with Qt linking
```

**Recommended Action**: Archive to `/docs/qt-reference/hollow-lantern-gui-backup/` before deletion

### Test Files
```
/tests/gui/MainWindow.test.cpp  - Qt widget tests
/tests/gui/CMakeLists.txt       - Test config with Qt Test
```

**Recommended Action**: Archive to `/docs/qt-reference/tests-gui-backup/` before deletion

---

## CMake Changes Required

### Root CMakeLists.txt (`/CMakeLists.txt`)

**Remove:**
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Test)
```

**Line**: 42

### GUI Library CMakeLists (`/src/gui/CMakeLists.txt`)

**Remove:**
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

target_link_libraries(gui
    PUBLIC
        Qt6::Core
        Qt6::Widgets
)
```

**Replace with**: imgui-sfml linking (see IMGUI_MIGRATION_PLAN.md)

### GUI Application CMakeLists (`/hollow-lantern-gui/CMakeLists.txt`)

**Remove:**
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

target_link_libraries(hollow-lantern-gui
    PRIVATE
        gui
        Qt6::Core
        Qt6::Widgets
)
```

**Replace with**: ImGui-SFML linking

### GUI Tests CMakeLists (`/tests/gui/CMakeLists.txt`)

**Remove:**
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

target_link_libraries(test_gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
)
```

**Replace with**: Simplified test configuration

---

## Qt Classes Used

### Current Qt Dependencies
- `QApplication` - Application event loop
- `QMainWindow` - Main window base class
- `QWidget` - Base widget class
- `QMenuBar` - Menu bar
- `QMenu` - Menu
- `QAction` - Menu action
- `QStatusBar` - Status bar
- `QLabel` - Text label
- `QFileDialog` - File selection dialog
- `QMessageBox` - Message dialog (planned)
- `QSignalSpy` - Test utility
- `QTest` - Test framework
- `QKeySequence` - Keyboard shortcuts

### Signal/Slot Connections
The following Qt signals need alternative implementations:
- `FileOpenRequested(const QString&)` → Direct function call
- `ExitRequested()` → Direct function call
- `triggered` (from QAction) → ImGui menu item selection

---

## Build System Dependencies

### To Remove from Documentation

**README.md** - Remove from prerequisites:
```markdown
- Qt6 (6.4+) for GUI application
- qt6-base-dev qt6-base-dev-tools libqt6opengl6-dev
```

**Build commands** - Remove Qt-specific commands:
```bash
# No longer needed:
sudo apt-get install qt6-base-dev qt6-base-dev-tools libqt6opengl6-dev
```

---

## Feature Mapping: Qt → ImGui

### Menu Bar
**Qt:**
```cpp
QMenuBar* menuBar();
QMenu* addMenu("&File");
QAction* action = new QAction("&Open...", this);
action->setShortcut(QKeySequence::Open);
```

**ImGui:**
```cpp
if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            // Action here
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}
```

### Status Bar
**Qt:**
```cpp
QStatusBar* statusBar();
statusBar()->showMessage("Ready", 3000);
```

**ImGui:**
```cpp
// Can use text at bottom of window
ImGui::SetCursorPosY(window_height - 20);
ImGui::Text("Ready");
// Or use a dedicated status window
```

### File Dialog
**Qt:**
```cpp
QString file = QFileDialog::getOpenFileName(
    this,
    "Open VOX File",
    "",
    "VOX Files (*.vox);;All Files (*)"
);
```

**ImGui:**
```cpp
// Option 1: Use imgui file browser addon
// Option 2: Platform-specific native dialog
// Option 3: Simple text input
ImGui::InputText("File Path", file_path_buffer, 256);
if (ImGui::Button("Browse...")) {
    // Open dialog
}
```

### Signals/Slots
**Qt:**
```cpp
connect(action, &QAction::triggered, this, &MainWindow::OnOpenFile);
emit FileOpenRequested(file_path);
```

**ImGui:**
```cpp
// Direct function calls - no signals needed
if (ImGui::MenuItem("Open...")) {
    OnOpenFile();
}

// State changes happen immediately
if (file_selected) {
    LoadFile(file_path);
}
```

---

## Testing Changes

### Qt Test Framework → Catch2

**Qt Test:**
```cpp
#include <QTest>
#include <QSignalSpy>

QSignalSpy spy(&window, &MainWindow::FileOpenRequested);
REQUIRE(spy.isValid());
exit_action->trigger();
REQUIRE(spy.count() == 1);
```

**Catch2 + Logic Testing:**
```cpp
#include <catch2/catch_test_macros.hpp>

// Test logic, not UI rendering
TEST_CASE("File loading works") {
    std::string file = "test.vox";
    auto result = LoadModelFile(file);
    REQUIRE(result.has_value());
}
```

### Headless Testing
- Qt: Offscreen platform plugin (`-platform offscreen`)
- ImGui: May need to mock SFML window or skip GUI tests in CI

---

## Migration Warnings

### What NOT to Remove
- SFML includes and linking (needed for imgui-sfml)
- VoxReader, VoxManipulator, Projector classes (backend unchanged)
- ModelData structure (core data structure)
- Existing tests for readers/manipulators (keep as-is)

### What to Archive (Not Delete)
- All Qt GUI code should be moved to `/docs/qt-reference/` as a backup
- This allows reference during migration and potential future use

### Build System Gotchas
1. **CMake cache**: After removing Qt, delete `build/` entirely
2. **Link order**: Ensure imgui-sfml comes before SFML in link list
3. **Include paths**: ImGui needs proper include directory setup
4. **C++ standard**: Maintain C++23 requirement throughout

---

## Validation Checklist

After removing Qt, verify:
- [ ] `cmake --preset Debug` succeeds without Qt6 errors
- [ ] `hollow-lantern` (CLI) still builds successfully
- [ ] Non-GUI tests still pass (VoxReader, Manipulator, etc.)
- [ ] No remaining Qt includes in source files
- [ ] No Qt linker errors
- [ ] Documentation updated (README, etc.)

---

## Quick Command Reference

### Archive Qt Code
```bash
mkdir -p docs/qt-reference
mv src/gui docs/qt-reference/src-gui-backup
mv hollow-lantern-gui docs/qt-reference/hollow-lantern-gui-backup
mv tests/gui docs/qt-reference/tests-gui-backup
```

### Clean Build
```bash
rm -rf build/
cmake --preset Debug
cmake --build --preset Debug
```

### Verify Non-GUI Components
```bash
ctest --preset Debug -R "test_vox_reader|test_vox_manipulator|test_projector|test_config"
```

---

## Additional Resources

- **ImGui Documentation**: https://github.com/ocornut/imgui
- **ImGui-SFML Documentation**: https://github.com/SFML/imgui-sfml
- **ImGui Demo**: Built into ImGui, shows all widgets and features
- **Migration Plan**: See `IMGUI_MIGRATION_PLAN.md` for detailed steps

---

**Last Updated**: 2025-10-22  
**Status**: Reference document - do not implement until approved
