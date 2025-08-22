#ifndef MULTIDOWNLOADER_H
#define MULTIDOWNLOADER_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <curl/curl.h>

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
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    
    // Progress callback
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, 
                               curl_off_t ultotal, curl_off_t ulnow);
    
public:
    SingleThreadedDownloader(const std::string& url, const std::string& filename);
    ~SingleThreadedDownloader();
    
    bool Download();
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
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    
    // Progress callback to track download progress
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, 
                               curl_off_t ultotal, curl_off_t ulnow);
    
    // Get file size from server using updated curl function
    curl_off_t GetFileSize(const std::string& url);
    
    // Check if server supports range requests
    bool SupportsRangeRequests(const std::string& url);
    
    // Download a specific chunk of the file
    void DownloadChunk(ChunkData chunk_data);
    
    // Merge all downloaded chunks into final file
    void MergeChunks();
    
public:
    MultithreadedDownloader(const std::string& url, const std::string& filename, int threads = 4);
    ~MultithreadedDownloader();
    
    // Update progress tracking
    void UpdateProgress(curl_off_t bytes_downloaded);
    
    // Main download function
    bool Download();
    
    // Display download statistics
    void DisplayStats();
};

#endif // MULTIDOWNLOADER_H 