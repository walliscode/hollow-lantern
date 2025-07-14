add_executable(config-tests
  directory_paths.test.cpp
)

target_link_libraries(config-tests
  PRIVATE
  Catch2::Catch2WithMain
  directory_paths
)

catch_discover_tests(config-tests)
