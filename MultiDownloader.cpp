#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <curl/curl.h>
#include <cmath>
#include <iomanip>
#include <atomic>

// Single-threaded downloader for comparison
class SingleThreadedDownloader {
private:
    std::string url;
    std::string filename;
    
    // Progress tracking
    struct ProgressData {
        SingleThreadedDownloader* downloader;
        std::chrono::high_resolution_clock::time_point start_time;
    };
    
    // Callback function to write downloaded data to file
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t total_size = size * nmemb;
        std::ofstream* file = static_cast<std::ofstream*>(userp);
        
        if (file && file->is_open()) {
            file->write(static_cast<char*>(contents), total_size);
            return total_size;
        }
        return 0;
    }
    
    // Progress callback
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, 
                               curl_off_t ultotal, curl_off_t ulnow) {
        if (dltotal == 0) return 0;
        
        ProgressData* data = static_cast<ProgressData*>(clientp);
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - data->start_time);
        
        double progress = (double)dlnow / dltotal * 100.0;
        double speed = dlnow / (elapsed.count() + 1); // +1 to avoid division by zero
        
        std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
                 << progress << "% (" << dlnow << "/" << dltotal << " bytes) "
                 << "Speed: " << std::fixed << std::setprecision(1) << speed / 1024 << " KB/s" << std::flush;
        
        return 0;
    }
    
public:
    SingleThreadedDownloader(const std::string& url, const std::string& filename) 
        : url(url), filename(filename) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    
    ~SingleThreadedDownloader() {
        curl_global_cleanup();
    }
    
    bool Download() {
        std::cout << "Starting single-threaded download..." << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Filename: " << filename << std::endl;
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return false;
        }
        
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize curl" << std::endl;
            return false;
        }
        
        ProgressData progress_data;
        progress_data.downloader = this;
        progress_data.start_time = std::chrono::high_resolution_clock::now();
        
        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);  // 5 minute timeout
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);  // 30 second connect timeout
        
        // Progress callback
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress_data);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        CURLcode res = curl_easy_perform(curl);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        // Get response info
        long response_code;
        curl_off_t download_size;
        char* content_type = nullptr;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &download_size);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        
        std::cout << "\nResponse code: " << response_code << std::endl;
        std::cout << "Content-Type: " << (content_type ? content_type : "unknown") << std::endl;
        std::cout << "Downloaded: " << download_size << " bytes" << std::endl;
        
        if (res != CURLE_OK) {
            std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            file.close();
            return false;
        }
        
        // Check for HTTP errors
        if (response_code >= 400) {
            std::cerr << "HTTP Error: " << response_code << std::endl;
            curl_easy_cleanup(curl);
            file.close();
            return false;
        }
        
        // Check if we got HTML instead of binary data
        if (content_type && (strstr(content_type, "text/html") || strstr(content_type, "text/plain"))) {
            std::cerr << "Warning: Received HTML/text content instead of binary file!" << std::endl;
            std::cerr << "This might indicate a server error or redirect issue." << std::endl;
        }
        
        curl_easy_cleanup(curl);
        file.close();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "Download completed successfully!" << std::endl;
        std::cout << "Total time: " << duration.count() << " ms" << std::endl;
        
        return true;
    }
};

// Multithreaded downloader with fixes
class MultithreadedDownloader {
private:
    std::string url;
    std::string filename;
    int num_threads;
    curl_off_t file_size;
    std::vector<std::thread> threads;
    std::mutex progress_mutex;
    std::atomic<curl_off_t> total_downloaded{0};
    
    // Structure to hold data for each chunk download
    struct ChunkData {
        std::string url;
        std::string filename;
        curl_off_t start_byte;
        curl_off_t end_byte;
        int chunk_id;
        MultithreadedDownloader* downloader;
    };
    
    // Callback function to write downloaded data to file
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t total_size = size * nmemb;
        std::ofstream* file = static_cast<std::ofstream*>(userp);
        
