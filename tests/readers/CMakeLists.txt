add_executable(test_readers
VoxReader.test.cpp
)

target_link_libraries(test_readers
PRIVATE
  Catch2::Catch2WithMain
  readers
  directory_paths
)

catch_discover_tests(test_readers)
