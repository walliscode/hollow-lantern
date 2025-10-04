# Hollow Lantern Testing Guide

## Overview

This document provides comprehensive guidance for testing the Hollow Lantern project. The project uses the Catch2 testing framework for both backend components and Qt6 GUI components.

## Test Organization

### Directory Structure
```
tests/
├── CMakeLists.txt           # Test configuration root
├── config/                  # Configuration tests
│   ├── CMakeLists.txt
│   └── directory_paths.test.cpp
├── readers/                 # Data reader tests
│   ├── CMakeLists.txt
│   └── VoxReader.test.cpp
├── manipulators/            # Data manipulation tests
│   ├── CMakeLists.txt
│   ├── VoxManipulator.test.cpp
│   └── Projector.test.cpp
└── gui/                     # GUI component tests
    ├── CMakeLists.txt
    └── MainWindow.test.cpp
```

## Running Tests

### All Tests
```bash
# Run all tests
ctest --preset Debug

# Run tests with verbose output
ctest --preset Debug --verbose

# Run tests with output on failure
ctest --preset Debug --output-on-failure
```

### Specific Test Suites
```bash
# Run only config tests
./build/Debug/tests/config/config-tests

# Run only reader tests
./build/Debug/tests/readers/test_readers

# Run only manipulator tests
./build/Debug/tests/manipulators/test_manipulators

# Run only GUI tests
./build/Debug/tests/gui/test_gui
```

### Specific Test Cases
```bash
# Run a specific test by name
./build/Debug/tests/gui/test_gui "MainWindow can be constructed"

# Run tests matching a tag
./build/Debug/tests/gui/test_gui "[mainwindow]"

# Run tests with multiple tags
./build/Debug/tests/gui/test_gui "[gui][statusbar]"
```

## Test Coverage

### Backend Tests (4 tests)

#### Configuration Tests
- **Test**: Directory paths in configuration
- **File**: `tests/config/directory_paths.test.cpp`
- **Coverage**: Verifies VOX and export directory paths

#### Reader Tests
- **Test**: VoxReader provides VoxData object
- **File**: `tests/readers/VoxReader.test.cpp`
- **Coverage**: 
  - VOX file parsing
  - Model data extraction
  - Error handling for missing files
  - Color palette handling

#### Manipulator Tests
- **Test**: VoxManipulator provides VoxData object
- **File**: `tests/manipulators/VoxManipulator.test.cpp`
- **Coverage**:
  - Voxel hollowing algorithm
  - Triangle mesh generation
  - Face mask creation
  - Visibility calculations

- **Test**: Projector projects 3D models onto 2D planes
- **File**: `tests/manipulators/Projector.test.cpp`
- **Coverage**:
  - 3D to 2D projection
  - Projection data generation

### GUI Tests (10 tests)

#### MainWindow Tests
- **File**: `tests/gui/MainWindow.test.cpp`
- **Test Cases**:
  1. **Construction**: MainWindow can be constructed
     - Verifies window creation
     - Checks initial size (800×600)
     - Validates window title
  
  2. **Menu Bar**: MainWindow has menu bar
     - Menu bar exists
     - File menu exists
     - Open action with Ctrl+O shortcut
     - Exit action with platform Quit shortcut
  
  3. **Status Bar**: MainWindow has status bar
     - Status bar exists
     - Initial "Ready" message displays
  
  4. **Signals**: MainWindow signals work correctly
     - FileOpenRequested signal
     - ExitRequested signal
  
  5. **Actions**: MainWindow actions trigger correctly
     - Exit action emits ExitRequested
  
  6. **Keyboard Shortcuts**: MainWindow keyboard shortcuts work
     - Open action has Ctrl+O
     - Exit action has platform Quit shortcut
  
  7. **Visibility**: MainWindow can be shown and hidden
     - Window starts hidden
     - Window can be shown
     - Window can be hidden after showing
  
  8. **Memory Management**: MainWindow memory management
     - Multiple create/destroy cycles
     - Clean destruction
  
  9. **Properties**: MainWindow has correct window properties
     - Correct window title
     - Is a QMainWindow instance
  
  10. **Components**: MainWindow UI components are accessible
      - Menu bar accessible
      - Status bar accessible
      - Valid geometry

## Writing New Tests

### Test Structure

#### Backend Tests
```cpp
#include <catch2/catch_test_macros.hpp>
#include "YourComponent.h"

TEST_CASE("Component does something", "[tag]") {
    // Arrange
    YourComponent component;
    
    // Act
    auto result = component.DoSomething();
    
    // Assert
    REQUIRE(result.has_value());
    CHECK(result.value() == expected_value);
}

TEST_CASE("Component handles errors", "[tag][error]") {
    SECTION("Invalid input") {
        YourComponent component;
        auto result = component.DoSomething("invalid");
        REQUIRE(!result.has_value());
        CHECK(result.error() == "Expected error message");
    }
}
```

