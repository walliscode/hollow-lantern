/// @file MainWindow.test.cpp
/// @brief Comprehensive tests for MainWindow Qt6 GUI component

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../../src/gui/MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QSignalSpy>
#include <QTest>

// Create QApplication instance for GUI tests
// This is needed for any Qt GUI testing
int argc = 1;
char arg0[] = "test_gui";
char arg1[] = "-platform";
char arg2[] = "offscreen";
char* argv[] = {arg0, arg1, arg2, nullptr};
static QApplication* test_app = nullptr;

// Helper to ensure QApplication exists
void EnsureQApplication() {
    if (!test_app) {
        argc = 3;  // Use offscreen platform for headless testing
        test_app = new QApplication(argc, argv);
    }
}

TEST_CASE("MainWindow can be constructed", "[gui][mainwindow]") {
    EnsureQApplication();
    
    SECTION("Default constructor creates valid window") {
        MainWindow window;
        
        // Window should be created successfully
        REQUIRE(window.windowTitle() == "Hollow Lantern");
        REQUIRE(window.width() > 0);
        REQUIRE(window.height() > 0);
    }
    
    SECTION("Window has correct initial size") {
        MainWindow window;
        
        REQUIRE(window.width() == 800);
        REQUIRE(window.height() == 600);
    }
}

TEST_CASE("MainWindow has menu bar", "[gui][mainwindow][menubar]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Menu bar exists") {
        QMenuBar* menu_bar = window.menuBar();
        REQUIRE(menu_bar != nullptr);
    }
    
    SECTION("File menu exists") {
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> actions = menu_bar->actions();
        
        REQUIRE(!actions.isEmpty());
        REQUIRE(actions[0]->text().contains("File"));
    }
    
    SECTION("File menu has Open action") {
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> menu_actions = menu_bar->actions();
        QMenu* file_menu = qobject_cast<QMenu*>(menu_actions[0]->menu());
        
        REQUIRE(file_menu != nullptr);
        QList<QAction*> file_actions = file_menu->actions();
        
        bool has_open = false;
        for (QAction* action : file_actions) {
            if (action->text().contains("Open")) {
                has_open = true;
                // Check that Open action has keyboard shortcut
                REQUIRE(!action->shortcut().isEmpty());
                break;
            }
        }
        REQUIRE(has_open);
    }
    
    SECTION("File menu has Exit action") {
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> menu_actions = menu_bar->actions();
        QMenu* file_menu = qobject_cast<QMenu*>(menu_actions[0]->menu());
        
        REQUIRE(file_menu != nullptr);
        QList<QAction*> file_actions = file_menu->actions();
        
        // Look for the last non-separator action (which should be Exit)
        QAction* exit_action = nullptr;
        for (int i = file_actions.size() - 1; i >= 0; --i) {
            if (!file_actions[i]->isSeparator()) {
                exit_action = file_actions[i];
                break;
            }
        }
        
        REQUIRE(exit_action != nullptr);
        REQUIRE(exit_action->text().contains("xit", Qt::CaseInsensitive));
        // Exit action has shortcut set (even if platform doesn't define QKeySequence::Quit)
        // On some platforms QKeySequence::Quit may be empty, so we just verify the action exists
        REQUIRE(exit_action->shortcut() == QKeySequence::Quit);
    }
}

TEST_CASE("MainWindow has status bar", "[gui][mainwindow][statusbar]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Status bar exists") {
        QStatusBar* status_bar = window.statusBar();
        REQUIRE(status_bar != nullptr);
    }
    
    SECTION("Status bar has initial message") {
        QStatusBar* status_bar = window.statusBar();
        QString current_message = status_bar->currentMessage();
        
        // Status bar should show "Ready" initially
        REQUIRE(current_message == "Ready");
    }
}

TEST_CASE("MainWindow signals work correctly", "[gui][mainwindow][signals]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("FileOpenRequested signal can be connected") {
        QSignalSpy spy(&window, &MainWindow::FileOpenRequested);
        REQUIRE(spy.isValid());
    }
    
    SECTION("ExitRequested signal can be connected") {
        QSignalSpy spy(&window, &MainWindow::ExitRequested);
        REQUIRE(spy.isValid());
    }
}

