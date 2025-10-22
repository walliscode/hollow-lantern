/// @file main.cpp
/// @brief Entry point for Hollow Lantern Qt6 GUI application

#include <QApplication>
#include "../src/gui/MainWindow.h"

#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Create Qt application
        QApplication app(argc, argv);
        
        // Set application metadata
        QApplication::setApplicationName("Hollow Lantern");
        QApplication::setApplicationVersion("0.1.0");
        QApplication::setOrganizationName("Hollow Lantern Project");
        
        // Create and show main window
        MainWindow main_window;
        main_window.show();
        
        // Run application event loop
        return app.exec();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        return 1;
    }
}
