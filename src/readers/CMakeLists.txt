add_library(readers
VoxReader.cpp
)

target_include_directories(readers
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)
