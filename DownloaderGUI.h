#ifndef DOWNLOADERGUI_H
#define DOWNLOADERGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <memory>

// Forward declarations
class SingleThreadedDownloader;
class MultithreadedDownloader;

class DownloadWorker : public QObject {
    Q_OBJECT

public:
    DownloadWorker(const QString& url, const QString& filename, bool useMultithread, int threads);
    ~DownloadWorker();

public slots:
    void startDownload();

signals:
    void downloadProgress(int percentage, qint64 downloaded, qint64 total, double speed);
    void downloadFinished(bool success, const QString& message);
    void logMessage(const QString& message);

private:
    QString m_url;
    QString m_filename;
    bool m_useMultithread;
    int m_threads;
    std::unique_ptr<SingleThreadedDownloader> m_singleDownloader;
    std::unique_ptr<MultithreadedDownloader> m_multiDownloader;
};

class DownloaderGUI : public QMainWindow {
    Q_OBJECT

public:
    DownloaderGUI(QWidget *parent = nullptr);
    ~DownloaderGUI();

private slots:
    void onBrowseClicked();
    void onDownloadClicked();
    void onCancelClicked();
    void onClearLogClicked();
    void onDownloadProgress(int percentage, qint64 downloaded, qint64 total, double speed);
    void onDownloadFinished(bool success, const QString& message);
    void onLogMessage(const QString& message);
    void updateTimer();

private:
    void setupUI();
    void setupConnections();
    void setDownloadState(bool isDownloading);
    QString formatBytes(qint64 bytes);
    QString formatSpeed(double bytesPerSecond);

    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // URL Input Section
    QGroupBox *m_urlGroup;
    QLineEdit *m_urlEdit;
    
    // File Output Section
    QGroupBox *m_fileGroup;
    QHBoxLayout *m_fileLayout;
    QLineEdit *m_filenameEdit;
    QPushButton *m_browseButton;
    
    // Download Method Section
    QGroupBox *m_methodGroup;
    QRadioButton *m_singleThreadRadio;
    QRadioButton *m_multiThreadRadio;
    QButtonGroup *m_methodButtonGroup;
    QLabel *m_threadsLabel;
    QSpinBox *m_threadsSpinBox;
    
    // Progress Section
    QGroupBox *m_progressGroup;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    QLabel *m_speedLabel;
    QLabel *m_sizeLabel;
    QLabel *m_timeLabel;
    
    // Control Buttons
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_downloadButton;
    QPushButton *m_cancelButton;
    QPushButton *m_clearLogButton;
    
    // Log Section
    QGroupBox *m_logGroup;
    QTextEdit *m_logTextEdit;
    
    // Status Bar
    QStatusBar *m_statusBar;
    
    // Download Management
    QThread *m_workerThread;
    DownloadWorker *m_worker;
    QTimer *m_updateTimer;
    QMutex m_logMutex;
    
    // Download State
    bool m_isDownloading;
    qint64 m_startTime;
    qint64 m_totalBytes;
    qint64 m_downloadedBytes;
};

#endif // DOWNLOADERGUI_H