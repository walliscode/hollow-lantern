add_library (structures INTERFACE)

target_include_directories (structures INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (structures INTERFACE
   glm
)