#### GUI Tests
```cpp
#include <catch2/catch_test_macros.hpp>
#include <QApplication>
#include <QSignalSpy>
#include "YourWidget.h"

// Qt application instance (required for GUI tests)
int argc = 1;
char arg0[] = "test";
char arg1[] = "-platform";
char arg2[] = "offscreen";
char* argv[] = {arg0, arg1, arg2, nullptr};
static QApplication* test_app = nullptr;

void EnsureQApplication() {
    if (!test_app) {
        argc = 3;
        test_app = new QApplication(argc, argv);
    }
}

TEST_CASE("Widget does something", "[gui][widget]") {
    EnsureQApplication();
    
    YourWidget widget;
    QSignalSpy spy(&widget, &YourWidget::SomeSignal);
    
    widget.DoSomething();
    
    REQUIRE(spy.count() == 1);
}
```

### Best Practices

1. **Use Descriptive Names**
   - Test names should clearly describe what is being tested
   - Tags should be consistent across related tests

2. **Arrange-Act-Assert Pattern**
   - **Arrange**: Set up test data and conditions
   - **Act**: Execute the code being tested
   - **Assert**: Verify the results

3. **Test One Thing**
   - Each test should verify a single behavior
   - Use SECTION for related variations

4. **Use Appropriate Assertions**
   - `REQUIRE`: Test stops if this fails (critical checks)
   - `CHECK`: Test continues if this fails (multiple checks)
   - `REQUIRE_THROWS`: Verify exceptions are thrown
   - `REQUIRE_NOTHROW`: Verify no exceptions

5. **GUI Test Considerations**
   - Always use offscreen platform for headless testing
   - Process events when needed: `QTest::qWait(10)`
   - Use QSignalSpy to verify signals
   - Clean up widgets properly

6. **Test Data**
   - Use test data from `data/test_data/vox/`
   - Don't rely on external resources
   - Use the `testing=true` parameter where available

## Continuous Integration

### Build and Test Pipeline
```bash
# Full workflow
cmake --preset Debug
cmake --build --preset Debug
ctest --preset Debug
```

### Expected Results
- **All tests must pass**: 100% success rate
- **No memory leaks**: Verified by memory management tests
- **No crashes**: All tests complete successfully

## Debugging Failed Tests

### View Test Output
```bash
# Run with verbose output
ctest --preset Debug --verbose

# Run specific test with output
./build/Debug/tests/gui/test_gui "Test Name" --success
```

### Common Issues

#### GUI Tests Fail with "Cannot connect to display"
**Solution**: Ensure `-platform offscreen` is used in QApplication initialization

#### Tests Fail to Find Data Files
**Solution**: Check that `testing=true` parameter is used and test data exists in `data/test_data/vox/`

#### Memory Leaks
**Solution**: Verify proper cleanup in destructors and use smart pointers

#### Flaky Tests
**Solution**: Add appropriate waits with `QTest::qWait()` for GUI operations

## Test Metrics

### Current Status
- **Total Tests**: 14
- **Pass Rate**: 100%
- **Coverage Areas**:
  - Configuration: 1 test
  - Readers: 1 test
  - Manipulators: 2 tests
  - GUI: 10 tests

### Performance Targets
- Individual tests: < 1 second
- Full test suite: < 5 seconds
- No memory leaks
- No crashes

## Adding New Components

When adding new components, follow these steps:

1. **Create Test File**
   ```bash
   touch tests/your_component/YourComponent.test.cpp
   ```

2. **Update CMakeLists.txt**
   ```cmake
   add_executable(test_your_component
       YourComponent.test.cpp
   )
   
   target_link_libraries(test_your_component
       PRIVATE
           Catch2::Catch2WithMain
           your_component
   )
   
   catch_discover_tests(test_your_component)
   ```

3. **Write Tests**
   - Start with basic construction tests
   - Add functionality tests
   - Add error handling tests
   - Add edge case tests

4. **Verify**
   ```bash
   cmake --build --preset Debug
   ctest --preset Debug
   ```

## Resources

- **Catch2 Documentation**: https://github.com/catchorg/Catch2
- **Qt Test Framework**: https://doc.qt.io/qt-6/qtest.html
- **C++23 Reference**: https://en.cppreference.com/w/cpp/23

## Maintenance

- Review and update tests when features change
- Add tests for new features before implementation (TDD)
- Keep test data minimal and focused
- Document complex test scenarios
- Maintain 100% test pass rate
