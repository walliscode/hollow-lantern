add_library(readers
VoxReader.cpp
)

target_include_directories(readers
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(readers
PUBLIC
directory_paths
glm
SFML::Graphics
)
