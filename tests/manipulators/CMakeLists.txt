add_executable(test_manipulators
VoxManipulator.test.cpp
Projector.test.cpp
)

target_link_libraries(test_manipulators
PRIVATE
  Catch2::Catch2WithMain
  readers
  manipulators
)

catch_discover_tests(test_manipulators)
