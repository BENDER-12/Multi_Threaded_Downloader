#include "DownloaderGUI.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Multithreaded File Downloader");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Company");
    
    DownloaderGUI window;
    window.show();
    
    return app.exec();
} 