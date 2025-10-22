#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>

/// @brief Main application window for Hollow Lantern Qt6 GUI
///
/// This is the primary window that contains all UI components including
/// menu bar, central widget, and status bar. It manages file operations
/// and coordinates between different UI components.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /// @brief Construct a new MainWindow
    /// @param parent Optional parent widget
    explicit MainWindow(QWidget* parent = nullptr);
    
    /// @brief Destructor
    ~MainWindow() override = default;

signals:
    /// @brief Emitted when a file is requested to be opened
    /// @param filePath Path to the file to open
    void FileOpenRequested(const QString& filePath);
    
    /// @brief Emitted when the application should exit
    void ExitRequested();

private slots:
    /// @brief Handle Open File action
    void OnOpenFile();
    
    /// @brief Handle Exit action
    void OnExit();
    
    /// @brief Update status bar message
    /// @param message Message to display
    void UpdateStatus(const QString& message);

private:
    /// @brief Set up the menu bar
    void SetupMenuBar();
    
    /// @brief Set up the status bar
    void SetupStatusBar();
    
    /// @brief Set up connections between signals and slots
    void SetupConnections();

    // Menu actions
    QAction* open_file_action_;
    QAction* exit_action_;
    
    // Status bar
    QLabel* status_label_;
};
