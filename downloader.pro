QT += core widgets
CONFIG += c++17

# Console version
TARGET = downloader_console
SOURCES += main_console.cpp MultiDownloader.cpp
HEADERS += MultiDownloader.h
LIBS += -lcurl -pthread

# GUI version
TARGET = downloader_gui
SOURCES += main_gui.cpp DownloaderGUI.cpp MultiDownloader.cpp
HEADERS += DownloaderGUI.h MultiDownloader.h
LIBS += -lcurl -pthread

# Default target
TARGET = downloader_console 