QT += core
CONFIG += c++17 console

TARGET = downloader_console
SOURCES += main_console.cpp MultiDownloader.cpp
HEADERS += MultiDownloader.h

LIBS += -lcurl -pthread

# Compilation instructions
# qmake console.pro
# make 