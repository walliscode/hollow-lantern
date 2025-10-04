# GUI Components Library

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(gui
    MainWindow.h
    MainWindow.cpp
)

target_link_libraries(gui
    PUBLIC
        Qt6::Core
        Qt6::Widgets
)

target_include_directories(gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Ensure C++23 standard
target_compile_features(gui PUBLIC cxx_std_23)
