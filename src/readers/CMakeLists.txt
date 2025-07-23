add_library(readers
VoxReader.cpp
DataExporter.cpp
)

target_include_directories(readers
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(readers
PUBLIC
directory_paths
structures
glm
SFML::Graphics
nlohmann_json::nlohmann_json
)
