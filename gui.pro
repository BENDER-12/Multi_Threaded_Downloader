QT += core widgets
CONFIG += c++17

TARGET = downloader_gui
SOURCES += main_gui.cpp DownloaderGUI.cpp MultiDownloader.cpp
HEADERS += DownloaderGUI.h MultiDownloader.h

LIBS += -lcurl -pthread

# Compilation instructions
# qmake gui.pro
# make 