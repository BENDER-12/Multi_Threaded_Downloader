#include "MultiDownloader.cpp"
#include <iostream>
#include <chrono>

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