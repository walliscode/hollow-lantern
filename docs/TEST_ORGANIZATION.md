# Test Organization and Structure

## Overview

The Hollow Lantern project has a comprehensive and well-organized test suite using Catch2. This document describes the current test organization, rationale, and guidelines for maintaining test quality.

## Test Architecture

### Modular Test Structure

Each component has its own test executable, allowing for:
- **Parallel execution**: Tests can run independently
- **Focused debugging**: Issues isolated to specific components
- **Clear boundaries**: Test scope matches component scope
- **Fast iteration**: Rebuild only affected tests

### Test Executables

```
build/Debug/tests/
├── config/
│   └── config-tests              # Configuration tests
├── readers/
│   └── test_readers              # File reading tests
├── manipulators/
│   └── test_manipulators         # Data manipulation tests
└── gui/
    └── test_gui                  # GUI component tests
```

## Test Categories

### 1. Unit Tests
**Purpose**: Test individual components in isolation

**Examples**:
- `VoxReader::ProvideVoxData()` parsing logic
- `VoxManipulator::HollowAndMesh()` algorithms
- `MainWindow` construction and properties

**Characteristics**:
- Fast execution (< 1 second per test)
- No external dependencies
- Deterministic results
- Use test data fixtures

### 2. Integration Tests
**Purpose**: Test component interactions

**Examples**:
- VoxReader → VoxManipulator data flow
- MainWindow menu → action → signal emission

**Characteristics**:
- Test multiple components
- Verify data contracts
- Check error propagation

### 3. GUI Tests
**Purpose**: Test Qt6 GUI components

**Examples**:
- Window creation and display
- Menu bar and actions
- Signal/slot connections
- User interaction simulation

**Characteristics**:
- Use offscreen platform for headless testing
- Verify Qt object hierarchy
- Test event handling
- Check memory management

## Test Quality Standards

### Code Coverage
- **Current**: Core components covered
- **Target**: 80%+ code coverage for new components
- **Priority Areas**:
  - Error handling paths
  - Public APIs
  - GUI interactions

### Test Characteristics

#### Good Tests Are:
1. **Fast**: Complete in < 1 second
2. **Isolated**: No dependencies between tests
3. **Repeatable**: Same results every time
4. **Self-Validating**: Clear pass/fail
5. **Timely**: Written with/before code

#### Tests Should:
- Have clear, descriptive names
- Test one behavior per test case
- Use appropriate assertions (REQUIRE vs CHECK)
- Include edge cases and error conditions
- Be maintainable and readable

### Test Naming Convention

```cpp
TEST_CASE("Component action description", "[tag1][tag2]") {
    SECTION("Specific scenario") {
        // Test implementation
    }
}
```

**Examples**:
- `TEST_CASE("MainWindow can be constructed", "[gui][mainwindow]")`
- `TEST_CASE("VoxReader handles missing files", "[readers][error]")`

### Tag System

Tags organize tests by:
- **Component**: `[gui]`, `[readers]`, `[manipulators]`, `[config]`
- **Feature**: `[mainwindow]`, `[menubar]`, `[statusbar]`
- **Type**: `[error]`, `[memory]`, `[performance]`

## Test Data Management

### Test Data Location
```
data/
├── vox/              # Production data
└── test_data/
    └── vox/          # Test-specific data
        └── colour_cube.vox
```

### Test Data Guidelines
1. **Keep test files small**: < 1 MB
2. **Use representative data**: Cover edge cases
3. **Version control**: Commit test data
4. **Document format**: Add README if complex

## Running Tests

### Quick Reference
```bash
# All tests
ctest --preset Debug

# Specific suite
./build/Debug/tests/gui/test_gui

# Specific test
./build/Debug/tests/gui/test_gui "MainWindow can be constructed"

# With tags
./build/Debug/tests/gui/test_gui "[mainwindow]"
```

### Performance Monitoring
```bash
# Show test timing
ctest --preset Debug --verbose | grep "Test "

# Expected results:
# Test #1: < 0.01 sec
# Test #2: < 0.01 sec
# ...
# Total: < 1 sec
```

## Test Maintenance

### When to Update Tests

1. **Feature Changes**
   - Update affected tests immediately
   - Add tests for new behavior
   - Remove obsolete tests

2. **Bug Fixes**
   - Add test reproducing bug
   - Verify fix with test
   - Keep test for regression prevention

3. **Refactoring**
   - Tests should still pass
   - Update only if API changes
   - Maintain test coverage

