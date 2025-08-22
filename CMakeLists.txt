cmake_minimum_required(VERSION 3.16)
project(MultithreadedDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure position independent code for Qt reduce-relocations builds
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

qt6_standard_project_setup()

qt6_add_executable(downloader_gui
    DownloaderGUI.cpp
    MultiDownloader.cpp
)

target_compile_options(downloader_gui PRIVATE -fPIC)
target_link_libraries(downloader_gui PRIVATE Qt6::Core Qt6::Widgets ${CURL_LIBRARIES})
target_include_directories(downloader_gui PRIVATE ${CURL_INCLUDE_DIRS} /usr/include/x86_64-linux-gnu/qt6)
