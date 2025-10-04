# GUI Component Tests

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(test_gui
    MainWindow.test.cpp
)

target_link_libraries(test_gui
    PRIVATE
        Catch2::Catch2WithMain
        gui
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
)

catch_discover_tests(test_gui)
