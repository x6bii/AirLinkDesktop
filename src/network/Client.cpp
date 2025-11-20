#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// BreakThread function
void breakFunc(std::string &cmd) {
  while (true) {
    std::cin >> cmd;
    if (cmd == "break") {
      break;
    }
  }
};

int main() {
  // File path
  std::string filePath;
  std::getline(std::cin, filePath);
  // Winsock2 initialize
  WSADATA wsaData;
  int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (startupResult != 0) {
    std::cout << "[ ERROR ] : WsaStartup failed error code " << startupResult
              << std::endl;
    return 1;
  }
  // BreakThread
  std::string breakCmd;
  std::thread breakThread(breakFunc, std::ref(breakCmd));
  // UDP socket
  SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sockaddr_in localAddr;
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(54001);
  localAddr.sin_addr.s_addr = INADDR_ANY;
  bind(udpSocket, (sockaddr *)&localAddr, sizeof(localAddr));
  // Scanning loop
  while (breakCmd != "break") {
    std::string ipContainer;
    char buffer[256];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    int bytes = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                         (sockaddr *)&senderAddr, &senderAddrSize);
    if (bytes <= 0) {
      Sleep(5000);
      continue;
    }
    buffer[bytes] = '\0';
    ipContainer = inet_ntoa(senderAddr.sin_addr);
    std::cout << ipContainer << std::endl;
    Sleep(5000);
  }
  // Socket cleanup
  closesocket(udpSocket);
  breakThread.join();
  // TCP socket
  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == INVALID_SOCKET) {
    std::cout << "[ ERROR ] : Socket creation failed error code "
              << WSAGetLastError() << std::endl;
    WSACleanup();
    return 1;
  }
  // Connect socket
  std::string receiverIp;
  std::cin >> receiverIp;
  sockaddr_in receiverAddr;
  receiverAddr.sin_family = AF_INET;
  receiverAddr.sin_port = htons(54000);
  inet_pton(AF_INET, receiverIp.c_str(), &receiverAddr.sin_addr);
  int attemptsCount = 0;
  bool isConnected = false;
  // Loop to connect
  while (attemptsCount <= 5) {
    if (connect(serverSocket, (sockaddr *)&receiverAddr,
                sizeof(receiverAddr)) == SOCKET_ERROR) {
      std::cout << "[ ERROR ] : Couldn't connect to the server, error code "
                << WSAGetLastError() << std::endl;
      attemptsCount++;
      Sleep(1000);
      continue;
    }
    isConnected = true;
    break;
  }
  // Quit condition
  if (isConnected == false) {
    std::cout << "CAN'T CONNECT" << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 0;
  }
  // Sending loop
  while (true) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      std::cout << "ERROR" << std::endl;
      break;
    }
    std::cout << "START" << std::endl;
    size_t fileNameIndex = filePath.find_last_of("\\");
    std::string fileName = filePath.substr(fileNameIndex + 1);
    int fileNamelength = fileName.size();
    if (send(serverSocket, (char *)&fileNamelength, sizeof(fileNamelength),
             0) <= 0) {
      std::cout << "BREAK" << std::endl;
      break;
    }
    if (send(serverSocket, fileName.c_str(), fileNamelength, 0) <= 0) {
      std::cout << "BREAK" << std::endl;
      break;
    }
    file.seekg(0, std::ios::end);
    long long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (send(serverSocket, (char *)&fileSize, sizeof(fileSize), 0) <= 0) {
      std::cout << "BREAK" << std::endl;
      break;
    }
    char buffer[4096];
    while (!file.eof()) {
      file.read(buffer, sizeof(buffer));
      int bytesRead = file.gcount();
      if (send(serverSocket, buffer, bytesRead, 0) <= 0) {
        std::cout << "BREAK" << std::endl;
        break;
      }
    }
    file.close();
    std::cout << "DONE" << std::endl;
    break;
  }
  closesocket(serverSocket);
  WSACleanup();
  return 0;
}