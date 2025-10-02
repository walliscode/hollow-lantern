#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , open_file_action_(nullptr)
    , exit_action_(nullptr)
    , status_label_(nullptr)
{
    // Set window properties
    setWindowTitle("Hollow Lantern");
    resize(800, 600);
    
    // Setup UI components
    SetupMenuBar();
    SetupStatusBar();
    SetupConnections();
    
    // Initial status
    UpdateStatus("Ready");
}

void MainWindow::SetupMenuBar() {
    // Create File menu
    QMenu* file_menu = menuBar()->addMenu("&File");
    
    // Open action
    open_file_action_ = new QAction("&Open...", this);
    open_file_action_->setShortcut(QKeySequence::Open);
    open_file_action_->setStatusTip("Open a .vox file");
    file_menu->addAction(open_file_action_);
    
    file_menu->addSeparator();
    
    // Exit action
    exit_action_ = new QAction("E&xit", this);
    exit_action_->setShortcut(QKeySequence::Quit);
    exit_action_->setStatusTip("Exit the application");
    file_menu->addAction(exit_action_);
}

void MainWindow::SetupStatusBar() {
    status_label_ = new QLabel("Ready");
    statusBar()->addWidget(status_label_);
}

void MainWindow::SetupConnections() {
    // Connect menu actions to slots
    connect(open_file_action_, &QAction::triggered, this, &MainWindow::OnOpenFile);
    connect(exit_action_, &QAction::triggered, this, &MainWindow::OnExit);
}

void MainWindow::OnOpenFile() {
    UpdateStatus("Opening file...");
    
    // Show file dialog (not functional yet, just for testing)
    QString file_path = QFileDialog::getOpenFileName(
        this,
        "Open VOX File",
        "",
        "VOX Files (*.vox);;All Files (*)"
    );
    
    if (!file_path.isEmpty()) {
        emit FileOpenRequested(file_path);
        UpdateStatus(QString("Selected: %1").arg(file_path));
    } else {
        UpdateStatus("Ready");
    }
}

void MainWindow::OnExit() {
    emit ExitRequested();
    close();
}

void MainWindow::UpdateStatus(const QString& message) {
    if (status_label_) {
        status_label_->setText(message);
    }
    statusBar()->showMessage(message, 3000);  // Show for 3 seconds
}