        if (file && file->is_open()) {
            file->write(static_cast<char*>(contents), total_size);
            return total_size;
        }
        return 0;
    }
    
    // Progress callback to track download progress
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, 
                               curl_off_t ultotal, curl_off_t ulnow) {
        ChunkData* chunk = static_cast<ChunkData*>(clientp);
        if (chunk && chunk->downloader) {
            chunk->downloader->UpdateProgress(dlnow);
        }
        return 0;
    }
    
    // Get file size from server using updated curl function
    curl_off_t GetFileSize(const std::string& url) {
        CURL* curl;
        CURLcode res;
        curl_off_t file_size = 0;
        
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);  // 30 second timeout
            
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &file_size);
                
                std::cout << "HEAD request - Response code: " << response_code << std::endl;
                std::cout << "Content-Length: " << file_size << " bytes" << std::endl;
                
                // Check if response is successful
                if (response_code < 200 || response_code >= 300) {
                    std::cerr << "Server returned error code: " << response_code << std::endl;
                    file_size = 0;
                }
            } else {
                std::cerr << "HEAD request failed: " << curl_easy_strerror(res) << std::endl;
            }
            curl_easy_cleanup(curl);
        }
        return file_size;
    }
    
    // Check if server supports range requests
    bool SupportsRangeRequests(const std::string& url) {
        CURL* curl;
        CURLcode res;
        bool supports_range = false;
        
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            
            // Set a small range to test
            curl_easy_setopt(curl, CURLOPT_RANGE, "0-1023");
            
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                std::cout << "Range request test - Response code: " << response_code << std::endl;
                supports_range = (response_code == 206); // Partial Content
            } else {
                std::cerr << "Range request test failed: " << curl_easy_strerror(res) << std::endl;
            }
            curl_easy_cleanup(curl);
        }
        return supports_range;
    }
    
    // Download a specific chunk of the file
    void DownloadChunk(ChunkData chunk_data) {
        CURL* curl;
        CURLcode res;
        
        // Create temporary file for this chunk
        std::string temp_filename = chunk_data.filename + ".part" + std::to_string(chunk_data.chunk_id);
        std::ofstream temp_file(temp_filename, std::ios::binary);
        
        if (!temp_file.is_open()) {
            std::cerr << "Failed to create temporary file: " << temp_filename << std::endl;
            return;
        }
        
        curl = curl_easy_init();
        if (curl) {
            // Set URL
            curl_easy_setopt(curl, CURLOPT_URL, chunk_data.url.c_str());
            
            // Set range for this chunk
            std::string range = std::to_string(chunk_data.start_byte) + "-" + 
                               std::to_string(chunk_data.end_byte);
            curl_easy_setopt(curl, CURLOPT_RANGE, range.c_str());
            
            // Set write callback
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &temp_file);
            
            // Set progress callback
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &chunk_data);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            
            // Other options
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
            
            // Perform the download
            res = curl_easy_perform(curl);
            
            if (res != CURLE_OK) {
                std::cerr << "Chunk " << chunk_data.chunk_id << " download failed: " 
                         << curl_easy_strerror(res) << std::endl;
            } else {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                std::cout << "Chunk " << chunk_data.chunk_id << " downloaded successfully (HTTP " << response_code << ")" << std::endl;
            }
            
            curl_easy_cleanup(curl);
        }
        
        temp_file.close();
    }
    
    // Merge all downloaded chunks into final file
    void MergeChunks() {
        std::ofstream final_file(filename, std::ios::binary);
        if (!final_file.is_open()) {
            std::cerr << "Failed to create final file: " << filename << std::endl;
            return;
        }
        
        std::cout << "\nMerging chunks..." << std::endl;
        
        for (int i = 0; i < num_threads; ++i) {
            std::string temp_filename = filename + ".part" + std::to_string(i);
            std::ifstream temp_file(temp_filename, std::ios::binary);
            
            if (temp_file.is_open()) {
                // Copy chunk data to final file
                final_file << temp_file.rdbuf();
                temp_file.close();
                
                // Remove temporary file
                std::remove(temp_filename.c_str());
                std::cout << "Merged chunk " << i << std::endl;
            } else {
                std::cerr << "Failed to open chunk file: " << temp_filename << std::endl;
            }
        }
        
        final_file.close();
        std::cout << "File merge completed!" << std::endl;
    }
    
