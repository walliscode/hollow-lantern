# Qt6 Code Archive

This directory contains the archived Qt6 GUI implementation that was replaced during the migration to imgui-sfml.

## Contents

### src-gui-backup/
Original GUI library components:
- `MainWindow.h` - Main window class definition
- `MainWindow.cpp` - Main window implementation
- `CMakeLists.txt` - GUI library build configuration

### hollow-lantern-gui-backup/
Original GUI application entry point:
- `main.cpp` - Application entry point
- `CMakeLists.txt` - GUI executable build configuration

### tests-gui-backup/
Original GUI test files:
- `MainWindow.test.cpp` - Qt Test-based unit tests
- `CMakeLists.txt` - GUI test build configuration

## Documentation

See `QT_FUNCTIONALITY_REFERENCE.md` for detailed documentation of the archived Qt functionality.

## Purpose

These files are preserved for reference during and after the migration to imgui-sfml. They document the original implementation and can serve as a reference for feature parity verification.

## Migration Date

**Archived**: 2025-10-22
**Migration**: Phase 1 - Preparation & Cleanup
**Reason**: Migration from Qt6 to imgui-sfml framework

## Important Notes

- These files are **not part of the active build** and should not be included in CMakeLists.txt
- They are kept for historical reference only
- The new imgui-sfml implementation will provide equivalent functionality with a different architecture
- Do not attempt to restore these files without understanding the complete migration context

## See Also

- `/IMGUI_MIGRATION_PLAN.md` - Complete migration plan and rationale
- Root `CMakeLists.txt` - Qt6 dependencies removed
- `/src/gui/` - New imgui-sfml GUI implementation (created in Phase 3)
