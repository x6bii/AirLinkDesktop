#include <cstdio>
#include <fstream>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
  std::string receivingPath;
  std::getline(std::cin, receivingPath);
  WSADATA wsaData;
  int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (startupResult != 0) {
    std::cout << "[ ERROR ] : WsaStartup failed error code " << startupResult
              << std::endl;
    return 1;
  }
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
  while (true) {
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == SOCKET_ERROR) {
      std::cout << "[ ERROR ] : Accepting client failed, error code : "
                << WSAGetLastError() << std::endl;
      continue;
    }
    std::cout << "START" << std::endl;
    int fileNameLength;
    if (recv(clientSocket, (char *)&fileNameLength, sizeof(fileNameLength),
             0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    char fileName[256];
    if (recv(clientSocket, fileName, fileNameLength, 0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    fileName[fileNameLength] = '\0';
    std::cout << "NAME: " << fileName << std::endl;
    std::string fullPath = receivingPath + fileName;
    std::ofstream file(fullPath, std::ios::binary);
    if (!file) {
      std::cout << "ERROR" << std::endl;
      closesocket(clientSocket);
      break;
    }
    long long fileSize;
    if (recv(clientSocket, (char *)&fileSize, sizeof(fileSize), 0) <= 0) {
      std::cout << "BREAKED" << std::endl;
      closesocket(clientSocket);
      break;
    }
    char buffer[4096];
    int bytesReceived = 0;
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
    }
    file.close();
    std::cout << "DONE" << std::endl;
    closesocket(clientSocket);
    break;
  }
  closesocket(serverSocket);
  WSACleanup();
  return 0;
}