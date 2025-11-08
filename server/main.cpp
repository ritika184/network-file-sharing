#include <iostream>
#include <sstream>
#include <string>
#include <cstring>       // For memset
#include <filesystem>
#include <fstream>
#include <vector>
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
    // --- Create server socket ---
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Failed to create socket.\n";
        return 1;
    }

    // --- Configure server address ---
    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    // --- Bind socket ---
    if (bind(serverSocket, (sockaddr*)&hint, sizeof(hint)) < 0) {
        cerr << "Bind failed. Make sure no other server is using port " << PORT << ".\n";
        return 2;
    }

    // --- Listen for incoming connections ---
    if (listen(serverSocket, 1) < 0) {
        cerr << "Listen failed.\n";
        return 3;
    }
    cout << "Server listening on port " << PORT << "...\n";

    // --- Accept client connection ---
    sockaddr_in client{};
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1) {
        cerr << "Client connection failed.\n";
        return 4;
    }
    cout << "Client connected!\n";

    // --- Current working directory (server base folder) ---
    fs::path currentDir = fs::current_path();
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break; // Client disconnected

        string input(buffer);
        stringstream ss(input);
        string command;
        ss >> command;

        string response; // Response to send back to client

        // --- EXIT command ---
        if (command == "EXIT") {
            cout << "Client requested exit.\n";
            break;
        }

        // --- LIST files/folders ---
        else if (command == "LIST") {
            try {
                for (const auto &entry : fs::directory_iterator(currentDir))
                    response += entry.path().filename().string() + "\n";
            } catch (...) { response = "Error listing files.\n"; }
        }

        // --- UPLOAD a file from client ---
        else if (command == "UPLOAD") {
            string filename; ss >> filename;
            if (filename.empty()) { response = "Usage: UPLOAD <filename>\n"; }
            else {
                size_t filesize;
                recv(clientSocket, &filesize, sizeof(filesize), 0);
                string serverFilePath = (currentDir / filename).string();
                if (receiveFile(clientSocket, serverFilePath, filesize)) {
                    response = "File received: " + filename + "\n";
                    cout << response;
                } else { response = "Failed to receive file: " + filename + "\n"; }
            }
        }

        // --- DOWNLOAD a file to client ---
        else if (command == "DOWNLOAD") {
            string filename; ss >> filename;
            if (filename.empty()) { response = "Usage: DOWNLOAD <filename>\n"; }
            else {
                string serverFilePath = (currentDir / filename).string();
                if (fs::exists(serverFilePath)) {
                    size_t filesize = fs::file_size(serverFilePath);
                    send(clientSocket, &filesize, sizeof(filesize), 0); // Send file size
                    sendFile(clientSocket, serverFilePath);             // Send file
                    response = "File sent: " + filename + "\n";
                    cout << response;
                } else {
                    size_t filesize = 0;
                    send(clientSocket, &filesize, sizeof(filesize), 0); // File not exists
                    response = "File does not exist on server: " + filename + "\n";
                }
            }
        }

        // --- Change directory into a subfolder ---
        else if (command == "CD") {
            string folder; ss >> folder;
            if (folder.empty()) response = "Usage: CD <folder>\n";
            else {
                fs::path newPath = currentDir / folder;
                if (fs::exists(newPath) && fs::is_directory(newPath)) {
                    currentDir = newPath;
                    response = "Directory changed to: " + currentDir.string() + "\n";
                } else { response = "Folder does not exist: " + folder + "\n"; }
            }
        }

        // --- Move to parent directory ---
        else if (command == "UP") {
            fs::path parent = currentDir.parent_path();
            if (!parent.empty()) { currentDir = parent; response = "Moved up to: " + currentDir.string() + "\n"; }
            else response = "Already at root directory\n";
        }

        // --- Show current directory ---
        else if (command == "PWD") {
            response = "Current directory: " + currentDir.string() + "\n";
        }

        // --- Unknown command ---
        else { response = "Unknown command. Available: LIST, UPLOAD <file>, DOWNLOAD <file>, CD <folder>, UP, PWD, EXIT\n"; }

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
    close(serverSocket);
    cout << "Server shut down.\n";
    return 0;
}
