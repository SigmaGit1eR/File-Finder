#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <atomic>
#include <chrono>

namespace fs = std::filesystem;

std::mutex coutMutex;
std::atomic<int> activeThreads(0);

void searchInDirectory(const fs::path& directory, const std::string& filename);

void launchSearch(const fs::path& directory, const std::string& filename) {
    activeThreads++;
    std::thread([directory, filename]() {
        searchInDirectory(directory, filename);
        activeThreads--;
        }).detach();
}

void searchInDirectory(const fs::path& directory, const std::string& filename) {
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory))
            return;

        for (const auto& entry : fs::directory_iterator(directory, fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_directory()) {
                    launchSearch(entry.path(), filename);
                }
                else if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos) {
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "Found " << entry.path().filename().string() << " at: " << entry.path().string() << std::endl;
                }
            }
            catch (...) {
            }
        }
    }
    catch (...) {
    }
}

#ifdef _WIN32
#include <Windows.h>
std::vector<fs::path> getRootPaths() {
    std::vector<fs::path> roots;
    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (drives & (1 << (letter - 'A'))) {
            roots.emplace_back(std::string(1, letter) + ":\\");
        }
    }
    return roots;
}
#else
#include <mntent.h>
#include <stdio.h>
std::vector<fs::path> getRootPaths() {
    std::vector<fs::path> mounts;
    FILE* mtab = setmntent("/etc/mtab", "r");
    if (mtab) {
        mntent* entry;
        while ((entry = getmntent(mtab))) {
            mounts.emplace_back(entry->mnt_dir);
        }
        endmntent(mtab);
    }

    if (mounts.empty() || mounts[0] != "/") {
        mounts.insert(mounts.begin(), "/");
    }

    return mounts;
}
#endif

int main() {
    std::string targetFilename;
    std::cout << "Enter the filename to search for: ";
    std::getline(std::cin, targetFilename);

    std::vector<fs::path> roots = getRootPaths();

    for (const auto& root : roots) {
        searchInDirectory(root, targetFilename);
    }

    // Wait for all threads to complete
    while (activeThreads > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Search finished." << std::endl;
    return 0;
}