#include "filesystem_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/socket.h> // for send/recv

namespace fs = std::filesystem;

namespace fs_utils {

// ------------------ Network File Functions ------------------

bool sendFile(int sockfd, const std::string &filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for sending: " << filepath << "\n";
        return false;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(sockfd, buffer, sizeof(buffer), 0);
    }
    // send remaining bytes
    if (file.gcount() > 0)
        send(sockfd, buffer, file.gcount(), 0);

    file.close();
    return true;
}

bool receiveFile(int sockfd, const std::string &filepath, size_t filesize) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for receiving: " << filepath << "\n";
        return false;
    }

    char buffer[1024];
    size_t received = 0;
    while (received < filesize) {
        ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        file.write(buffer, bytes);
        received += bytes;
    }

    file.close();
    return (received == filesize);
}

// ------------------ Local File/Directory Operations ------------------

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