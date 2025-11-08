#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

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
