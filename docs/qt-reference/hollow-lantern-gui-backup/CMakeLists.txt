# Hollow Lantern Qt6 GUI Application

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(hollow-lantern-gui
    main.cpp
)

target_link_libraries(hollow-lantern-gui
    PRIVATE
        gui
        Qt6::Core
        Qt6::Widgets
)

# Ensure C++23 standard
target_compile_features(hollow-lantern-gui PRIVATE cxx_std_23)