### Test Review Checklist

Before committing:
- [ ] All tests pass (100%)
- [ ] New features have tests
- [ ] Test names are descriptive
- [ ] No commented-out tests
- [ ] No debug output in tests
- [ ] Tests run in < 5 seconds total

## GUI Testing Specifics

### Headless Testing

GUI tests must work without a display:

```cpp
// ✅ Correct: Use offscreen platform
int argc = 3;
char arg0[] = "test";
char arg1[] = "-platform";
char arg2[] = "offscreen";
char* argv[] = {arg0, arg1, arg2, nullptr};
QApplication app(argc, argv);

// ❌ Wrong: Assumes display available
QApplication app(argc, argv);  // Fails in CI
```

### Qt Test Integration

```cpp
#include <QTest>           // For QTest::qWait()
#include <QSignalSpy>      // For signal verification

// Wait for events
QTest::qWait(10);

// Verify signals
QSignalSpy spy(&widget, &Widget::SignalName);
widget.trigger();
REQUIRE(spy.count() == 1);
```

### Memory Management

```cpp
TEST_CASE("No memory leaks", "[memory]") {
    // Create and destroy multiple times
    for (int i = 0; i < 100; ++i) {
        auto* widget = new YourWidget();
        delete widget;
    }
    // If no crash, memory management is correct
    REQUIRE(true);
}
```

## Continuous Integration

### CI Pipeline
```yaml
1. Install dependencies (Qt6, Catch2, etc.)
2. Configure build: cmake --preset Debug
3. Build: cmake --build --preset Debug
4. Test: ctest --preset Debug
5. Report: Must show 100% pass rate
```

### Failure Response
1. CI fails → Investigation required
2. No commits until tests pass
3. Fix or skip failing test temporarily
4. Document known issues

## Future Improvements

### Planned Enhancements
1. **Code coverage reporting**: lcov/gcov integration
2. **Performance benchmarks**: Track test execution time
3. **Mocking framework**: For complex dependencies
4. **Property-based testing**: For algorithmic code
5. **Visual regression tests**: For GUI appearance

### Additional Test Areas
- **Phase 3**: File dialog tests
- **Phase 4**: Model statistics tests
- **Phase 5**: Visualization tests
- **Phase 6**: Integration tests
- **Phase 7**: End-to-end tests

## Best Practices Summary

### Do:
- ✅ Write tests first (TDD)
- ✅ Keep tests simple and focused
- ✅ Use descriptive names
- ✅ Test error paths
- ✅ Run tests before commit
- ✅ Maintain 100% pass rate

### Don't:
- ❌ Skip tests in commits
- ❌ Ignore failing tests
- ❌ Write interdependent tests
- ❌ Use sleep() for timing
- ❌ Test implementation details
- ❌ Commit test data to production paths

## Metrics and Goals

### Current Metrics (Phase 2)
- **Total Tests**: 14
- **Pass Rate**: 100%
- **Execution Time**: ~0.3 seconds
- **Test/Code Ratio**: ~1:3 (good)
- **Coverage**: Core components

### Phase 2 Achievements
- ✅ Established GUI testing infrastructure
- ✅ Configured headless testing
- ✅ Created comprehensive MainWindow tests
- ✅ Documented testing patterns
- ✅ Achieved 100% pass rate

### Future Goals
- **Phase 3**: Add file dialog tests (3-5 tests)
- **Phase 4**: Add statistics tests (5-7 tests)
- **Phase 5**: Add visualization tests (8-10 tests)
- **Phase 6**: Add integration tests (10-15 tests)
- **Phase 7**: Add end-to-end tests (5-10 tests)
- **Target**: 50+ tests by completion

## References

- [TESTING_GUIDE.md](./TESTING_GUIDE.md) - Detailed testing guide
- [Catch2 Documentation](https://github.com/catchorg/Catch2/tree/devel/docs)
- [Qt Test Tutorial](https://doc.qt.io/qt-6/qtest-tutorial.html)
- [C++ Testing Best Practices](https://github.com/cpp-best-practices/cppbestpractices)

## Conclusion

The test organization is designed to be:
- **Scalable**: Easy to add new tests
- **Maintainable**: Clear structure and naming
- **Comprehensive**: Multiple test types
- **Fast**: Quick feedback cycles
- **Reliable**: Consistent results

Following these guidelines ensures high-quality, maintainable tests that support rapid development and confident refactoring.
