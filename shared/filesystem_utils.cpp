#include "filesystem_utils.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace fs_utils {

std::string getCurrentPath() {
    return fs::current_path().string();
}

std::vector<std::string> listFiles(const std::string &path) {
    std::vector<std::string> files;
    try {
        for (const auto &entry : fs::directory_iterator(path)) {
            files.push_back(entry.path().filename().string());
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error listing files: " << e.what() << std::endl;
    }
    return files;
}

bool createFile(const std::string &filename) {
    std::ofstream file(filename);
    return file.good();
}

bool deleteFile(const std::string &filename) {
    try {
        return fs::remove(filename);
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
        return false;
    }
}

bool createDirectory(const std::string &dirname) {
    try {
        return fs::create_directory(dirname);
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace fs_utils

