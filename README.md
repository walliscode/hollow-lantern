# Hollow Lantern

<!--toc:start-->

- [Hollow Lantern](#hollow-lantern)
  - [Testing](#testing) - [Running Tests](#running-tests)
  <!--toc:end-->

Converts 3D models data (currently .vox data) into 2d projections in the form of
triangles.

## Testing

Testing is carried out using the 'catch2' testing framework in conjunction with cmake.
[NOTE TO SELF] Each directory in the codebase should have its own test executable
(took me a while to figure this out), this should allow for better modularity and
allow testing to be run in parallel.

### Running Tests

Cmake build followed by a cmake test command will run the tests. For this code base,
a preset needs to be specified as set in the CMakePresets.json file. For example,
to run the tests for "Debug" configuration, you would run:

```bash
cmake --build --preset Debug
ctest --preset Debug
```
