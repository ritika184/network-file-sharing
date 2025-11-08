#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../shared/filesystem_utils.h"

namespace fs = std::filesystem;
using namespace std;
using namespace fs_utils;

#define PORT 54000
#define BUFFER_SIZE 1024

int main() {
    // --- Create client socket ---
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Socket creation failed.\n";
        return 1;
    }

    // --- Configure server address ---
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 127.0.0.1 if local

    // --- Connect to server ---
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connection failed.\n";
        return 2;
    }

    cout << "Connected to server!\n";

    string line;
    char buffer[BUFFER_SIZE];

    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        // Convert command to uppercase for consistency
        for (auto &c : cmd) c = toupper(c);

        string arg;
        getline(ss, arg);
        if (!arg.empty() && arg[0] == ' ') arg = arg.substr(1); // remove leading space

        // --- EXIT command ---
        if (cmd == "EXIT") {
            send(sock, "EXIT", 4, 0);
            cout << "Disconnected from server.\n";
            break;
        }

        // --- UPLOAD command ---
        else if (cmd == "UPLOAD") {
            if (arg.empty()) { cout << "Usage: UPLOAD <filename>\n"; continue; }
            if (!fs::exists(arg)) { cout << "File does not exist: " << arg << endl; continue; }

            size_t filesize = fs::file_size(arg);

            string fullCmd = "UPLOAD " + arg;
            send(sock, fullCmd.c_str(), fullCmd.size(), 0);

            // Send file size
            send(sock, &filesize, sizeof(filesize), 0);

            // Send file content
            if (sendFile(sock, arg)) cout << "File uploaded: " << arg << endl;
            else cout << "Failed to upload file: " << arg << endl;
        }

        // --- DOWNLOAD command ---
        else if (cmd == "DOWNLOAD") {
            if (arg.empty()) { cout << "Usage: DOWNLOAD <filename>\n"; continue; }

            string fullCmd = "DOWNLOAD " + arg;
            send(sock, fullCmd.c_str(), fullCmd.size(), 0);

            // Receive file size first
            size_t filesize = 0;
            read(sock, &filesize, sizeof(filesize));
            if (filesize == 0) {
                cout << "File does not exist on server: " << arg << endl;
                continue;
            }

            // Receive file content
            if (receiveFile(sock, arg, filesize)) cout << "File downloaded: " << arg << endl;
            else cout << "Failed to download file: " << arg << endl;
        }

        // --- LIST, CD, UP, PWD commands ---
        else if (cmd == "LIST" || cmd == "CD" || cmd == "UP" || cmd == "PWD") {
            string sendCmd = cmd;
            if (!arg.empty()) sendCmd += " " + arg;
            send(sock, sendCmd.c_str(), sendCmd.size(), 0);

            // Receive server response
            memset(buffer, 0, BUFFER_SIZE);
            int bytesReceived = read(sock, buffer, BUFFER_SIZE - 1);
            if (bytesReceived > 0) cout << string(buffer, bytesReceived);
        }

        // --- Unknown command ---
        else {
            cout << "Unknown command. Available: LIST, UPLOAD <file>, DOWNLOAD <file>, CD <folder>, UP, PWD, EXIT\n";
        }
    }

    close(sock);
    return 0;
}
