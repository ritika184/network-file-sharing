#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include "../shared/filesystem_utils.h"

namespace fs = std::filesystem; // alias for std::filesystem

using namespace std;
using namespace fs_utils;

int main() {
    string line;

    cout << "=== Minimal File Explorer ===\n";
    cout << "Commands: ls | cd <dir> | up | pwd | create <file> | delete <file> | mkdir <dir> | exit\n";

    while (true) {
        cout << getCurrentPath() << " > ";
        if (!getline(cin, line)) break; // Exit on EOF
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") break;

        // Show current directory
        else if (cmd == "pwd") {
            cout << getCurrentPath() << "\n";
        }

        // List files and directories
        else if (cmd == "ls") {
            auto files = listFiles(getCurrentPath());
            for (auto &f : files)
                cout << f << "\n";
        }

        // Change directory
        else if (cmd == "cd") {
            string folder; ss >> folder;
            if (folder.empty()) { cout << "Usage: cd <folder>\n"; continue; }
            try {
                fs::path newPath = fs::path(getCurrentPath()) / folder;
                if (fs::exists(newPath) && fs::is_directory(newPath)) {
                    fs::current_path(newPath);
                } else {
                    cout << "Not a directory: " << folder << "\n";
                }
            } catch (const fs::filesystem_error &e) {
                cout << "Error: " << e.what() << "\n";
            }
        }

        // Go to parent directory
        else if (cmd == "up") {
            try {
                fs::path parent = fs::current_path().parent_path();
                fs::current_path(parent);
            } catch (const fs::filesystem_error &e) {
                cout << "Error: " << e.what() << "\n";
            }
        }

        // Create a new file
        else if (cmd == "create") {
            string filename; ss >> filename;
            if (filename.empty()) { cout << "Usage: create <filename>\n"; continue; }
            if (createFile(filename))
                cout << "File created: " << filename << "\n";
            else
                cout << "Failed to create file.\n";
        }

        // Delete a file
        else if (cmd == "delete") {
            string filename; ss >> filename;
            if (filename.empty()) { cout << "Usage: delete <filename>\n"; continue; }
            if (deleteFile(filename))
                cout << "Deleted file: " << filename << "\n";
            else
                cout << "Failed to delete file.\n";
        }

        // Create a new directory
        else if (cmd == "mkdir") {
            string dirname; ss >> dirname;
            if (dirname.empty()) { cout << "Usage: mkdir <dirname>\n"; continue; }
            if (createDirectory(dirname))
                cout << "Directory created: " << dirname << "\n";
            else
                cout << "Failed to create directory.\n";
        }

        // Unknown command
        else {
            cout << "Unknown command. Available: ls, cd <dir>, up, pwd, create <file>, delete <file>, mkdir <dir>, exit\n";
        }
    }

    cout << "Goodbye.\n";
    return 0;
}
