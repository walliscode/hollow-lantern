# Qt6 GUI Functionality Reference

## Overview
This document captures the functionality of the Hollow Lantern Qt6 GUI before migration to imgui-sfml.

**Date Archived**: 2025-10-22
**Version**: 0.1.0

---

## Application Structure

### Main Window (MainWindow class)
- **Base Class**: QMainWindow
- **Window Title**: "Hollow Lantern"
- **Default Size**: 800x600 pixels
- **Architecture**: Qt signal/slot system

---

## Features Implemented

### 1. Menu Bar

#### File Menu
- **Open... (Ctrl+O)**
  - Opens QFileDialog for .vox file selection
  - File filter: "VOX Files (*.vox);;All Files (*)"
  - Status tip: "Open a .vox file"
  - Emits signal: `FileOpenRequested(QString filePath)`
  - Updates status bar with selected file path

- **Exit (Ctrl+Q)**
  - Closes the application
  - Status tip: "Exit the application"
  - Emits signal: `ExitRequested()`

### 2. Status Bar
- Displays application status messages
- Shows temporary messages for 3 seconds
- Initial status: "Ready"
- Updates during file operations:
  - "Opening file..." - when file dialog opens
  - "Selected: [file_path]" - when file selected
  - "Ready" - when operation cancelled

### 3. Signals and Slots

#### Signals
```cpp
void FileOpenRequested(const QString& filePath);
void ExitRequested();
```

#### Slots
```cpp
void OnOpenFile();
void OnExit();
void UpdateStatus(const QString& message);
```

#### Connections
- `open_file_action_->triggered` → `MainWindow::OnOpenFile`
- `exit_action_->triggered` → `MainWindow::OnExit`

---

## Component Details

### MainWindow.h
- Header file defining the main window class
- Uses Q_OBJECT macro for meta-object system
- Inherits from QMainWindow
- Contains:
  - Menu actions (QAction*)
  - Status bar label (QLabel*)
  - Signal declarations
  - Slot declarations
  - Setup methods

### MainWindow.cpp
- Implementation of MainWindow class
- Constructor initializes window and UI components
- Methods:
  - `SetupMenuBar()` - Creates File menu with actions
  - `SetupStatusBar()` - Creates status bar with label
  - `SetupConnections()` - Connects signals to slots
  - `OnOpenFile()` - Handles file open dialog
  - `OnExit()` - Handles application exit
  - `UpdateStatus()` - Updates status bar message

### main.cpp
- Entry point for Qt6 GUI application
- Creates QApplication instance
- Sets application metadata:
  - Application name: "Hollow Lantern"
  - Application version: "0.1.0"
  - Organization name: "Hollow Lantern Project"
- Creates and shows MainWindow
- Runs Qt event loop with `app.exec()`
- Exception handling for fatal errors

---

## Build Configuration

### CMake Settings (src/gui/CMakeLists.txt)
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(gui
    MainWindow.h
    MainWindow.cpp
)

target_link_libraries(gui
    PUBLIC
        Qt6::Core
        Qt6::Widgets
)
```

### CMake Settings (hollow-lantern-gui/CMakeLists.txt)
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(hollow-lantern-gui
    main.cpp
)

target_link_libraries(hollow-lantern-gui
    PRIVATE
        gui
        Qt6::Core
        Qt6::Widgets
)
```

### Root CMakeLists.txt Qt Dependencies
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Test)
```

---

## Testing

### Test File: tests/gui/MainWindow.test.cpp
- Uses Qt Test framework
- Tests MainWindow creation, signals, slots, and behavior
- **Note**: Test details preserved in backup

### Test CMake Configuration
```cmake
# See backed up file for details
```

---

## Screenshots

**Note**: Screenshots could not be captured as GUI was not built in this environment. The application displays:
- Standard Qt application window
- Menu bar at top with "File" menu
- Empty central widget area (planned for model viewer)
- Status bar at bottom with status messages

---

## Features Not Yet Implemented

The following were planned but not implemented in the Qt version:
- Model visualization in central widget
- Model information display/statistics window
- Runtime parameter controls
- Actual .vox file loading and processing integration
- Model rendering viewport

---

## Migration Notes

### Equivalent imgui-sfml Features

| Qt Feature | imgui-sfml Equivalent |
|------------|----------------------|
| QMainWindow | sf::RenderWindow + ImGui::Begin() |
| QMenuBar | ImGui::BeginMainMenuBar() |
| QMenu | ImGui::BeginMenu() |
| QAction | ImGui::MenuItem() |
| QFileDialog | Native dialog or imgui file browser |
| QStatusBar | ImGui window or text overlay |
| Signals/Slots | Direct function calls |
| QKeySequence | ImGui shortcut system |

### Key Differences
- **Immediate Mode**: UI rebuilt every frame vs retained mode
- **No MOC**: No meta-object compiler needed
- **Direct Calls**: No signal/slot indirection
- **Single Loop**: SFML event loop handles everything
- **Lightweight**: Much smaller dependency footprint

---

## References

- Original source files backed up in `/docs/qt-reference/src-gui-backup/`
- Application entry point backed up in `/docs/qt-reference/hollow-lantern-gui-backup/`
- Test files backed up in `/docs/qt-reference/tests-gui-backup/`