public:
    MultithreadedDownloader(const std::string& url, const std::string& filename, int threads = 4) 
        : url(url), filename(filename), num_threads(threads), file_size(0) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    
    ~MultithreadedDownloader() {
        curl_global_cleanup();
    }
    
    // Update progress tracking
    void UpdateProgress(curl_off_t bytes_downloaded) {
        std::lock_guard<std::mutex> lock(progress_mutex);
        // This is a simplified progress update
        // In a real implementation, you'd track per-chunk progress more accurately
    }
    
    // Main download function
    bool Download() {
        std::cout << "Starting multithreaded download..." << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Filename: " << filename << std::endl;
        std::cout << "Threads: " << num_threads << std::endl;
        
        // Get file size
        file_size = GetFileSize(url);
        if (file_size <= 0) {
            std::cerr << "Failed to get file size or file is empty. Trying single-threaded download..." << std::endl;
            
            // Fall back to single-threaded download
            SingleThreadedDownloader fallback(url, filename);
            return fallback.Download();
        }
        
        std::cout << "File size: " << file_size << " bytes (" << file_size / 1024 / 1024 << " MB)" << std::endl;
        
        // Check if server supports range requests
        if (!SupportsRangeRequests(url)) {
            std::cout << "Server doesn't support range requests. Falling back to single-threaded download..." << std::endl;
            SingleThreadedDownloader fallback(url, filename);
            return fallback.Download();
        }
        
        std::cout << "Server supports range requests. Proceeding with multithreaded download." << std::endl;
        
        // Calculate chunk size
        curl_off_t chunk_size = file_size / num_threads;
        curl_off_t remainder = file_size % num_threads;
        
        std::cout << "Chunk size: " << chunk_size << " bytes" << std::endl;
        std::cout << "\nStarting download with " << num_threads << " threads..." << std::endl;
        
        // Record start time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Create and start threads for each chunk
        for (int i = 0; i < num_threads; ++i) {
            ChunkData chunk_data;
            chunk_data.url = url;
            chunk_data.filename = filename;
            chunk_data.start_byte = i * chunk_size;
            chunk_data.end_byte = (i == num_threads - 1) ? 
                                  (i + 1) * chunk_size - 1 + remainder : 
                                  (i + 1) * chunk_size - 1;
            chunk_data.chunk_id = i;
            chunk_data.downloader = this;
            
            std::cout << "Thread " << i << ": bytes " << chunk_data.start_byte 
                     << "-" << chunk_data.end_byte << " (" << (chunk_data.end_byte - chunk_data.start_byte + 1) << " bytes)" << std::endl;
            
            threads.emplace_back(&MultithreadedDownloader::DownloadChunk, this, chunk_data);
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Record end time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nAll chunks downloaded in " << duration.count() << " ms" << std::endl;
        
        // Merge chunks
        MergeChunks();
        
        std::cout << "Download completed successfully!" << std::endl;
        std::cout << "Total time: " << duration.count() << " ms" << std::endl;
        
        return true;
    }
    
    // Display download statistics
    void DisplayStats() {
        std::cout << "\n=== Download Statistics ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "Size: " << file_size << " bytes (" << file_size / 1024 / 1024 << " MB)" << std::endl;
        std::cout << "Threads used: " << num_threads << std::endl;
        std::cout << "Chunks: " << num_threads << std::endl;
        std::cout << "Average chunk size: " << (file_size / num_threads) << " bytes" << std::endl;
    }
};

// Example usage and demonstration
int main() {
    std::cout << "=== File Downloader (Single-threaded vs Multithreaded) ===" << std::endl;
    std::cout << "This program demonstrates both single-threaded and multithreaded downloading." << std::endl;
    std::cout << "The multithreaded version automatically falls back to single-threaded if needed." << std::endl;
    std::cout << std::endl;
    
    std::string download_url;
    std::string output_filename;
    int choice;
    
    std::cout << "Enter URL to download: ";
    std::getline(std::cin, download_url);
    
    std::cout << "Enter output filename: ";
    std::getline(std::cin, output_filename);
    
    std::cout << "\nChoose download method:" << std::endl;
    std::cout << "1. Single-threaded download" << std::endl;
    std::cout << "2. Multithreaded download" << std::endl;
    std::cout << "Enter choice (1 or 2): ";
    std::cin >> choice;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    if (choice == 1) {
        // Single-threaded download
        SingleThreadedDownloader downloader(download_url, output_filename);
        if (!downloader.Download()) {
            std::cerr << "Download failed!" << std::endl;
            return 1;
        }
    } else {
        // Multithreaded download
        int num_threads = 4;
        std::cout << "Enter number of threads (default 4): ";
        std::cin >> num_threads;
        if (num_threads <= 0) num_threads = 4;
        
        MultithreadedDownloader downloader(download_url, output_filename, num_threads);
        if (downloader.Download()) {
            downloader.DisplayStats();
        } else {
            std::cerr << "Download failed!" << std::endl;
            return 1;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "\n=== Total Program Time ===" << std::endl;
    std::cout << "Total execution time: " << total_duration.count() << " ms" << std::endl;
    
    return 0;
}

// Compilation instructions:
// g++ -std=c++11 -pthread -lcurl -o downloader downloader.cpp
//
// Prerequisites:
// - libcurl development libraries
// - On Ubuntu/Debian: sudo apt-get install libcurl4-openssl-dev
// - On CentOS/RHEL: sudo yum install libcurl-devel
// - On macOS: brew install curl
// - On Windows (MSYS2): pacman -S mingw-w64-ucrt-x86_64-curl
//
// Working test URLs:
// - https://download.blender.org/release/Blender4.0/blender-4.0.2-linux-x64.tar.xz
// - https://nodejs.org/dist/v20.10.0/node-v20.10.0-linux-x64.tar.xz
// - https://github.com/git/git/archive/refs/tags/v2.42.0.tar.gz
// - https://www.7-zip.org/a/7z2201-x64.exe
// - https://releases.ubuntu.com/20.04/ubuntu-20.04.6-desktop-amd64.iso
//
// For testing large files with known working servers:
// - https://proof.ovh.net/files/100Mb.dat (100MB test file)
// - https://ash-speed.hetzner.com/100MB.bin (Alternative Hetzner mirror)
// - https://lg.hostkey.com/100MB.test (100MB test file)
// - http://speedtest.tele2.net/100MB.zip (100MB test file)