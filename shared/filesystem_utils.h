#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <string>
#include <vector>

namespace fs_utils {
    std::string getCurrentPath();
    std::vector<std::string> listFiles(const std::string &path);
    bool createFile(const std::string &filename);
    bool deleteFile(const std::string &filename);
    bool createDirectory(const std::string &dirname);
}

#endif
