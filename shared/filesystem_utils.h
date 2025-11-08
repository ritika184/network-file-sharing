#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <string>
#include <vector>
#include <sys/socket.h> // for send/recv

namespace fs_utils {

    // ------------------ Network File Functions ------------------
    bool sendFile(int sockfd, const std::string &filepath);
    bool receiveFile(int sockfd, const std::string &filepath, size_t filesize);

    // ------------------ Local File/Directory Operations ------------------
    std::string getCurrentPath();
    std::vector<std::string> listFiles(const std::string &path);
    bool createFile(const std::string &filename);
    bool deleteFile(const std::string &filename);
    bool createDirectory(const std::string &dirname);

}

#endif // FILESYSTEM_UTILS_H
