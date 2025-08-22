#include "DownloaderGUI.h"
#include "MultiDownloader.cpp" // Include the implementation
#include <QtWidgets/QApplication>
#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtGui/QFont>
#include <QtGui/QIcon>

// DownloadWorker Implementation
DownloadWorker::DownloadWorker(const QString& url, const QString& filename, bool useMultithread, int threads)
    : m_url(url), m_filename(filename), m_useMultithread(useMultithread), m_threads(threads) {
}

DownloadWorker::~DownloadWorker() {
}

void DownloadWorker::startDownload() {
    emit logMessage(QString("Starting download: %1").arg(m_url));
    emit logMessage(QString("Output file: %1").arg(m_filename));
    emit logMessage(QString("Method: %1").arg(m_useMultithread ? "Multithreaded" : "Single-threaded"));
    
    bool success = false;
    QString message;
    
    try {
        if (m_useMultithread) {
            emit logMessage(QString("Using %1 threads").arg(m_threads));
            m_multiDownloader = std::make_unique<MultithreadedDownloader>(
                m_url.toStdString(), m_filename.toStdString(), m_threads);
            success = m_multiDownloader->Download();
            message = success ? "Multithreaded download completed successfully!" : "Multithreaded download failed!";
        } else {
            m_singleDownloader = std::make_unique<SingleThreadedDownloader>(
                m_url.toStdString(), m_filename.toStdString());
            success = m_singleDownloader->Download();
            message = success ? "Single-threaded download completed successfully!" : "Single-threaded download failed!";
        }
    } catch (const std::exception& e) {
        success = false;
        message = QString("Download error: %1").arg(e.what());
        emit logMessage(message);
    }
    
    emit downloadFinished(success, message);
}

// DownloaderGUI Implementation
DownloaderGUI::DownloaderGUI(QWidget *parent)
    : QMainWindow(parent)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
    , m_isDownloading(false)
    , m_startTime(0)
    , m_totalBytes(0)
    , m_downloadedBytes(0) {
    
    setupUI();
    setupConnections();
    setDownloadState(false);
    
    // Set window properties
    setWindowTitle("Multithreaded File Downloader");
    setMinimumSize(800, 600);
    resize(900, 700);
    
    // Setup update timer
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &DownloaderGUI::updateTimer);
}

DownloaderGUI::~DownloaderGUI() {
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void DownloaderGUI::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // URL Input Section
    m_urlGroup = new QGroupBox("Download URL", this);
    QVBoxLayout *urlLayout = new QVBoxLayout(m_urlGroup);
    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setPlaceholderText("Enter the URL to download (e.g., https://proof.ovh.net/files/100Mb.dat)");
    urlLayout->addWidget(m_urlEdit);
    m_mainLayout->addWidget(m_urlGroup);
    
    // File Output Section
    m_fileGroup = new QGroupBox("Output File", this);
    m_fileLayout = new QHBoxLayout(m_fileGroup);
    m_filenameEdit = new QLineEdit(this);
    m_filenameEdit->setPlaceholderText("Enter filename or browse...");
    m_browseButton = new QPushButton("Browse...", this);
    m_fileLayout->addWidget(m_filenameEdit);
    m_fileLayout->addWidget(m_browseButton);
    m_mainLayout->addWidget(m_fileGroup);
    
    // Download Method Section
    m_methodGroup = new QGroupBox("Download Method", this);
    QGridLayout *methodLayout = new QGridLayout(m_methodGroup);
    
    m_singleThreadRadio = new QRadioButton("Single-threaded download", this);
    m_multiThreadRadio = new QRadioButton("Multithreaded download", this);
    m_multiThreadRadio->setChecked(true); // Default to multithreaded
    
    m_methodButtonGroup = new QButtonGroup(this);
    m_methodButtonGroup->addButton(m_singleThreadRadio, 0);
    m_methodButtonGroup->addButton(m_multiThreadRadio, 1);
    
    m_threadsLabel = new QLabel("Number of threads:", this);
    m_threadsSpinBox = new QSpinBox(this);
    m_threadsSpinBox->setRange(1, 16);
    m_threadsSpinBox->setValue(4);
    
    methodLayout->addWidget(m_singleThreadRadio, 0, 0, 1, 2);
    methodLayout->addWidget(m_multiThreadRadio, 1, 0, 1, 2);
    methodLayout->addWidget(m_threadsLabel, 2, 0);
    methodLayout->addWidget(m_threadsSpinBox, 2, 1);
    
    m_mainLayout->addWidget(m_methodGroup);
    
    // Progress Section
    m_progressGroup = new QGroupBox("Download Progress", this);
    QVBoxLayout *progressLayout = new QVBoxLayout(m_progressGroup);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    progressLayout->addWidget(m_progressBar);
    
    QGridLayout *infoLayout = new QGridLayout();
    m_progressLabel = new QLabel("Ready to download", this);
    m_speedLabel = new QLabel("Speed: 0 KB/s", this);
    m_sizeLabel = new QLabel("Size: 0 / 0 bytes", this);
    m_timeLabel = new QLabel("Time: 00:00", this);
    
    infoLayout->addWidget(m_progressLabel, 0, 0);
    infoLayout->addWidget(m_speedLabel, 0, 1);
    infoLayout->addWidget(m_sizeLabel, 1, 0);
    infoLayout->addWidget(m_timeLabel, 1, 1);
    
    progressLayout->addLayout(infoLayout);
    m_mainLayout->addWidget(m_progressGroup);
    
    // Control Buttons
    m_buttonLayout = new QHBoxLayout();
    m_downloadButton = new QPushButton("Start Download", this);
    m_downloadButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
    m_clearLogButton = new QPushButton("Clear Log", this);
    
    m_buttonLayout->addWidget(m_downloadButton);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_clearLogButton);
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // Log Section
    m_logGroup = new QGroupBox("Download Log", this);
    QVBoxLayout *logLayout = new QVBoxLayout(m_logGroup);
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setMaximumHeight(200);
    m_logTextEdit->setFont(QFont("Consolas", 9));
    m_logTextEdit->setReadOnly(true);
    logLayout->addWidget(m_logTextEdit);
    m_mainLayout->addWidget(m_logGroup);
    
    // Status Bar
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    m_statusBar->showMessage("Ready");
}

