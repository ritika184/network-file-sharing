# Network File Sharing Project (C++)

## Overview
This is a C++ client-server application for network-based file sharing.  
It allows clients to connect to the server and perform file operations such as listing files, uploading, downloading, and navigating directories.

## Features
- **LIST**: View all files and folders in the server's current directory.
- **UPLOAD <filename>**: Upload a file from the client to the server.
- **DOWNLOAD <filename>**: Download a file from the server to the client.
- **CD <folder>**: Change to a subdirectory on the server.
- **UP**: Move to the parent directory on the server.
- **PWD**: Display the current directory path on the server.
- **EXIT**: Disconnect the client from the server.

## Folder Structure
network_file_sharing/
├─ client/ # Client-side application code
│ └─ main.cpp
├─ server/ # Server-side application code
│ └─ main.cpp
├─ shared/ # Shared utilities
│ ├─ filesystem_utils.cpp
│ └─ filesystem_utils.h
├─ build/ # Compiled binaries
├─ myfile.txt # Example file for upload
└─ README.md # Project documentation

## How to Run
1. Compile the server and client:

"g++ -std=c++17 server/main.cpp shared/filesystem_utils.cpp -o build/server_app
g++ -std=c++17 client/main.cpp shared/filesystem_utils.cpp -o build/client_app"

2. Start the server first:
"./build/server_app "

3. Start the client in another terminal:
"./build/client_app"

Example Commands
> LIST
server
client
shared
README.md

> UPLOAD myfile.txt
File uploaded: myfile.txt

> DOWNLOAD myfile.txt
File downloaded: myfile.txt

> CD shared
Directory changed to: /path/to/server/shared

> UP
Moved up to: /path/to/server

Author
Ritika Kumari
