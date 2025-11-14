#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma once

std::vector<SOCKET> clients;
std::mutex clientsMutex;

void handleClient(SOCKET client) {
  while (true) {
    // Receive file name length
    int fileNameLength;
    if (recv(client, (char *)&fileNameLength, sizeof(fileNameLength), 0) <= 0)
      break;

    // Receive file name
    char fileName[256];
    if (recv(client, fileName, fileNameLength, 0) <= 0)
      break;
    fileName[fileNameLength] = '\0';

    // Create file to start writing on it
    std::string savingPath = "C:\\Users\\Adam\\Desktop\\";
    std::string fullpath = savingPath + fileName;
    std::ofstream file(fullpath, std::ios::binary);
    if (!file) {
      std::cout << "[ FILE ]: " << fileName << " can't be created on path "
                << "\"" << savingPath << "\"" << std::endl;
    }

    // Receive file size
    long long fileSize;
    if (recv(client, (char *)&fileSize, sizeof(fileSize), 0) <= 0)
      break;

    // Receiving data and writing on the created file
    char buffer[4096];
    int bytesReceived = 0;
    while (bytesReceived < fileSize) {
      int toReceive =
          (int)std::min<long long>(fileSize - bytesReceived, sizeof(buffer));
      int r = recv(client, buffer, toReceive, 0);
      if (r <= 0)
        break;
      file.write(buffer, r);
      bytesReceived += r;
    }
    file.close();
    std::cout << "[ FILE ]: " << fileName
              << " received successfully from client, and is going to get "
                 "transfered to all the connected clients on the server"
              << std::endl;

    // Broadcast to other clients
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET c : clients) {
      if (c != client) {
        if (send(c, (char *)&fileNameLength, sizeof(fileNameLength), 0) <= 0)
          break;
        if (send(c, fileName, fileNameLength, 0) <= 0)
          break;
        if (send(c, (char *)&fileSize, sizeof(fileSize), 0) <= 0)
          break;
        std::ifstream file(fullpath, std::ios::binary);
        if (!file) {
          std::cout << "[ ERROR ]: Can't open the file " << fileName
                    << " to read binary data from" << std::endl;
          break;
        }
        char buffer[4096];
        file.seekg(0, std::ios::beg);
        while (!file.eof()) {
          file.read(buffer, sizeof(buffer));
          int bytesRead = file.gcount();
          send(c, buffer, bytesRead, 0);
        }
        std::cout << "[ FILE ]: " << fileName
                  << " successfully sent to all connected clients" << std::endl;
        file.close();
      }
    }
  }

  // Remove client
  std::lock_guard<std::mutex> lock(clientsMutex);
  clients.erase(std::remove(clients.begin(), clients.end(), client),
                clients.end());
  std::cout << "[ CLIENT ]: " << " client disconnected" << std::endl;
  closesocket(client);
}

void receiveFiles(SOCKET serverSocket) {

  std::cout << "- Waiting for clients to send..." << std::endl;

  // Receive file name length
  int fileNameLength = 0;
  if (recv(serverSocket, (char *)&fileNameLength, sizeof(fileNameLength), 0) <=
      0) {
    std::cout << "- Receiving file name length failed" << std::endl;
    return;
  }

  // Receive file name
  char fileName[512];
  if (recv(serverSocket, fileName, fileNameLength, 0) <= 0) {
    std::cout << "- Receiving file name failed" << std::endl;
    return;
  }
  fileName[fileNameLength] = '\0';

  // Create the file for writing
  std::string path = "D:\\Adam\\AirLink\\";
  std::string fullPath = path + fileName;
  std::ofstream file(fullPath, std::ios::binary);
  if (!file) {
    std::cout << "- The file " << fileName
              << " can't be created on path : " << "\"" << path << "\""
              << std::endl;
  }

  // Get file size on bytes
  long long fileSize;
  if (recv(serverSocket, (char *)&fileSize, sizeof(fileSize), 0) <= 0)
    return;

  // Get & write the file data
  char buffer[4096];
  int receivedBytes = 0;
  while (receivedBytes < fileSize) {
    int toReceive =
        (int)std::min<long long>(fileSize - receivedBytes, sizeof(buffer));
    int r = recv(serverSocket, buffer, toReceive, 0);
    if (r <= 0)
      break;
    file.write(buffer, r);
    receivedBytes += r;
  }
  file.close();
  std::cout << "- The file \"" << fileName << "\" received successfully"
            << std::endl;
}

void sendFiles(SOCKET serverSocket) {
  // Ask for file path
  std::string path;
  std::cout << "- Please enter file path: ";
  std::getline(std::cin, path);

  // Extract file name
  size_t fileNameIndex = path.find_last_of("\\/");
  std::string fileName = path.substr(fileNameIndex + 1);

  // Open the file and reads it in binary mode
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cout << "- Failed to open the file " << fileName << std::endl;
    return;
  }

  // Send file name length in network byte order
  int fileNameLength = fileName.size();
  if (!send(serverSocket, (char *)&fileNameLength, sizeof(fileNameLength), 0)) {
    std::cout << "- Failed to send file name length.\n";
    return;
  }

  // Send the file name itself
  if (!send(serverSocket, fileName.c_str(), fileNameLength, 0)) {
    std::cout << "- Failed to send file name.\n";
    return;
  }

  // Send file size (in bytes)
  file.seekg(0, std::ios::end);
  long long fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  send(serverSocket, (char *)&fileSize, sizeof(fileSize), 0);

  // Send file to server in binary
  char buffer[4096];
  while (!file.eof()) {
    file.read(buffer, sizeof(buffer));
    int bytesRead = file.gcount();
    send(serverSocket, buffer, bytesRead, 0);
  }
  std::cout << "- The file \"" << fileName
            << "\" sent successfully to server\n";
  file.close();
}