void DownloaderGUI::setupConnections() {
    connect(m_browseButton, &QPushButton::clicked, this, &DownloaderGUI::onBrowseClicked);
    connect(m_downloadButton, &QPushButton::clicked, this, &DownloaderGUI::onDownloadClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &DownloaderGUI::onCancelClicked);
    connect(m_clearLogButton, &QPushButton::clicked, this, &DownloaderGUI::onClearLogClicked);
    
    // Enable/disable thread spinbox based on method selection
    connect(m_singleThreadRadio, &QRadioButton::toggled, [this](bool checked) {
        m_threadsLabel->setEnabled(!checked);
        m_threadsSpinBox->setEnabled(!checked);
    });
}

void DownloaderGUI::onBrowseClicked() {
    QString downloads = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString filename = QFileDialog::getSaveFileName(this, "Save File As", downloads, "All Files (*.*)");
    if (!filename.isEmpty()) {
        m_filenameEdit->setText(filename);
    }
}

void DownloaderGUI::onDownloadClicked() {
    if (m_isDownloading) {
        return;
    }
    
    QString url = m_urlEdit->text().trimmed();
    QString filename = m_filenameEdit->text().trimmed();
    
    // Validation
    if (url.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid URL.");
        return;
    }
    
    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please specify an output filename.");
        return;
    }
    
    // Check if file already exists
    if (QFile::exists(filename)) {
        int ret = QMessageBox::question(this, "File Exists", 
            "The file already exists. Do you want to overwrite it?", 
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            return;
        }
    }
    
    // Start download
    setDownloadState(true);
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    
    // Create worker thread
    m_workerThread = new QThread(this);
    m_worker = new DownloadWorker(url, filename, m_multiThreadRadio->isChecked(), m_threadsSpinBox->value());
    m_worker->moveToThread(m_workerThread);
    
    // Connect worker signals
    connect(m_workerThread, &QThread::started, m_worker, &DownloadWorker::startDownload);
    connect(m_worker, &DownloadWorker::downloadProgress, this, &DownloaderGUI::onDownloadProgress);
    connect(m_worker, &DownloadWorker::downloadFinished, this, &DownloaderGUI::onDownloadFinished);
    connect(m_worker, &DownloadWorker::logMessage, this, &DownloaderGUI::onLogMessage);
    
    // Start the thread
    m_workerThread->start();
    m_updateTimer->start(500); // Update every 500ms
    
    onLogMessage("Download started...");
    m_statusBar->showMessage("Downloading...");
}

void DownloaderGUI::onCancelClicked() {
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
        setDownloadState(false);
        onLogMessage("Download cancelled by user.");
        m_statusBar->showMessage("Download cancelled");
    }
}

void DownloaderGUI::onClearLogClicked() {
    m_logTextEdit->clear();
}

