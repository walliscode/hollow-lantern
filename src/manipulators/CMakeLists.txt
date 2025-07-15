add_library(manipulators
  VoxManipulator.cpp
)

target_include_directories(manipulators
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(manipulators
PRIVATE
  Catch2::Catch2WithMain
  readers
)
