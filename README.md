# Multithreaded File Downloader

A C++ application that demonstrates single-threaded vs multithreaded file downloading with both console and GUI interfaces.

## Features

- **Console Interface**: Command-line downloader with progress tracking
- **GUI Interface**: Modern Qt-based graphical interface
- **Multithreading**: Parallel download with configurable thread count
- **Range Requests**: HTTP Range support for efficient downloads
- **Fallback Mechanism**: Automatic fallback to single-threaded if needed
- **Progress Tracking**: Real-time download progress and speed monitoring
- **Error Handling**: Robust error handling and validation

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y build-essential libcurl4-openssl-dev
sudo apt-get install -y qtbase5-dev qttools5-dev-tools
```

### CentOS/RHEL
```bash
sudo yum install -y gcc-c++ libcurl-devel
sudo yum install -y qt5-qtbase-devel qt5-qttools-devel
```

### macOS
```bash
brew install curl qt
```

## Building the Application

### Method 1: Using Makefile (Recommended)
```bash
# Check dependencies
make check-deps

# Install dependencies if needed
make install-deps

# Build both versions
make all

# Or build individually
make downloader_console  # Console version only
make downloader_gui      # GUI version only
```

### Method 2: Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Method 3: Manual Compilation

#### Console Version
```bash
g++ -std=c++17 -pthread -o downloader_console MultiDownloader.cpp -lcurl
```

#### GUI Version
```bash
g++ -std=c++17 -pthread $(pkg-config --cflags Qt5Core Qt5Widgets) \
    -o downloader_gui DownloaderGUI.cpp MultiDownloader.cpp \
    $(pkg-config --libs Qt5Core Qt5Widgets) -lcurl
```

## Usage

### Console Version
```bash
./downloader_console
```

The console version will prompt you for:
1. **URL**: The file URL to download
2. **Filename**: Output filename
3. **Method**: Single-threaded (1) or Multithreaded (2)
4. **Threads**: Number of parallel threads (if multithreaded)

### GUI Version
```bash
./downloader_gui
```

The GUI provides:
- **URL Input**: Enter download URL
- **File Browser**: Choose output location
- **Method Selection**: Radio buttons for single/multithreaded
- **Thread Configuration**: Spinbox for thread count
- **Progress Bar**: Real-time download progress
- **Speed Monitor**: Download speed display
- **Log Viewer**: Detailed download log

## Test URLs

For testing the downloader, you can use these reliable test files:

- **100MB Test File**: `https://proof.ovh.net/files/100Mb.dat`
- **Alternative 100MB**: `https://ash-speed.hetzner.com/100MB.bin`
- **Another 100MB**: `https://lg.hostkey.com/100MB.test`
- **Large File**: `https://download.blender.org/release/Blender4.0/blender-4.0.2-linux-x64.tar.xz`

## Architecture

### Console Version
- `MultiDownloader.cpp`: Contains both `SingleThreadedDownloader` and `MultithreadedDownloader` classes
- Command-line interface with user input prompts
- Progress display via console output

### GUI Version
- `DownloaderGUI.h/cpp`: Qt-based graphical interface
- `MultiDownloader.h/cpp`: Downloader implementation
- Worker thread pattern for non-blocking UI
- Real-time progress updates via Qt signals/slots

## Multithreading Implementation

The multithreaded downloader works by:

1. **File Size Detection**: HEAD request to get total file size
2. **Range Support Test**: Verify server supports HTTP Range requests
3. **Chunk Calculation**: Divide file into equal chunks
4. **Parallel Download**: Each thread downloads its assigned chunk
5. **File Assembly**: Merge all chunks into final file

### Thread Safety
- `std::mutex` for progress updates
- `std::atomic` for thread-safe counters
- Separate temporary files for each chunk
- Sequential merge to avoid race conditions

## Error Handling

The application includes comprehensive error handling:

- **Network Errors**: Connection timeouts, DNS failures
- **HTTP Errors**: 404, 403, 500 status codes
- **File System Errors**: Permission denied, disk full
- **Range Request Failures**: Automatic fallback to single-threaded
- **Invalid URLs**: Input validation and error messages

## Performance

Typical performance improvements:
- **Single-threaded**: Baseline performance
- **4-threaded**: 2-4x speed improvement (depending on network/server)
- **8-threaded**: 3-6x speed improvement (diminishing returns)

## Troubleshooting

### Common Issues

1. **"curl_global_init not found"**
   - Install libcurl development package: `sudo apt-get install libcurl4-openssl-dev`

2. **"Qt not found"**
   - Install Qt development packages: `sudo apt-get install qtbase5-dev qttools5-dev-tools`

3. **"Permission denied"**
   - Check file permissions and disk space
   - Ensure write access to output directory

4. **"Download failed"**
   - Verify URL is accessible
   - Check network connectivity
   - Try single-threaded mode as fallback

### Debug Mode
```bash
# Compile with debug information
g++ -std=c++17 -g -pthread -o downloader_console MultiDownloader.cpp -lcurl
```

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues and enhancement requests!