void DownloaderGUI::onDownloadProgress(int percentage, qint64 downloaded, qint64 total, double speed) {
    m_progressBar->setValue(percentage);
    m_downloadedBytes = downloaded;
    m_totalBytes = total;
    
    m_progressLabel->setText(QString("Progress: %1%").arg(percentage));
    m_speedLabel->setText(QString("Speed: %1").arg(formatSpeed(speed)));
    m_sizeLabel->setText(QString("Size: %1 / %2").arg(formatBytes(downloaded)).arg(formatBytes(total)));
}

void DownloaderGUI::onDownloadFinished(bool success, const QString& message) {
    setDownloadState(false);
    m_updateTimer->stop();
    
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        m_workerThread->deleteLater();
        m_workerThread = nullptr;
    }
    
    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
    }
    
    onLogMessage(message);
    
    if (success) {
        m_statusBar->showMessage("Download completed successfully!");
        m_progressBar->setValue(100);
        QMessageBox::information(this, "Download Complete", "File downloaded successfully!");
    } else {
        m_statusBar->showMessage("Download failed!");
        QMessageBox::critical(this, "Download Failed", message);
    }
}

void DownloaderGUI::onLogMessage(const QString& message) {
    QMutexLocker locker(&m_logMutex);
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
}

void DownloaderGUI::updateTimer() {
    if (m_isDownloading && m_startTime > 0) {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
        int seconds = elapsed / 1000;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        
        m_timeLabel->setText(QString("Time: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    }
}

void DownloaderGUI::setDownloadState(bool isDownloading) {
    m_isDownloading = isDownloading;
    m_urlEdit->setEnabled(!isDownloading);
    m_filenameEdit->setEnabled(!isDownloading);
    m_browseButton->setEnabled(!isDownloading);
    m_singleThreadRadio->setEnabled(!isDownloading);
    m_multiThreadRadio->setEnabled(!isDownloading);
    m_threadsSpinBox->setEnabled(!isDownloading && m_multiThreadRadio->isChecked());
    m_downloadButton->setEnabled(!isDownloading);
    m_cancelButton->setEnabled(isDownloading);
    
    if (!isDownloading) {
        m_progressBar->setValue(0);
        m_progressLabel->setText("Ready to download");
        m_speedLabel->setText("Speed: 0 KB/s");
        m_sizeLabel->setText("Size: 0 / 0 bytes");
        m_timeLabel->setText("Time: 00:00");
    }
}

QString DownloaderGUI::formatBytes(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString::number(bytes / (double)GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / (double)MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / (double)KB, 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " bytes";
    }
}

QString DownloaderGUI::formatSpeed(double bytesPerSecond) {
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    
    if (bytesPerSecond >= MB) {
        return QString::number(bytesPerSecond / MB, 'f', 2) + " MB/s";
    } else if (bytesPerSecond >= KB) {
        return QString::number(bytesPerSecond / KB, 'f', 2) + " KB/s";
    } else {
        return QString::number(bytesPerSecond, 'f', 0) + " B/s";
    }
}

// main.cpp - moved to separate file



/*
Compilation Instructions:

1. Install Qt5 or Qt6 development libraries:
   Ubuntu/Debian: sudo apt-get install qtbase5-dev qttools5-dev-tools
   CentOS/RHEL: sudo yum install qt5-qtbase-devel qt5-qttools-devel
   Windows: Download Qt from https://www.qt.io/download
   macOS: brew install qt

2. Create a CMakeLists.txt file:
   
cmake_minimum_required(VERSION 3.16)
project(MultithreadedDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

qt6_standard_project_setup()

qt6_add_executable(downloader_gui
    DownloaderGUI.cpp
    MultiDownloader.cpp
)

target_link_libraries(downloader_gui Qt6::Core Qt6::Widgets ${CURL_LIBRARIES})
target_include_directories(downloader_gui PRIVATE ${CURL_INCLUDE_DIRS})

3. Build:
   mkdir build && cd build
   cmake ..
   make

Alternative qmake method:
1. Create downloader.pro:
   QT += core widgets
   CONFIG += c++17
   TARGET = downloader_gui
   SOURCES += DownloaderGUI.cpp
   HEADERS += MultiDownloader.cpp
   LIBS += -lcurl

2. Build:
   qmake
   make

Features:
- Modern Qt-based GUI interface
- Real-time progress tracking
- Download speed monitoring
- Log viewer with timestamps
- File browser integration
- Input validation
- Thread count configuration
- Single/Multi-threaded selection
- Cancel functionality
- Error handling with message boxes
- Cross-platform compatibility
*/