TEST_CASE("MainWindow actions trigger correctly", "[gui][mainwindow][actions]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Exit action emits ExitRequested signal") {
        QSignalSpy spy(&window, &MainWindow::ExitRequested);
        
        // Find and trigger Exit action
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> menu_actions = menu_bar->actions();
        QMenu* file_menu = qobject_cast<QMenu*>(menu_actions[0]->menu());
        
        QAction* exit_action = nullptr;
        for (QAction* action : file_menu->actions()) {
            // Skip separators
            if (action->isSeparator()) continue;
            
            if (action->text().contains("xit", Qt::CaseInsensitive)) {
                exit_action = action;
                break;
            }
        }
        
        REQUIRE(exit_action != nullptr);
        
        // Trigger the action
        exit_action->trigger();
        
        // Check that signal was emitted
        REQUIRE(spy.count() == 1);
    }
}

TEST_CASE("MainWindow keyboard shortcuts work", "[gui][mainwindow][shortcuts]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Open action has Ctrl+O shortcut") {
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> menu_actions = menu_bar->actions();
        QMenu* file_menu = qobject_cast<QMenu*>(menu_actions[0]->menu());
        
        QAction* open_action = nullptr;
        for (QAction* action : file_menu->actions()) {
            if (action->text().contains("Open")) {
                open_action = action;
                break;
            }
        }
        
        REQUIRE(open_action != nullptr);
        QKeySequence shortcut = open_action->shortcut();
        REQUIRE(shortcut == QKeySequence::Open);
    }
    
    SECTION("Exit action has Quit shortcut") {
        QMenuBar* menu_bar = window.menuBar();
        QList<QAction*> menu_actions = menu_bar->actions();
        QMenu* file_menu = qobject_cast<QMenu*>(menu_actions[0]->menu());
        
        QAction* exit_action = nullptr;
        for (QAction* action : file_menu->actions()) {
            // Skip separators
            if (action->isSeparator()) continue;
            
            if (action->text().contains("xit", Qt::CaseInsensitive)) {
                exit_action = action;
                break;
            }
        }
        
        REQUIRE(exit_action != nullptr);
        QKeySequence shortcut = exit_action->shortcut();
        REQUIRE(shortcut == QKeySequence::Quit);
    }
}

TEST_CASE("MainWindow can be shown and hidden", "[gui][mainwindow][visibility]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Window starts hidden") {
        REQUIRE(!window.isVisible());
    }
    
    SECTION("Window can be shown") {
        window.show();
        // Process events to ensure show() takes effect
        QTest::qWait(10);
        REQUIRE(window.isVisible());
    }
    
    SECTION("Window can be hidden after showing") {
        window.show();
        QTest::qWait(10);
        window.hide();
        QTest::qWait(10);
        REQUIRE(!window.isVisible());
    }
}

TEST_CASE("MainWindow memory management", "[gui][mainwindow][memory]") {
    EnsureQApplication();
    
    SECTION("Window can be created and destroyed multiple times") {
        for (int i = 0; i < 10; ++i) {
            MainWindow* window = new MainWindow();
            REQUIRE(window != nullptr);
            delete window;
        }
    }
    
    SECTION("Window destruction is clean") {
        MainWindow* window = new MainWindow();
        window->show();
        QTest::qWait(10);
        delete window;
        // If we get here without crash, memory management is correct
        REQUIRE(true);
    }
}

TEST_CASE("MainWindow has correct window properties", "[gui][mainwindow][properties]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Window has correct title") {
        QString title = window.windowTitle();
        REQUIRE(title == "Hollow Lantern");
    }
    
    SECTION("Window is a QMainWindow") {
        MainWindow* ptr = &window;
        QMainWindow* base_ptr = dynamic_cast<QMainWindow*>(ptr);
        REQUIRE(base_ptr != nullptr);
    }
}

TEST_CASE("MainWindow UI components are accessible", "[gui][mainwindow][components]") {
    EnsureQApplication();
    MainWindow window;
    
    SECTION("Menu bar is accessible") {
        REQUIRE(window.menuBar() != nullptr);
    }
    
    SECTION("Status bar is accessible") {
        REQUIRE(window.statusBar() != nullptr);
    }
    
    SECTION("Window has valid geometry") {
        QRect geometry = window.geometry();
        REQUIRE(geometry.width() > 0);
        REQUIRE(geometry.height() > 0);
    }
}
