#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Broadcasting function
void broadcastingFunc(bool &working) {
  SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sockaddr_in broadcastAddr;
  broadcastAddr.sin_family = AF_INET;
  broadcastAddr.sin_port = htons(54001);
  broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;
  int enableBroadcast = 1;
  setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (char *)&enableBroadcast,
             sizeof(enableBroadcast));
  while (working) {
    const char *msg = "AIRLINK";
    sendto(udpSocket, msg, strlen(msg), 0, (sockaddr *)&broadcastAddr,
           sizeof(broadcastAddr));
    Sleep(500);
  }
  closesocket(udpSocket);
};

int main() {
  // Receiving path
  std::string receivingPath;
  std::getline(std::cin, receivingPath);
  // Winsock2 initialize
  WSADATA wsaData;
  int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (startupResult != 0) {
    std::cout << "[ ERROR ] : WsaStartup failed error code " << startupResult
              << std::endl;
    return 1;
  }
  // UDP thread
  bool udpBroadcasting = true;
  std::thread udpThread(broadcastingFunc, std::ref(udpBroadcasting));
  // TCP socket
  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == INVALID_SOCKET) {
    std::cout << "[ ERROR ] : Socket creation failed error code "
              << WSAGetLastError() << std::endl;
    WSACleanup();
    return 1;
  }
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(54000);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  int bindingStatus =
      bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
  if (bindingStatus == SOCKET_ERROR) {
    std::cout << "[ ERROR ] : Socket binding failed error code "
              << WSAGetLastError() << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  }
  int listenStatus = listen(serverSocket, SOMAXCONN);
  if (listenStatus == SOCKET_ERROR) {
    std::cout
        << "[ ERROR ] : Failed to put socket in listening mode, error code : "
        << WSAGetLastError() << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  } else {
    std::cout << "LISTENING" << std::endl;
  }
  // Receiving loop
  while (true) {
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == SOCKET_ERROR) {
      std::cout << "[ ERROR ] : Accepting client failed, error code : "
                << WSAGetLastError() << std::endl;
      continue;
    }
    std::cout << "START" << std::endl;
    Sleep(10);
    // Stop udp broadcasting
    udpBroadcasting = false;
    udpThread.join();
    // Get file name length
    int fileNameLength;
    if (recv(clientSocket, (char *)&fileNameLength, sizeof(fileNameLength),
             0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    // Get file name
    char fileName[256] = {0};
    if (recv(clientSocket, fileName, fileNameLength, 0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    fileName[fileNameLength] = '\0';
    std::cout << "NAME: " << fileName << std::endl;
    // Create file
    std::string fullPath = receivingPath + "\\" + fileName;
    std::ofstream file(fullPath, std::ios::binary);
    if (!file) {
      std::cout << "ERROR" << std::endl;
      closesocket(clientSocket);
      break;
    }
    // Get file size
    long long fileSize;
    if (recv(clientSocket, (char *)&fileSize, sizeof(fileSize), 0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    // Write on file
    char buffer[4096] = {0};
    long long totalSize = fileSize;
    int lastPercent = -1;
    long long bytesReceived = 0;
    while (bytesReceived < fileSize) {
      int toReceive =
          (int)std::min<long long>(fileSize - bytesReceived, sizeof(buffer));
      int bytesReceivedThisLoop = recv(clientSocket, buffer, toReceive, 0);
      if (bytesReceivedThisLoop <= 0) {
        std::cout << "BREAKED" << std::endl;
        file.close();
        std::remove(fullPath.c_str());
        closesocket(clientSocket);
        break;
      }
      file.write(buffer, bytesReceivedThisLoop);
      bytesReceived += bytesReceivedThisLoop;
      int percent = (int)((bytesReceived * 100) / totalSize);
      if (percent != lastPercent && percent % 5 == 0) {
        std::cout << percent << std::endl;
        lastPercent = percent;
      }
    }
    // Close file & cleanup
    file.close();
    Sleep(200);
    std::cout << "DONE" << std::endl;
    closesocket(clientSocket);
    break;
  }
  // Cleanup
  closesocket(serverSocket);
  WSACleanup();
  return 0;
}